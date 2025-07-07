#include "pch.h"
#include "BossEye.h"
#include "SpriteManagerComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "AABBCollisionComponent.h"
#include "BossScene.h"
#include "Player.h"
#include "Engine.h"
#include "AudioManager.h"
#include "TextureManager.h"
#include "ObjectManager.h"
#include "CameraComponent.h"
#include "Random.h"
#include "HitEffect.h"
#include "InputSystem.h"
BossEye::BossEye(ObjectManager* owner, ObjectType type, BossScene* bossScene) : Object(owner, type), bossScene(bossScene)
{
	iris = std::make_shared<TextureComponent>("Boss_Iris", "Resource/Texture/Object/Boss/obj_eye/iris.png");
	eyeMask = std::make_shared<TextureComponent>("Boss_Eye_Mask", "Resource/Texture/Object/Boss/obj_eye/mask.png");
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_eye/obj_eye.txt");
	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);

	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 80, 40 });
}

void BossEye::Update(double dt)
{
	switch (eyeState)
	{
	case EyeState::Wait:
	{
		timer += dt;
		if (timer > 0.)
		{
			timer = 0.;
			eyeState = EyeState::Open;
			sprite->PlaySprite("eye_open");
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_eyesingle_open_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
		}
	} break;
	case EyeState::Open:
	{
		Object::Update(dt);

		if (sprite->IsDone())
		{
			irisPos = transform->GetPosition();
			eyeState = EyeState::Stare;
			sprite->PlaySprite("eye_stare");
		}
	} break;
	case EyeState::Stare:
	{
		Object::Update(dt);

		auto playerPos = bossScene->player->GetComponent<TransformComponent>()->GetPosition();
		auto myPos = transform->GetPosition();
		auto dir = playerPos - myPos;
		float norm = (dir.x * dir.x) / (eyeEllipseSize.x * eyeEllipseSize.x) + (dir.y * dir.y) / (eyeEllipseSize.y * eyeEllipseSize.y);
		if (norm < 1.0)
		{
			irisPos = playerPos;
		}
		else
		{
			irisPos = myPos + dir / std::sqrt(norm);
		}

		auto attacks = owner->GetObjectList(ObjectType::PBullet);
		for (auto a : attacks)
		{
			if (!hit && AABB->CheckCollide(a.get()))
			{
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
				Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_eyes_death_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
				eyeState = EyeState::Die;
				hit = true;
				sprite->PlaySprite("eye_die");
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

		if (sprite->IsDone())
		{
			if (!bossScene->player->hit)
			{
				bossScene->retry = true;
				bossScene->player->hit = true;
				bossScene->player->willDie = true;
				bossScene->player->ChangeState(&bossScene->player->stateHurtFlyBegin);
				for (int i = 0; i < 6; i++)
				{
					auto fire = std::make_shared<Particle>(owner, ObjectType::Effect);
					fire->SetTimer(5.0);
					fire->GetComponent<TransformComponent>()->SetScale(2, 2);
					auto sprite = fire->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/Fire/Fire.txt");
					sprite->PlaySprite("fire" + std::to_string(Engine::GetInstance().GetRandom()->get(0, 1)));
					float ranX = Engine::GetInstance().GetRandom()->get(-40, 40);
					float ranY = Engine::GetInstance().GetRandom()->get(-40, 40);
					fire->SetStartTimer(Engine::GetInstance().GetRandom()->get(0., 0.5));
					fire->SetHasOwner(true, bossScene->player->GetComponent<TransformComponent>(), { ranX, ranY });
					owner->AddObject(ObjectType::Effect, fire);
				}
			}
		}
	} break;
	case EyeState::Die:
	{
		Object::Update(dt);

		if (sprite->IsDone())
		{
			SetDead();
		}
	} break;
	}
}

void BossEye::Render()
{
	if (!isDead)
	{
		if (eyeState != EyeState::Wait) Object::Render();
		if (eyeState == EyeState::Stare)
		{
			glm::vec2 newPos = glm::vec2(irisPos.x, Engine::GetInstance().GetWindowSize().bottom - irisPos.y);
			glm::mat4 irisMat = glm::translate(glm::mat4(1.0), glm::vec3(newPos, 0.0));
			irisMat = glm::scale(irisMat, glm::vec3(2.0, 2.0, 1.0));
			RenderInfo info{};
			info.useMask = true;
			info.mask = Engine::GetInstance().GetTextureManager()->GetTexture(eyeMask->GetKey());
			glm::vec2 newMaskPos = glm::vec2(transform->GetPosition().x, Engine::GetInstance().GetWindowSize().bottom - transform->GetPosition().y);
			glm::mat4 eyeMaskMat = glm::translate(glm::mat4(1.0), glm::vec3(newMaskPos, 0.0));
			eyeMaskMat = glm::scale(eyeMaskMat, glm::vec3(57.0, 36.0, 1.0));
			info.maskMat = eyeMaskMat;
			iris->Render(irisMat, info);
		}
	}

}
