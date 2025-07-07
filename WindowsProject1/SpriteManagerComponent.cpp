#include "pch.h"
#include "SpriteManagerComponent.h"

#include "Object.h"
#include "TransformComponent.h"

#include "Engine.h"
#include "TextureManager.h"
#include "ShaderProgram.h"
#include "CameraComponent.h"

void Sprite::Reset()
{
    timer = 0.;
    currentFrame = 0;
}

void Sprite::Update(double dt)
{
    if (currentFrame < totalFrame)
    {
        timer += dt * speed;
    }

	while (timer > 1.)
	{
		timer -= 1.;
		currentFrame++;
	}

    if (currentFrame >= totalFrame)
    {
        if (isRepeat)
        {
            currentFrame = 0;
        }
        else
        {
            currentFrame--;
        }
    }
}

void Sprite::Render(glm::mat4 modelMat, RenderInfo info)
{
    
    shader->Use();
    if (currentFrame >= totalFrame) return;
    auto texture = Engine::GetInstance().GetTextureManager()->GetTexture(spriteName + std::to_string(currentFrame));
    glm::vec2 texSize = glm::vec2(texture->width, texture->height);
    if (info.frameSize == glm::vec2{ 0.0, 0.0 }) info.frameSize = texSize;
    glm::mat4 m = glm::scale(glm::mat4(1.0f), { info.frameSize, 1. });
    shader->SetUniform("uModel", modelMat * m);
    shader->SetUniform("uView", (info.isFixed || camera == nullptr) ? glm::mat4(1.0) : camera->GetViewMatrix());
    shader->SetUniform("uProjection", (camera == nullptr) ? glm::ortho(0.0f,1280.f,720.f,0.0f,-1.0f,1.0f) : camera->GetProjectionMatrix());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->textureID);
    shader->SetUniform("uTexture1", 0);

    if (info.texture2)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, info.texture2->textureID);
        shader->SetUniform("uTexture2", 1);
        shader->SetUniform("uBlendFactor", info.blendFactor);
    }
    else
    {
        shader->SetUniform("uBlendFactor", 0.0f);
    }

    if (info.isSmoke)
    {
        shader->SetUniform("uIsSmokeEffect", true);
        shader->SetUniform("uScreenSize", glm::vec2(Engine::GetInstance().GetOriginalWindowSize().right, Engine::GetInstance().GetOriginalWindowSize().bottom));

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Engine::GetInstance().maskColorTexture);
        shader->SetUniform("uObjectMask", 2);
    }

    glm::vec2 uvOffset = info.texelPos / texSize;
    glm::vec2 uvSize = info.frameSize / texSize;

    shader->SetUniform("uWorldRenderLowerBound", info.worldRenderLowerBound);
    shader->SetUniform("uWorldRenderUpperBound", info.worldRenderUpperBound);
    shader->SetUniform("uUseBounding", info.useBounding);
    shader->SetUniform("uUVOffset", uvOffset);
    shader->SetUniform("uUVSize", uvSize);

    shader->SetUniform("uUseMask", info.useMask);
    if (info.useMask)
    {
        shader->SetUniform("uMaskModelInverse", glm::inverse(info.maskMat));
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, info.mask->textureID);
        shader->SetUniform("uMask", 3);
    }

    if (info.uEnableObjectWave)
    {
        shader->SetUniform("uObjectWaveStrength", info.uObjectWaveStrength);
        shader->SetUniform("uObjectWaveWidth", info.uObjectWaveWidth);
        shader->SetUniform("uObjectWaveOffset", info.uObjectWaveOffset);
        shader->SetUniform("uEnableObjectWave", info.uEnableObjectWave);
    }
    else
    {
        shader->SetUniform("uObjectWaveStrength", 0.f);
        shader->SetUniform("uObjectWaveWidth", 0.f);
        shader->SetUniform("uObjectWaveOffset", 0.f);
        shader->SetUniform("uEnableObjectWave", 0);
    }
    if (info.uEnableObjectGlitch)
    {
        shader->SetUniform("uEnableObjectGlitch", info.uEnableObjectGlitch);
        shader->SetUniform("uObjectGlitchSeed", info.uObjectGlitchSeed);
    }
    else
    {
        shader->SetUniform("uEnableObjectGlitch", 0);
        shader->SetUniform("uObjectGlitchSeed", 0.f);
    }
    if (info.uEnableTVNoise)
    {
        shader->SetUniform("uEnableTVNoise", info.uEnableTVNoise);
        shader->SetUniform("uTVNoiseWidth", info.uTVNoiseWidth);
    }
    else
    {
        shader->SetUniform("uEnableTVNoise", 0);
        shader->SetUniform("uTVNoiseWidth", 0.f);
    }
    shader->SetUniform("uUseTexture", info.useTexture);
    shader->SetUniform("uIsAfterimage", false);
    shader->SetUniform("uColor", info.color);
    shader->SetUniform("uFillRatio", info.renderRatio);
    shader->SetUniform("uUseLighting", info.useLight);
    shader->SetUniform("uLightPos", info.lightPos);
    shader->SetUniform("uLightPower", info.lightPower);
    shader->SetUniform("uLightColor", info.lightColor);

    shader->SetUniform("uUseGlowEffect", info.useGlowEffect);
    shader->SetUniform("uTime", info.timer);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Sprite::SetSpriteFrame(int f)
{
    currentFrame = f;
}

