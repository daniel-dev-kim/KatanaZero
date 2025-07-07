#include "pch.h"
#include "BossDoppleganger.h"

#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "AABBCollisionComponent.h"
#include "ObjectManager.h"

#include "Engine.h"
#include "Random.h"
#include "AudioManager.h"
#include "BossScene.h"
#include "TextureManager.h"
#include "BossFire.h"
#include "CameraComponent.h"
#include "Player.h"

BossDoppleGanger::BossDoppleGanger(ObjectManager* owner, ObjectType type, BossScene* bossScene) : Object(owner, type), bossScene(bossScene)
{
	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_boss_doppleganger/obj_boss_doppleganger.txt");
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 30, 80 });

	ChangeState(BossState::Glasses);
}

void BossDoppleGanger::Update(double dt)
{
	Object::Update(dt);

	switch (currState)
	{
	case BossState::Glasses:
	{
		//sprite->PlaySprite("boss_glasses");
	} break;
	case BossState::Float:
	{
		//sprite->PlaySprite("boss_float_loop");
	} break;
	case BossState::Appear:
	{
		if (sprite->IsDone())
		{
			ChangeState(BossState::Charge);
		}
	} break;
	case BossState::Charge:
	{
		if (sprite->IsDone())
		{
			if (bossScene->phase2Pattern == BossScene::Phase2Pattern::ColumnAttack)
			{
				ChangeState(BossState::Fire);
				auto fire = std::make_shared<BossFire>(owner, ObjectType::BossFire);
				fire->GetComponent<TransformComponent>()->SetPosition(transform->GetPosition() - glm::vec2{ 0.f, 10.f });
				auto dir = glm::vec2(0, -1);
				fire->SetVelocity(dir * 800.f);
				owner->AddObject(ObjectType::BossFire, fire);
			}
			else
			{
				ChangeState(BossState::Fire);
				auto fire = std::make_shared<BossFire>(owner, ObjectType::BossFire);
				fire->GetComponent<TransformComponent>()->SetPosition(transform->GetPosition() - glm::vec2{ 0.f, 10.f });
				auto dir = glm::normalize(bossScene->player->GetComponent<TransformComponent>()->GetPosition() - transform->GetPosition());
				fire->SetVelocity(dir * 800.f);
				owner->AddObject(ObjectType::BossFire, fire);
			}
		}
	} break;
	case BossState::Fire:
	{
		if (sprite->IsDone())
		{
			floatTimer += dt;
		}
		if (floatTimer > 0.5)
		{
			floatTimer = 0.;
			ChangeState(BossState::Disappear);
		}
	} break;
	case BossState::Disappear:
	{
		if (sprite->IsDone())
		{
			ChangeState(BossState::Glasses);
		}
	} break;
	case BossState::FakeDie:
	{
		if (sprite->IsDone())
		{
			alive = false;
		}
	} break;
	case BossState::Die:
	{
		velocity = { 0, 0 };
		if (!darkCapeSound && sprite->GetCurrentSprite()->GetCurrentFrame() == 4)
		{
			darkCapeSound = true;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_darkcape_01");
		}

		if (sprite->IsDone())
		{
			alive = false;
		}
	} break;
	}

	if (moveSwitch)
	{
		moveTimer += dt;
		if (moveTimer >= 0.25)
		{
			velocity = { 0, 0 };
			transform->SetPosition(goal);
			moveTimer = 0.;
			moveSwitch = false;
		}
		else
		{
			velocity = dir * 4.f;
		}
	}
	if (hit)
	{
		velocity *= 0.96;
	}
	if (!alive)
	{
		velocity = { 0, 0 };
	}
	transform->Translate(velocity * (float)dt);

	auto attacks = owner->GetObjectList(ObjectType::BossFire);
	for (auto a : attacks)
	{
		auto attack = dynamic_cast<BossFire*>(a.get());
		if (!attack->hit && attack->reflected && !hit && GetComponent<AABBCollisionComponent>()->CheckCollide(attack))
		{
			hit = true;
			moveSwitch = false;
			velocity = attack->GetVelocity();
			attack->Crash();
			Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
			int remain = 0;
			for (int i = 0; i < bossScene->fakeBosses.size(); i++)
			{
				if (!bossScene->fakeBosses[i]->hit) remain++;
			}
			if (remain == 0)
			{
				ChangeState(BossState::Die);
				bossScene->ChangeBossSceneState(BossScene::BossSceneState::Phase3Intro);
			}
			else
			{
				ChangeState(BossState::FakeDie);
			}
		}
	}


	afterimageTimer += dt;
	if (afterimageTimer > 0.05 && alive)
	{
		afterimageTimer = 0.;
		Afterimage afterImage{};
		afterImage.modelMat = transform->GetModelMatrix();
		afterImage.texture = sprite->GetCurrentSprite()->GetCurrentData();
		afterImage.lifetime = 0.3;
		afterImage.info.color = glm::vec4(0.36, 0.0, 0.40, 0.8);
		afterimages.push_back(afterImage);
	}

	for (auto& afterimage : afterimages)
	{
		afterimage.lifetime -= dt;
		afterimage.info.color.a -= dt * 2;
	}

	afterimages.remove_if([](const auto& img) { return img.lifetime <= 0.0f; });
}

void BossDoppleGanger::Render()
{
	for (auto& afterimage : afterimages)
	{
		afterimage.Render(Engine::GetInstance().GetShaderProgram(), Engine::GetInstance().playerCam);
	}
	if(alive) sprite->Render();

	AABB->Render();
}

void BossDoppleGanger::ChangeState(BossState newState)
{
	switch (newState)
	{
	case BossState::Glasses:
	{
		sprite->PlaySprite("boss_glasses");
	} break;
	case BossState::Float:
	{
		sprite->PlaySprite("boss_float_loop");
	} break;
	case BossState::Appear:
	{
		sprite->PlaySprite("boss_appear");
	} break;
	case BossState::Charge:
	{
		sprite->PlaySprite("boss_charge");
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_blast_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
	} break;
	case BossState::Fire:
	{
		sprite->PlaySprite("boss_fire");
	} break;
	case BossState::Disappear:
	{
		sprite->PlaySprite("boss_disappear");
	} break;
	case BossState::FakeDie:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_glitchdeath_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
		sprite->PlaySprite("boss_doppleganger_die");
	} break;
	case BossState::Die:
	{
		sprite->PlaySprite("boss_die");
	} break;
	}
	currState = newState;
}

void BossDoppleGanger::SetGoal(glm::vec2 pos)
{
	moveSwitch = true;
	moveTimer = 0.;
	goal = pos;
	dir = pos - transform->GetPosition();
}
