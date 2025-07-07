#include "pch.h"
#include "TextureComponent.h"

#include "Engine.h"
#include "TextureManager.h"
#include "ShaderProgram.h"
#include "CameraComponent.h"
TextureComponent::TextureComponent(const std::string& key, const std::string& filePath) : key(key) 
{
    shader = Engine::GetInstance().GetShaderProgram();
    camera = Engine::GetInstance().playerCam;
    Engine::GetInstance().GetTextureManager()->LoadTexture(key, filePath);
    size = { Engine::GetInstance().GetTextureManager()->GetTexture(key)->width, Engine::GetInstance().GetTextureManager()->GetTexture(key)->height };
}

TextureComponent::TextureComponent(const std::string& key) : key(key)
{
    shader = Engine::GetInstance().GetShaderProgram();
    camera = Engine::GetInstance().playerCam;
    size = { Engine::GetInstance().GetTextureManager()->GetTexture(key)->width, Engine::GetInstance().GetTextureManager()->GetTexture(key)->height };
}

void TextureComponent::Render(glm::mat4 modelMat, RenderInfo info)
{
    shader->Use();

    auto texture = Engine::GetInstance().GetTextureManager()->GetTexture(key);
    glm::vec2 texSize = glm::vec2(texture->width, texture->height);
    if (info.frameSize == glm::vec2{ 0.0, 0.0 }) info.frameSize = texSize;

    glm::mat4 m = glm::scale(glm::mat4(1.0f), { info.frameSize, 1. });
    shader->SetUniform("uModel", modelMat * m);
    shader->SetUniform("uView", (info.isFixed || camera == nullptr) ? glm::mat4(1.0) : camera->GetViewMatrix());
    shader->SetUniform("uProjection", (camera == nullptr) ? glm::ortho(0.0f, 1280.f, 720.f, 0.0f, -1.0f, 1.0f) : camera->GetProjectionMatrix());

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
    } else shader->SetUniform("uIsSmokeEffect", false);


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

glm::vec2 TextureComponent::GetSize()
{
    return size;
}

std::string TextureComponent::GetKey()
{
    return key;
}
