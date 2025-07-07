#include "pch.h"
#include "Injection.h"
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
#include "Player.h"

Injection::Injection(ObjectManager* owner, ObjectType type, glm::vec2 startPos, float angle) : Object(owner, type), portalPos(startPos), angle(angle)
{
	portal = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");

	portalCover = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portalCover->PlaySprite("portal_front");
	portalMask = std::make_shared<TextureComponent>("portalMask", "Resource/Texture/Effect/Boss/portal/mask.png");
	injection = AddComponent<TextureComponent>("obj_injection", "Resource/Texture/Object/Boss/obj_injection/0.png");
	transform = AddComponent<TransformComponent>();
	transform->SetPosition(startPos + glm::vec2{ 0, 0 });
	transform->SetScale(2., 2.);
	transform->SetRotation(angle);
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 30, 30 });
	AABB->SetOffset(glm::vec2{cos(angle), sin(angle)} * 100.f);
	currentState = InjectionState::Wait;
	dir = { cos(angle), sin(angle) };
}

void Injection::Update(double dt)
{

	Object::Update(dt);
	switch (currentState)
	{
	case InjectionState::Wait:
	{
		timer += dt;
		if (timer > 0.)
		{
			portal->PlaySprite("portal_open");
			timer = 0;
			currentState = InjectionState::PortalOpen;
		}
	} break;
	case InjectionState::PortalOpen:
	{
		portal->Update(dt);
		portalCover->Update(dt);
		if (portal->IsDone())
		{
			timer = .0;
			portal->PlaySprite("portal_back");
			currentState = InjectionState::Appear;
		}
	} break;
	case InjectionState::Appear:
	{
		portal->Update(dt);
		portalCover->Update(dt);
		velocity = dir * 500.f;

		timer += dt;
		if (timer > 0.2)
		{
			timer = 0.;
			velocity.y = 0.;
			currentState = InjectionState::Back;
		}

	} break;
	case InjectionState::Back:
	{
		portal->Update(dt);
		portalCover->Update(dt);
		velocity = dir * -200.f;

		timer += dt;
		if (timer > 0.5)
		{
			timer = 0.0;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akirasword_fly_02");
			portal->PlaySprite("portal_close");
			currentState = InjectionState::Shot;
		}
	} break;

	case InjectionState::Shot:
	{
		portal->Update(dt);
		portalCover->Update(dt);
		velocity = dir * 1200.f;
		timer += dt;
		auto player = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());
		if (!player->isInvincible && !player->hit && AABB->CheckCollide(player))
		{
			player->hit = true;
			player->willDie = true;
			player->currentGround = nullptr;
			player->ChangeState(&player->stateHurtFlyBegin);
			player->SetVelocity({ velocity.x, 400 });
			Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
		}
		if (timer > 4.0)
		{
			SetDead();
		}
	} break;
	}
	transform->Translate(velocity * (float)dt);
}

void Injection::Render()
{
	if (currentState != InjectionState::Wait)
	{
		RenderInfo info{};
		glm::vec2 newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y };
		glm::mat4 portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 1.0));
		portalMat = glm::rotate(portalMat, -angle, glm::vec3(0.f, 0.f, 1.f));
		portalMat = glm::scale(portalMat, glm::vec3(-2.0, 2.0, 1.0));
		portal->Render(portalMat);


		glm::vec2 portalDir = { cos(-angle), sin(-angle) };
		newPortalPos = glm::vec2{ portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y } + portalDir * -11.f;
		portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 0.0));
		portalMat = glm::rotate(portalMat, -angle, glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 maskMat = glm::scale(portalMat, glm::vec3(-40.0, 52.0, 1.0));
		info.useMask = true;
		info.mask = Engine::GetInstance().GetTextureManager()->GetTexture(portalMask->GetKey());
		info.maskMat = maskMat;

		injection->Render(transform->GetModelMatrix(), info);

		if (currentState == InjectionState::Appear || currentState == InjectionState::Back)
		{
			portalMat = glm::scale(portalMat, glm::vec3(-2.0, 2.0, 1.0));
			portalCover->Render(portalMat);
		}
	}

	AABB->Render();

}
