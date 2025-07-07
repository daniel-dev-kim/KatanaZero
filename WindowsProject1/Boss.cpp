#include "pch.h"
#include "Boss.h"
#include "ObjectManager.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "AABBCollisionComponent.h"
#include "TextureManager.h"

#include "Engine.h"
#include "Random.h"
#include "AudioManager.h"
#include "BossScene.h"

#include "BossSpark.h"
#include "BossAura.h"
#include "BossThunder.h"
#include "BossIntro.h"
#include "BossArm.h"
Boss::Boss(ObjectManager* owner, ObjectType type, BossScene* bossScene) : Object(owner, type), bossScene(bossScene)
{
	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_boss/obj_boss.txt");

	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({30, 80});
	ChangeState(BossState::Idle);
}

void Boss::Update(double dt)
{
	Object::Update(dt);
	transform->Translate(velocity * (float)dt);
	switch (currState)
	{
	case BossState::Idle:
	{

	} break;
	case BossState::Glasses:
	{
		if (sprite->IsDone())
		{
			ChangeState(BossState::Float_Begin);
		}
	} break;
	case BossState::Float_Begin:
	{
		velocity.y += 35. * dt;
		floatBeginTimer += dt;
		if (floatBeginTimer > 1.0)
		{
			ChangeState(BossState::Float_Up_Slow);
		}
	} break;
	case BossState::Float_Up_Slow:
	{
		velocity.y = 25.;
		floatBeginTimer += dt;
		if (!secondThunder && floatBeginTimer > 2.5)
		{
			secondThunder = true;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_flash_01");
			owner->AddObject(ObjectType::EffectOverUI, std::make_shared<BossThunder>(owner, ObjectType::EffectOverUI));
		}
		if (floatBeginTimer > 4.0)
		{
			floatStart = transform->GetPosition();
			ChangeState(BossState::Float_Up_Fast);
		}
	} break;
	case BossState::Float_Up_Fast:
	{
		floatTimer += dt * 0.5;
		transform->SetPosition(HermiteCurve(floatStart, floatEnd, glm::vec2(-200, 100), glm::vec2(50, 300), std::clamp(1 - (1 - floatTimer) * (1 - floatTimer), 0.f, 1.f)));
		afterimageTimer += dt;
		if (floatTimer > 1.0)
		{
			ChangeState(BossState::Float_End);
		}
	} break;
	case BossState::Float_End:
	{
		velocity.y = 0.;
		afterimageTimer += dt;
		radiusX += dt * 4;
		radiusY += dt * 2;
		radiusX = std::clamp(radiusX, 3.f, 7.f);
		radiusY = std::clamp(radiusY, 0.f, 2.f);
		glm::vec2 gap = { radiusX * sin(floatTimer * 5.f), radiusY * cos(floatTimer * 5.f)};
		floatTimer += dt;

		transform->SetPosition(floatEnd + gap);
		if (radiusY == 2.f)
		{
			ChangeState(BossState::ReadyToFight);
		}
	} break;
	case BossState::ReadyToFight:
	{
		afterimageTimer += dt;

		glm::vec2 gap = { radiusX * sin(floatTimer * 5.f), radiusY * cos(floatTimer * 5.f) };
		floatTimer += dt;
		transform->SetPosition(floatEnd + gap);

		if (floatTimer > 3.0f)
		{
			ChangeState(BossState::ShowCaseHands);
		}
	} break;
	case BossState::ShowCaseHands:
	{
		afterimageTimer += dt;
		glm::vec2 gap = { radiusX * sin(floatTimer * 5.f), radiusY * cos(floatTimer * 5.f) };
		transform->SetPosition(floatEnd + gap);
		if (owner->GetObjectList(ObjectType::BossArm).empty())
		{
			floatTimer += dt;
		}
		if (floatTimer > 4.0f)
		{
			if (bossScene->currBossSceneState != BossScene::BossSceneState::ZigZag)
			{
				bossScene->ChangeBossSceneState(BossScene::BossSceneState::ZigZag);
			}
		}
	} break;
	case BossState::Fight:
	{
		afterimageTimer += dt;

		glm::vec2 gap = { radiusX * sin(floatTimer * 5.f), radiusY * cos(floatTimer * 5.f) };
		floatTimer += dt;
		transform->SetPosition(floatEnd + gap);
		if (owner->GetObjectList(ObjectType::BossArm).empty())
		{
			if (hp > 0)
			{
				bool type = Engine::GetInstance().GetRandom()->getBool();
				switch (randomPattern)
				{
				case 0:
				{
					auto arm1 = std::make_shared<BossArm>(owner, ObjectType::BossArm, (type == false) ? 0 : 1, this);
					arm1->timer = Engine::GetInstance().GetRandom()->get(-0.5, 0.0);
					auto arm2 = std::make_shared<BossArm>(owner, ObjectType::BossArm, (type == false) ? 4 : 5, this);
					owner->AddObject(ObjectType::BossArm, arm1);
					owner->AddObject(ObjectType::BossArm, arm2);
				} break;
				case 1:
				{

					auto arm1 = std::make_shared<BossArm>(owner, ObjectType::BossArm, (type == false) ? 2 : 3, this);
					arm1->timer = Engine::GetInstance().GetRandom()->get(-0.5, 0.0); 
					auto arm2 = std::make_shared<BossArm>(owner, ObjectType::BossArm, (type == false) ? 8 : 9, this);
					owner->AddObject(ObjectType::BossArm, arm1);
					owner->AddObject(ObjectType::BossArm, arm2);
				} break;
				case 2:
				{
					auto arm1 = std::make_shared<BossArm>(owner, ObjectType::BossArm, (type == false) ? 6 : 7, this);
					arm1->timer = Engine::GetInstance().GetRandom()->get(-0.5, 0.0); 
					auto arm2 = std::make_shared<BossArm>(owner, ObjectType::BossArm, (type == false) ? 10 : 11, this);
					owner->AddObject(ObjectType::BossArm, arm1);
					owner->AddObject(ObjectType::BossArm, arm2);
				} break;
				}
				randomPattern++;
				if(Engine::GetInstance().GetRandom()->get(0, 3) == 0) randomPattern++;
				randomPattern %= 3;
			}
			else
			{
				ChangeState(BossState::Disappear);
			}
		}

	} break;
	case BossState::KillPlayer:
	{
		floatTimer += dt * 0.5;
		auto dirX = transform->GetScale().x;
		if(glm::distance(transform->GetPosition(), killPlayerPos) > 10) transform->SetPosition(HermiteCurve(floatEnd, killPlayerPos, glm::vec2((dirX > 0 ? -200 : 200), -100), glm::vec2((dirX > 0 ? 400 : -400), 0), std::clamp(1 - (1 - floatTimer) * (1 - floatTimer), 0.f, 1.f)));
		afterimageTimer += dt;

		if (!killPlayerSwitch && glm::distance(transform->GetPosition(), killPlayerPos) < 50)
		{
			killPlayerSwitch = true;
			grabArm->currState = BossArm::ArmState::HandExplode;
			sprite->PlaySprite("boss_explode_head");
		}

		if (killPlayerSwitch && sprite->IsDone())
		{
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_headburst_01");
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_headsplatter_01");
			grabArm->hand->PlaySprite("arm_player_die");
			ChangeState(BossState::Count);
		}

	} break;
	case BossState::ArmAttacked:
	{
		afterimageTimer += dt;
		if (sprite->IsDone())
		{
			ChangeState(BossState::Fight);
		}
	} break;
	case BossState::Disappear:
	{
		if (sprite->IsDone())
		{
			bossScene->retry = false;
			bossScene->ChangeBossSceneState(BossScene::BossSceneState::PlayPhase2);
			bossScene->phase = 2;
			SetDead();
		}
	} break;
	}

	if(afterimageTimer > 0.05)
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

void Boss::Render()
{
	for (auto& afterimage : afterimages)
	{
		afterimage.Render(Engine::GetInstance().GetShaderProgram(), Engine::GetInstance().playerCam);
	}

	RenderInfo info{};
	if(currState == BossState::ArmAttacked) info.color = { 1.0, 0.0, 0.0, 1.0 };
	sprite->Render(info);

	AABB->Render();
}

void Boss::ChangeState(BossState newState)
{
	switch (newState)
	{
	case BossState::Idle:
	{
		sprite->PlaySprite("spr_psych_prebattle_idle");
	} break;	
	case BossState::Glasses:
	{
		sprite->PlaySprite("spr_psych_prebattle_glasses");
	} break;
	case BossState::Float_Begin:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_powerup_01");
		floatBeginTimer = 0.;
		sprite->PlaySprite("boss_float_begin");
	} break;
	case BossState::Float_Up_Slow:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("song_mindgames");
		aura = std::make_shared<BossAura>(owner, ObjectType::BackgroundEffect, this);
		owner->AddObject(ObjectType::BackgroundEffect, aura);

		spark = std::make_shared<BossSpark>(owner, ObjectType::Effect, this);
		owner->AddObject(ObjectType::Effect, spark);
		sprite->PlaySprite("boss_float_loop");
	} break;
	case BossState::Float_Up_Fast:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_flash_03");
		owner->AddObject(ObjectType::EffectOverUI, std::make_shared<BossThunder>(owner, ObjectType::EffectOverUI));
		aura->SetDead();
		spark->SetDead();
	} break;
	case BossState::Float_End:
	{
	} break;
	case BossState::ReadyToFight:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_flash_03");
		owner->AddObject(ObjectType::EffectOverUI, std::make_shared<BossThunder>(owner, ObjectType::EffectOverUI));
	} break;
	case BossState::ShowCaseHands:
	{
		bossScene->currState = Scene::SceneState::Play;
		owner->AddObject(ObjectType::BossArm, std::make_shared<BossIntro>(owner, ObjectType::BossArm, glm::vec2(390, 180)));
		auto right = std::make_shared<BossIntro>(owner, ObjectType::BossArm, glm::vec2(890, 180));
		right->timer = -0.2;
		owner->AddObject(ObjectType::BossArm, right);
	} break;
	case BossState::Fight:
	{
		floatTimer = 0.;
		transform->SetPosition(floatEnd + glm::vec2{ 0., radiusY });

	} break;
	case BossState::KillPlayer:
	{
		floatTimer = 0.;
	} break;
	case BossState::ArmAttacked:
	{
		sprite->PlaySprite("boss_hurt_arm");
	} break;
	case BossState::Disappear:
	{
		sprite->PlaySprite("boss_disappear");
	} break;
	}
	currState = newState;
}
