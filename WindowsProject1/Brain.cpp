#include "pch.h"
#include "Brain.h"
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
#include "BossScene.h"
#include "Player.h"
Brain::Brain(ObjectManager* owner, ObjectType type, glm::vec2 startPos, Player* player) : Object(owner, type), portalPos(startPos), player(player)
{
	portal = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portal->PlaySprite("portal_open");
	portalCover = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portalCover->PlaySprite("portal_front");
	portalMask = std::make_shared<TextureComponent>("portalMask", "Resource/Texture/Effect/Boss/portal/mask.png");
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_brain/obj_brain.txt");
	sprite->PlaySprite("brain_idle");
	transform = AddComponent<TransformComponent>();
	transform->SetPosition(startPos + glm::vec2{ 0, 0 });
	transform->SetScale(2., 2.);
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 30, 30 });
	currentState = BrainState::PortalOpen;
	Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akiraportal_orb_01");
	moveperiod = Engine::GetInstance().GetRandom()->get(0.7, 1.3);
}

void Brain::Update(double dt)
{
	portal->Update(dt);
	portalCover->Update(dt);
	Object::Update(dt);
	switch (currentState)
	{
	case BrainState::PortalOpen:
	{
		if (portal->IsDone())
		{
			portal->PlaySprite("portal_back");
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akiraorb_emerge_01");
			currentState = BrainState::Appear;
		}
	} break;
	case BrainState::Appear:
	{
		velocity.y = 500;

		timer += dt;
		if (timer > 0.35)
		{
			timer = 0.;
			velocity.y = 0.;
			portal->PlaySprite("portal_close");
			currentState = BrainState::Idle;
			targetPos.x = Engine::GetInstance().GetRandom()->get(180, 1100);
			targetPos.y = Engine::GetInstance().GetRandom()->get(300, 450);
			velocity = glm::normalize(targetPos - transform->GetPosition()) * 150.f;

		}

	} break;
	case BrainState::Idle:
	{
		if (glm::distance(targetPos, transform->GetPosition()) < 10)
		{
			targetPos.x = Engine::GetInstance().GetRandom()->get(180, 1100);
			targetPos.y = Engine::GetInstance().GetRandom()->get(300, 450);
			velocity = glm::normalize(targetPos - transform->GetPosition()) * 150.f;

		}
		timer += dt;
		// 상태 전이
		if (timer > moveperiod)
		{
			timer = 0.0;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akiraorb_shot_01");
			currentState = BrainState::Attack;
			sprite->PlaySprite("brain_attack");
		}
	} break;

	case BrainState::Attack:
	{
		velocity = { 0, 0 };
		if (sprite->IsDone())
		{
			moveperiod = Engine::GetInstance().GetRandom()->get(0.7, 1.3);
			currentState = BrainState::Idle;
			auto fire = std::make_shared<BossFire>(owner, ObjectType::BossFire);
			fire->GetComponent<TransformComponent>()->SetPosition(transform->GetPosition());
			auto dir = glm::normalize(player->GetComponent<TransformComponent>()->GetPosition() - transform->GetPosition());
			fire->SetVelocity(dir * 800.f);
			owner->AddObject(ObjectType::BossFire, fire);
		}
	} break;
	case BrainState::Die:
	{
		if (sprite->IsDone())
		{
			SetDead();
		}
	} break;
	}
	transform->Translate(velocity * (float)dt);
	
	auto attacks = owner->GetObjectList(ObjectType::BossFire);
	for (auto a : attacks)
	{
		auto attack = dynamic_cast<BossFire*>(a.get());
		if (!attack->hit && attack->reflected && !hit && GetComponent<AABBCollisionComponent>()->CheckCollide(attack))
		{
			hit = true;
			attack->Crash();
			currentState = BrainState::Die;
			sprite->PlaySprite("brain_die");
		}
	}
}

void Brain::Render()
{
	RenderInfo info{};

	glm::vec2 newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y };
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
	sprite->Render(transform->GetModelMatrix(), info);

	if (currentState == BrainState::Appear)
	{
		portalMat = glm::scale(portalMat, glm::vec3(2.0, 2.0, 1.0));
		portalCover->Render(portalMat);
	}
}
