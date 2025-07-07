#include "pch.h"
#include "Smoke.h"

#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "AABBCollisionComponent.h"

#include "Engine.h"
#include "Random.h"

#include "ObjectManager.h"

#include "Enemy.h"
#include "EnemyMark.h"
#include "Player.h"
#include "Warning.h"
#include "Light.h"
Smoke::Smoke(ObjectManager* owner, ObjectType objType, double startTime) : Object(owner, objType), timer(startTime)
{
	transform = AddComponent<TransformComponent>();
	transform->SetScale(Engine::GetInstance().GetRandom()->get(2.5, 3.), Engine::GetInstance().GetRandom()->get(2.0, 2.5));
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 60., 60. });

	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_smoke/SmokeSpriteData.txt");
	currState = SmokeState::Wait;

	dieTime = Engine::GetInstance().GetRandom()->get(3., 4.);

	auto lights = owner->GetObjectList(ObjectType::Light);
	Light* targetLight = nullptr;
	float targetDist = 10000.f;
	for (auto light : lights)
	{
		float dist = glm::distance(GetComponent<TransformComponent>()->GetPosition(), light->GetComponent<TransformComponent>()->GetPosition());
		if (dist < 200 && dist < targetDist)
		{
			targetDist = dist;
			targetLight = dynamic_cast<Light*>(light.get());
		}
	}
	if (targetLight)
	{
		color = targetLight->GetColor();
	}
	else
	{
		color = glm::vec3(1.0);
	}
}

void Smoke::Update(double dt)
{
	switch (currState)
	{
	case SmokeState::Wait:
	{
		timer += dt;
		if (timer > 0.0)
		{
			currState = SmokeState::Appear;
			sprite->PlaySprite("Smoke_Appear_");
		}
	} break;
	case SmokeState::Appear:
	{
		Object::Update(dt);

		if (sprite->IsDone())
		{
			currState = SmokeState::Loop;
			sprite->PlaySprite("Smoke_Loop_");
		}
	} break;
	case SmokeState::Loop:
	{
		Object::Update(dt);

		auto enemies = owner->GetObjectList(ObjectType::Enemy);
		for (auto e : enemies)
		{
			auto enemy = dynamic_cast<Enemy*>(e.get());
			if (enemy->hit && enemy->willDie) continue;
			if (AABB->CheckCollide(enemy->GetComponent<TransformComponent>()->GetPosition()))
			{
				enemy->inSmoke = true;
			}
		}

		auto p = owner->GetFrontObject(ObjectType::Player);
		if (AABB->CheckCollide(p->GetComponent<TransformComponent>()->GetPosition()))
		{
			auto player = dynamic_cast<Player*>(p.get());
			player->hidden = true;
		}
		timer += dt;
		if (timer > dieTime)
		{
			currState = SmokeState::Dissolve;
			sprite->PlaySprite("Smoke_Dissolve_");
		}
	} break;
	case SmokeState::Dissolve:
	{
		Object::Update(dt);

		if (sprite->IsDone())
		{
			SetDead();
		}
	} break;
	}
}

void Smoke::Render()
{
	if (currState != SmokeState::Wait) 
	{
		RenderInfo info{};
		info.isSmoke = true;
		info.color.a = 0.8;
		info.lightColor = color;

		sprite->Render(transform->GetModelMatrix(), info);
	}
}
