#include "pch.h"
#include "Fanblade.h"
#include "SpriteManagerComponent.h"
#include "TextureComponent.h"

#include "Player.h"
#include "ObjectManager.h"
#include "TransformComponent.h"
#include "AABBCollisionComponent.h"

#include "Engine.h"
#include "AudioManager.h"
#include "HitEffect.h"
Fanblade::Fanblade(ObjectManager* owner, ObjectType objType) : Object(owner, objType)
{
	p = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());
	auto transform = AddComponent<TransformComponent>();
	transform->SetPosition(450, 250);
	transform->SetScale(2, 2);
	auto collision = AddComponent<AABBCollisionComponent>();
	collision->SetSize({5, 180});
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_fan/FanbladeSpt.txt");
	sprite->PlaySprite("Fanblade_");

	texture = AddComponent<TextureComponent>("obj_fanblade_cover_", "Resource/Texture/Object/obj_fan/spr_fan_fg_0.png");
	Engine::GetInstance().GetAudioManager()->StartSound("object_fan_loop", false);
	Engine::GetInstance().GetAudioManager()->SetVolume("object_fan_loop", 0.1);
}

void Fanblade::Update(double dt)
{
	dt *= p->currentSlow;
	Object::Update(dt);
	int shape = sprite->GetCurrentSprite()->GetCurrentFrame();
	if (shape > 3 && shape <= 29)
	{
		isThroughable = true;
	}
	else
	{
		isThroughable = false;
	}

	if (!isThroughable && GetComponent<AABBCollisionComponent>()->CheckCollide(p))
	{
		if (p->currState != &p->stateHurtFlyBegin)
		{
			Engine::GetInstance().GetAudioManager()->StartSound("enemy_punch_hit");

			if (p->currState == &p->stateRoll)
			{
				p->velocity = { -1000, 0 };
			}
			else
			{
				{ // HitEffect ±¤¼±

					auto dir = p->GetComponent<TransformComponent>()->GetPosition().x > GetComponent<TransformComponent>()->GetPosition().x ? glm::vec2{1.0, 0.0} : glm::vec2{-1.0, 0.0};
					auto hitEffect = std::make_shared<HitEffect>(owner, ObjectType::BackgroundEffect);
					auto hitEffectTransform = hitEffect->GetComponent<TransformComponent>();
					hitEffectTransform->SetPosition(p->GetComponent<TransformComponent>()->GetPosition());
					hitEffectTransform->SetRotation(atan2(dir.y, dir.x));
					hitEffectTransform->SetScale(4, 2);
					owner->AddObject(ObjectType::BackgroundEffect, hitEffect);
				}
				p->currentGround = nullptr;
				p->willDie = true;
				p->velocity = { -1500, 150 };
			}
			p->ChangeState(&p->stateHurtFlyBegin);
			auto transform = p->GetComponent<TransformComponent>();
			if (transform->GetScale().x < 0.) p->GetComponent<TransformComponent>()->ScaleBy(-1, 1);
		}
	}
}

void Fanblade::Render()
{
	RenderInfo info{};
	info.color *= 0.9;
	if (!isThroughable && p->currentSlow != 1.0)
	{
		info.color = glm::vec4(1.0f, 0.0f, 0.0f, 0.8f);
	}
	sprite->Render(info);
	auto transform = GetComponent<TransformComponent>();
	transform->Translate(32, 5);
	if (p->currentSlow != 1.0)
	{
		info.color = glm::vec4(glm::vec3(p->currentSlow), 0.8f);
	}
	texture->Render(transform->GetModelMatrix(), info);
	transform->Translate(-32, -5);
	if (auto AABB = GetComponent<AABBCollisionComponent>())
	{
		AABB->Render();
	}
}
