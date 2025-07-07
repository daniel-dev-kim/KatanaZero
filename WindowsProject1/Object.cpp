#include "pch.h"
#include "Object.h"

#include "Engine.h"
#include "ObjectManager.h"

#include "TransformComponent.h"
#include "CameraComponent.h"
#include "TextureComponent.h"
#include "SpriteManagerComponent.h"
#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"
#include "ShaderProgram.h"
#include "TextureManager.h"


void Afterimage::Render(ShaderProgram* shader, CameraComponent* camera)
{
	shader->Use();

	glm::vec2 texSize = glm::vec2(texture->width, texture->height);

	glm::mat4 m = glm::scale(glm::mat4(1.0f), { texSize, 1. });
	shader->SetUniform("uModel", modelMat * m);
	shader->SetUniform("uView", camera->GetViewMatrix());
	shader->SetUniform("uProjection", camera->GetProjectionMatrix());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->textureID);
	shader->SetUniform("uTexture1", 0);

	if (info.texture2)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, info.texture2->textureID);
		shader->SetUniform("uTexture2", 1);
		shader->SetUniform("uBlendFactor", 0.6f);
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
	else shader->SetUniform("uIsSmokeEffect", false);
	
	shader->SetUniform("uWorldRenderLowerBound", info.worldRenderLowerBound);
	shader->SetUniform("uWorldRenderUpperBound", info.worldRenderUpperBound);
	shader->SetUniform("uUseBounding", info.useBounding);
	shader->SetUniform("uUseMask", info.useMask);
	shader->SetUniform("uUVOffset", glm::vec2(0.f));
	shader->SetUniform("uUVSize", glm::vec2(1.f));

	shader->SetUniform("uUseTexture", info.useTexture);
	shader->SetUniform("uIsAfterimage", true);
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

Object::Object(ObjectManager* owner, ObjectType objType) : owner(owner), objType(objType), isDead(false), currState(&stateNothing) 
{
}

void Object::Update(double dt)
{
	for (auto& component : components)
	{
		component->Update(dt);
	}	
	currState->Update(this, dt);
	currState->TestForExit(this);
}

void Object::Render()
{
	auto sprite = GetComponent<SpriteManagerComponent>();
	if (sprite)
	{
		if (useSpecial)
		{
			sprite->Render(specialInfo);
		}
		else
		{
			sprite->Render();
		}
	}

	auto texture = GetComponent<TextureComponent>();
	if (texture)
	{
		if (useSpecial)
		{
			texture->Render(GetComponent<TransformComponent>()->GetModelMatrix(), specialInfo);
		}
		else
		{
			texture->Render(GetComponent<TransformComponent>()->GetModelMatrix());
		}
	}

	
	if (auto AABB = GetComponent<AABBCollisionComponent>())
	{
		AABB->Render();
	}  
	if (auto Line = GetComponent<LineCollisionComponent>())
	{
		Line->Render();
	}
}

bool Object::IsRecorded()
{
	switch (objType)
	{
	case ObjectType::BackgroundEffect:
	case ObjectType::Item:
	case ObjectType::Enemy:
	case ObjectType::EnemyArm:
	case ObjectType::Door:
	case ObjectType::Player:
	case ObjectType::Laser:
	case ObjectType::Neutral3:
	case ObjectType::Effect:
	case ObjectType::Blood2:
	case ObjectType::Blood3:
	case ObjectType::EBullet:
	case ObjectType::PBullet:
	{
		return true;
	}
	default:
		return false;
	}
}

void Object::AddVelocity(glm::vec2 v)
{
	velocity += v;
}

void Object::SetVelocity(glm::vec2 v)
{
	velocity = v;
}

glm::vec2 Object::GetVelocity()
{
	return velocity;
}

ObjectSnapshot Object::GetSnapshot()
{
	ObjectSnapshot snapshot{};
	snapshot.dummy = false;
	auto transform = GetComponent<TransformComponent>();
	snapshot.position = transform->GetPosition();
	snapshot.scale = transform->GetScale();
	snapshot.rotate = transform->GetRotation();
	snapshot.isDead = isDead;
	if (auto sprite = GetComponent<SpriteManagerComponent>())
	{
		snapshot.spriteFrame = sprite->GetCurrentSprite()->GetCurrentFrame();
		snapshot.spriteName = sprite->GetName();
	}
	return snapshot;
}

void Object::LoadSnapshot(const ObjectSnapshot& snapshot)
{
	auto transform = GetComponent<TransformComponent>();
	transform->SetPosition(snapshot.position);
	transform->SetScale(snapshot.scale);
	transform->SetRotation(snapshot.rotate);
	isDead = snapshot.isDead;
	auto sprite = GetComponent<SpriteManagerComponent>();
	if (!isDead && sprite)
	{
		sprite->PlaySprite(snapshot.spriteName);
		sprite->GetCurrentSprite()->SetSpriteFrame(snapshot.spriteFrame);
	}
}

void Object::ChangeState(State* newState)
{
	currState = newState;
	currState->Enter(this);
}