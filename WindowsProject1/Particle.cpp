#include "pch.h"
#include "Particle.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "SpriteManagerComponent.h"
#include "AABBCollisionComponent.h"
#include "Engine.h"
Particle::Particle(ObjectManager* owner, ObjectType objType) : Object(owner, objType), timer(0.)
{
	AddComponent<TransformComponent>();
}

void Particle::SetVelocity(glm::vec2 v)
{
	velocity = v;
}

void Particle::SetTimer(double timer)
{
	deadTime = timer;
}

void Particle::SetStartTimer(double timer)
{
	this->timer = -timer;
}

void Particle::SetIsDeadBySpriteEnd(bool input)
{
	isDeadBySpriteEnd = input;
}

void Particle::SetIsGettingDisappearByTime(bool i)
{
	isGettingDisappearByTime = i;
}

void Particle::SetGravityFactor(float u)
{
	gravityFactor = u;
}

void Particle::SetIgnoreSlow(bool i)
{
	ignoreSlow = i;
}

void Particle::Update(double dt)
{
	if (ignoreSlow) dt /= Engine::GetInstance().dtFactor;
	if (timer >= 0.)
	{
		Object::Update(dt);
	}
	timer += dt;
	if (isGettingDisappearByTime)
	{
		info.color.a -= dt;
	}
	if (isDeadBySpriteEnd && GetComponent<SpriteManagerComponent>()->IsDone() || !isDeadBySpriteEnd && timer > deadTime)
	{
		SetDead();
	}
	if (hasOwner)
	{
		GetComponent<TransformComponent>()->SetPosition(ownerTransform->GetPosition() + ownerOffset);
	}
	else
	{
		velocity.y -= GRAVITY * gravityFactor * dt;
		GetComponent<TransformComponent>()->Translate(velocity * (float)dt);
	}
}

void Particle::Render()
{
	auto transform = GetComponent<TransformComponent>();
	if (objType == ObjectType::BackgroundEffect)
	{
		RenderInfo temp{ info };
		temp.color *= glm::vec4(glm::vec3(Engine::GetInstance().dtFactor), info.color.a);
		if (auto sprite = GetComponent<SpriteManagerComponent>())
		{
			sprite->Render(temp);
		}

		if (auto texture = GetComponent<TextureComponent>())
		{
			texture->Render(transform->GetModelMatrix(), temp);
		}
	}
	else
	{
		if (auto sprite = GetComponent<SpriteManagerComponent>())
		{
			sprite->Render(info);
		}

		if (auto texture = GetComponent<TextureComponent>())
		{
			texture->Render(transform->GetModelMatrix(), info);
		}
	}
	if (auto AABB = GetComponent<AABBCollisionComponent>())
	{
		AABB->Render();
	}
}

void Particle::SetRenderInfo(RenderInfo i)
{
	info = i;
}

void Particle::SetHasOwner(bool h, TransformComponent* o, glm::vec2 p)
{
	hasOwner = h;
	ownerTransform = o;
	ownerOffset = p;
}