TextureData* Sprite::GetCurrentData()
{
    return Engine::GetInstance().GetTextureManager()->GetTexture(spriteName + std::to_string(currentFrame));
}

int Sprite::GetCurrentFrame()
{
    return currentFrame;
}

bool Sprite::isDone()
{
    return currentFrame == totalFrame - 1;
}

Sprite::Sprite(const std::string& name, int totalFrame, double speed, bool isRepeat) : spriteName(name), totalFrame(totalFrame), speed(speed), isRepeat(isRepeat), timer(0.), currentFrame(0) 
{
    shader = Engine::GetInstance().GetShaderProgram();
    camera = Engine::GetInstance().playerCam;
}

SpriteManagerComponent::SpriteManagerComponent(const std::string& filePath)
{
    LoadSprites(filePath);
}

void SpriteManagerComponent::LoadSprites(const std::string& filePath)
{
    //로드 함수
    //Engine::GetInstance().GetTextureManager()->LoadTexture(//key, filePath);
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << '\n';
        return;
    }

    std::string line;

    // 첫 줄: 총 개수 (사용하지 않아도 무방)
    std::getline(file, line);
    int totalSprites = std::stoi(line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name, path, countStr, repeatStr, speedStr;

        std::getline(ss, name, ',');
        std::getline(ss, path, ',');
        std::getline(ss, countStr, ',');
        std::getline(ss, repeatStr, ',');
        std::getline(ss, speedStr);

        int totalFrame = std::stoi(countStr);
        bool isRepeat = (repeatStr == "1");
        double speed = std::stod(speedStr);
        for (int i = 0; i < totalFrame; i++)
        {
            Engine::GetInstance().GetTextureManager()->LoadTexture(name + std::to_string(i), path + "/" + std::to_string(i) + ".png");
        }
        sprites.insert_or_assign(name, Sprite(name, totalFrame, speed, isRepeat));
    }
}

void SpriteManagerComponent::Update(double dt)
{
    if(!freeze) sprites.at(playedSpriteName).Update(dt);
}

bool SpriteManagerComponent::IsDone()
{
    return playedSpriteName.empty() || sprites.at(playedSpriteName).isDone();
}

void SpriteManagerComponent::Render(RenderInfo info)
{
    sprites.at(playedSpriteName).Render( owner->GetComponent<TransformComponent>()->GetModelMatrix(), info);
}

void SpriteManagerComponent::Render(glm::mat4 modelMat, RenderInfo info)
{
    if (playedSpriteName.empty()) return;
    sprites.at(playedSpriteName).Render(modelMat, info);
}

void SpriteManagerComponent::PlaySprite(const std::string& name)
{
    playedSpriteName = name;
    sprites.at(playedSpriteName).Reset();
}

void SpriteManagerComponent::Freeze(bool f)
{
    freeze = f;
}

Sprite* SpriteManagerComponent::GetCurrentSprite()
{
   return &(sprites[playedSpriteName]);
}

std::string SpriteManagerComponent::GetName() const
{
    return playedSpriteName;
}
