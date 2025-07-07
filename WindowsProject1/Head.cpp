#include "pch.h"
#include "Head.h"
#include "SpriteManagerComponent.h"
#include "TextureComponent.h"
#include "TextureManager.h"
#include "TransformComponent.h"
#include "AudioManager.h"
#include "Engine.h"
#include "Mutation.h"
#include "ObjectManager.h"
#include "AABBCollisionComponent.h"
#include "CameraComponent.h"
#include "InputSystem.h"
#include "Random.h"
#include "HitEffect.h"
#include "BossFire.h"
Head::Head(ObjectManager* owner, ObjectType type, Mutation* mutation, glm::vec2 startPos) : Object(owner, type), mutation(mutation), portalPos(startPos)
{
	portal = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portal->PlaySprite("portal_open");
	portalCover = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portalCover->PlaySprite("portal_front");
	portalMask = std::make_shared<TextureComponent>("portalMask", "Resource/Texture/Effect/Boss/portal/mask.png");
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_head/obj_head.txt");
	sprite->PlaySprite("head_idle");
	transform = AddComponent<TransformComponent>();
	transform->SetPosition(startPos + glm::vec2{0, -100});
	transform->SetScale(2., 2.);
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 30, 30 });
	currentState = HeadState::PortalOpen;
	Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akiraportal_therapist_01");

}

void Head::Update(double dt)
{
	portal->Update(dt);
	portalCover->Update(dt);
	Object::Update(dt);
	switch (currentState)
	{
	case HeadState::PortalOpen:
	{
		if (portal->IsDone())
		{
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akiratherapist_emerge_01");

			portal->PlaySprite("portal_back");
			currentState = HeadState::Appear;
		}
	} break;
	case HeadState::Appear:
	{
		velocity.y = 500;

		timer += dt;
		if (timer > 0.35)
		{
			velocity.y = 0.;
			currentState = HeadState::Idle;
		}

	} break;
	case HeadState::Idle:
	{
		timer += dt;
		if (timer > 1.0)
		{
			timer = 0.0;
			currentState = HeadState::Attack;
			sprite->PlaySprite("head_attack");
			for (int i = 0; i < 18; i++)
			{
				auto fire = std::make_shared<BossFire>(owner, ObjectType::BossFire);
				fire->GetComponent<TransformComponent>()->SetPosition(transform->GetPosition() + glm::vec2(0, 50));
				double angle = i * PI / 9;
				fire->SetVelocity(glm::vec2(cos(angle), sin(angle)) * 800.f);
				owner->AddObject(ObjectType::BossFire, fire);
			}

		}
	} break;
	case HeadState::Attack:
	{
		if (sprite->IsDone())
		{
			currentState = HeadState::Idle;
		}
	} break;
	case HeadState::Hurt:
	{
		if (sprite->IsDone())
		{
			timer = 0.;
			currentState = HeadState::Disappear;
		}
	} break;
	case HeadState::Disappear:
	{
		velocity.y -= 500;

		timer += dt;
		if (timer > 0.35)
		{
			velocity.y = 0.;
			currentState = HeadState::PortalClose;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_arm_portal_02");
			portal->PlaySprite("portal_close");
		}
	} break;
	case HeadState::PortalClose:
	{
		if (portal->IsDone())
		{
			SetDead();
		}
	} break;
	}

	transform->Translate(velocity * (float)dt);

	{
		auto attacks = owner->GetObjectList(ObjectType::PBullet);
		for (auto a : attacks)
		{
			if (!hit && AABB->CheckCollide(a.get()))
			{
				hit = true;
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR * 1.5, SHAKE_MAG * 1.5);
				Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akira_hurt_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
				Engine::GetInstance().GetAudioManager()->StartSound("enemy_death_sword_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
				currentState = HeadState::Hurt;
				sprite->PlaySprite("head_hurt");

				if (mutation)
				{
					mutation->hp--;
					if (mutation->hp != 0)
					{
						mutation->sprite->PlaySprite("mutation_hurt");
						mutation->mutationState = Mutation::MutationState::Hurt;
					}
					else
					{
						Engine::GetInstance().GetAudioManager()->StartSound("boss_scream");
						mutation->sprite->PlaySprite("mutation_hurt");
						mutation->mutationState = Mutation::MutationState::Scream;
					}
				}
				{ // HitEffect ±¤¼±
					auto dir = Engine::GetInstance().GetInputSystem()->GetMousePos() + Engine::GetInstance().playerCam->GetAppliedPos() - a->GetComponent<TransformComponent>()->GetPosition();
					auto hitEffect = std::make_shared<HitEffect>(owner, ObjectType::HitEffect);
					auto hitEffectTransform = hitEffect->GetComponent<TransformComponent>();
					hitEffectTransform->SetPosition(transform->GetPosition());
					hitEffectTransform->SetRotation(atan2(dir.y, dir.x));
					hitEffectTransform->SetScale(4, 2);
					owner->AddObject(ObjectType::HitEffect, hitEffect);
				}
			}
		}
	}


	{
		auto attacks = owner->GetObjectList(ObjectType::BossFire);
		for (auto a : attacks)
		{
			auto attack = dynamic_cast<BossFire*>(a.get());
			if (!attack->hit && attack->reflected && !hit && GetComponent<AABBCollisionComponent>()->CheckCollide(attack))
			{
				hit = true;
				attack->Crash();
				currentState = HeadState::Hurt;
				Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akira_hurt_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
				sprite->PlaySprite("head_hurt");
				if (mutation)
				{
					mutation->hp--;
					if (mutation->hp != 0)
					{
						mutation->sprite->PlaySprite("mutation_hurt");
						mutation->mutationState = Mutation::MutationState::Hurt;
					}
					else
					{
						mutation->sprite->PlaySprite("mutation_hurt");
						mutation->mutationState = Mutation::MutationState::Scream;
					}
				}
			}
		}
	}
	
}

void Head::Render()
{
	RenderInfo info{};

	glm::vec2 newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y};
	glm::mat4 portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 1.0));
	portalMat = glm::rotate(portalMat, (float)PI / 2.f, glm::vec3(0.f, 0.f, 1.f));
	portalMat = glm::scale(portalMat, glm::vec3(2.0, 2.0, 1.0));
	portal->Render(portalMat);

	newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y + 11. };
	portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 0.0));
	portalMat = glm::rotate(portalMat, (float)PI / 2.f, glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 maskMat = glm::scale(portalMat, glm::vec3(40.0, 52.0, 1.0));
	info.useMask = true;
	info.mask = Engine::GetInstance().GetTextureManager()->GetTexture(portalMask->GetKey());
	info.maskMat = maskMat;
	if (currentState == HeadState::Hurt) info.color = { 1, 0, 0, 1 };
	sprite->Render(transform->GetModelMatrix(), info);
	
	if (currentState != HeadState::PortalOpen && currentState != HeadState::PortalClose)
	{
		portalMat = glm::scale(portalMat, glm::vec3(2.0, 2.0, 1.0));
		portalCover->Render(portalMat);
	}
}
