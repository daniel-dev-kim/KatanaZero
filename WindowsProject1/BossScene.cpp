#include "pch.h"
#include "BossScene.h"
#include "ObjectManager.h"

#include "Engine.h"
#include "InputSystem.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "AudioManager.h"
#include "Random.h"

//Objects
#include "Background.h"
#include "Player.h"
#include "Terrain.h"
#include "Cursor.h"
#include "Item.h"
#include "UI.h"
#include "Transition.h"
#include "Boss.h"
#include "BossDoppleGanger.h"
#include "BossEye.h"
#include "Filter.h"
#include "Mutation.h"
#include "Head.h"
#include "Brain.h"
#include "Stab.h"
#include "Injection.h"
#include "Pause.h"

//Components
#include "TextureComponent.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "LineCollisionComponent.h"
#include "Light.h"

void BossScene::ChangeTerrain()
{
	objectManager->ClearObjectList(ObjectType::Background);
	objectManager->ClearObjectList(ObjectType::Terrain);
	// 지형
	{
		auto t1 = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		t1->AddLine({ {0, 100}, {1280, 100} });
		t1->SetWallThroughable(false);
		objectManager->AddObject(ObjectType::Terrain, t1);
		floor = t1->GetComponent<LineCollisionComponent>();
		player->currentGround = floor;

		auto t2 = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto t2Transform = t2->AddComponent<TransformComponent>();
		t2Transform->SetPosition({ 0, 351 });
		t2->AddAABB({ 50, 600 }, { 0, 0 });
		t2->SetWallSidable(false);
		objectManager->AddObject(ObjectType::Terrain, t2);

		auto t4 = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto t4Transform = t4->AddComponent<TransformComponent>();
		t4Transform->SetPosition({ 1280, 351 });
		t4->AddAABB({ 50, 600 }, { 0, 0 });
		t4->SetWallSidable(false);
		objectManager->AddObject(ObjectType::Terrain, t4);
	}
}

void BossScene::Load()
{
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 7; ++col) {
			if ((row + col) % 2 == 0) {
				float x = 256 + col * 128;
				float y = 288 + row * 72;
				gridPositions.push_back({ x, y });
			}
		}
	}

	Scene::Load();
	// 플레이어
	{
		player = std::make_shared<Player>(objectManager.get(), ObjectType::Player);
		player->GetComponent<TransformComponent>()->SetPosition(200, 230);
		player->CanMove = false;
		objectManager->AddObject(ObjectType::Player, player);
	}
	
	{
		objectManager->pause = std::make_shared<Pause>(objectManager.get(), ObjectType::UI);
	}

	// 보스
	{
		boss = std::make_shared<Boss>(objectManager.get(), ObjectType::Boss, this);
		boss->GetComponent<TransformComponent>()->SetPosition(730, 235);
		objectManager->AddObject(ObjectType::Boss, boss);
	}

	//배경
	{
		auto bg = std::make_shared<Background>(objectManager.get(), ObjectType::Background);
		auto texture = bg->AddComponent<TextureComponent>("bg_boss", "Resource/Texture/Background/bg_boss/bg_psychiatrist_boss_0.png");
		auto transform = bg->AddComponent<TransformComponent>();
		transform->SetPosition(texture->GetSize() / 1.9f);
		Engine::GetInstance().playerCam->SetWorldBounds({ 0.f, 0.f }, texture->GetSize());

		objectManager->AddObject(ObjectType::Background, bg);

		{
			filter = std::make_shared<Filter>(objectManager.get(), ObjectType::EffectOverUI);
		}
	}

	{ //난로
		firePlace = std::make_shared<Light>(objectManager.get(), ObjectType::Background, glm::vec3{ 1.f, 0.27f , 0.f });
		auto sprite = firePlace->AddComponent<SpriteManagerComponent>("Resource/Texture/Background/bg_boss/fireplace/fireplace.txt");
		sprite->PlaySprite("fireplace_loop");
		auto transform = firePlace->GetComponent<TransformComponent>();
		transform->SetPosition(659, 248);
		transform->SetScale(2.05, 2.05);
		firePlace->info.color.a = 1.0;
		objectManager->AddObject(ObjectType::Background, firePlace);
	}

	//배경
	{
		bg2 = std::make_shared<Background>(objectManager.get(), ObjectType::Background);
		auto texture = bg2->AddComponent<TextureComponent>("bg_boss_front", "Resource/Texture/Background/bg_boss/bg_psychiatrist_0.png");
		auto transform = bg2->AddComponent<TransformComponent>();
		transform->SetPosition(texture->GetSize() / 1.9f);
		Engine::GetInstance().playerCam->SetWorldBounds({ 0.f, 0.f }, texture->GetSize());

		objectManager->AddObject(ObjectType::Background, bg2);
	}

	// 지형
	{
		auto t1 = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		t1->AddLine({ {125, 219}, {301, 219}, {323, 195}, {1290, 195} });
		t1->SetWallThroughable(false);
		objectManager->AddObject(ObjectType::Terrain, t1);
		floor = t1->GetComponent<LineCollisionComponent>();
		player->currentGround = floor;

		auto t2 = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto t2Transform = t2->AddComponent<TransformComponent>();
		t2Transform->SetPosition({ 140, 351 });
		t2->AddAABB({ 50, 600 }, { 0, 0 });
		t2->SetWallSidable(false);
		objectManager->AddObject(ObjectType::Terrain, t2);

		auto t4 = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto t4Transform = t4->AddComponent<TransformComponent>();
		t4Transform->SetPosition({ 1150, 351 });
		t4->AddAABB({ 50, 600 }, { 0, 0 });
		t4->SetWallSidable(false);
		objectManager->AddObject(ObjectType::Terrain, t4);
	}

	//UI
	{
		//커서
		{
			auto cursor = std::make_shared<Cursor>(objectManager.get(), ObjectType::UI);
			objectManager->AddObject(ObjectType::UI, cursor);
		}

		{
			auto ui = std::make_shared<UI>(objectManager.get(), ObjectType::UI);
			objectManager->AddObject(ObjectType::UI, ui);
		}
	}
	Engine::GetInstance().GetAudioManager()->StopSound("song_youwillneverknow");
	Engine::GetInstance().GetAudioManager()->StartSound("sound_object_fireplace_loop_01");
	Engine::GetInstance().GetAudioManager()->StartSound("song_nocturne");

	zigzagTimer = 0.0;
	showGO = false;
	ChangeBossSceneState(BossSceneState::TransitionIn);
	objectManager->SetState(ObjectManager::OMState::Boss);


}

void BossScene::Update(double dt)
{
	objectManager->Update(dt);

	switch (currBossSceneState)
	{
	case BossSceneState::TransitionIn:
	{
		if (objectManager->GetObjectList(ObjectType::EffectOverUI).empty())
		{
			objectManager->ClearObjectList(ObjectType::EffectOverUI);
			player->CanMove = true;
		}
		if (glm::distance(player->GetComponent<TransformComponent>()->GetPosition(), boss->GetComponent<TransformComponent>()->GetPosition()) < 150)
		{
			savePos = player->GetComponent<TransformComponent>()->GetPosition();
			player->CanMove = false;
			player->ChangeState(&player->stateIdle);
			ChangeBossSceneState(BossSceneState::Phase1Intro);
		}
	}break;
	case BossSceneState::PlayPhase1:
	{

	} break;
	case BossSceneState::PlayPhase2:
	{
		for (auto& fakeBoss : fakeBosses)
		{
			fakeBoss->Update(dt);
		}

		switch (phase2Pattern)
		{
		case Phase2Pattern::EllipseSpread:
		{
			bool checkMoveDone = false;
			for (int i = 0; i < fakeBosses.size(); i++)
			{
				if (fakeBosses[i]->alive)
				{
					checkMoveDone |= fakeBosses[i]->moveSwitch;
				}
			}
			if (checkMoveDone == false)
			{
				ChangeBossPhase2Pattern(Phase2Pattern::EllipseSpin);
			}
		}break;
		case Phase2Pattern::EllipseSpin:
		{
			phase2Timer += dt * 1.25 * 9 / fakeBosses.size();
			for (int i = 0; i < fakeBosses.size(); i++)
			{
				if(!fakeBosses[i]->hit) fakeBosses[i]->GetComponent<TransformComponent>()->SetPosition(PointOnEllipse(i * 2 * PI / fakeBosses.size() + phase2Timer));
				if (glm::length(fakeBosses[i]->GetComponent<TransformComponent>()->GetPosition() - PointOnEllipse(7 * PI / 6)) < 2. && !fakeBosses[i]->didAttack && phase2Attack < 3)
				{
					phase2Attack++;
					fakeBosses[i]->didAttack = true;
					fakeBosses[i]->ChangeState(BossDoppleGanger::BossState::Appear);
				}
			}

			if (phase2Timer > PI * 11 / fakeBosses.size())
			{
				phase2Timer = 0.;
				phase2Attack = 0;
				int aliveBosses = 0;
				for (int i = 0; i < fakeBosses.size(); i++)
				{
					if (!fakeBosses[i]->hit) aliveBosses++;
				}
				if (aliveBosses > 5)
				{
					ChangeBossPhase2Pattern(Phase2Pattern::ColumnSpread);
				}
				else
				{
					ChangeBossPhase2Pattern(Phase2Pattern::RandomSpread);
				}
			}
		}break;
		case Phase2Pattern::ColumnSpread:
		{
			bool checkMoveDone = false;
			for (int i = 0; i < fakeBosses.size(); i++)
			{
				if (fakeBosses[i]->alive)
				{
					checkMoveDone |= fakeBosses[i]->moveSwitch;
				}
			}
			if (checkMoveDone == false)
			{
				ChangeBossPhase2Pattern(Phase2Pattern::ColumnAttack);
			}
		}break;
		case Phase2Pattern::ColumnAttack:
		{
			phase2Timer += dt;
			for (int i = 0; i < fakeBosses.size(); i++)
			{
				if (phase2Timer > columnAttackStartTimer[i] && !fakeBosses[i]->didAttack && !fakeBosses[i]->hit)
				{
					fakeBosses[i]->didAttack = true;
					fakeBosses[i]->ChangeState(BossDoppleGanger::BossState::Appear);
				}
			}
			bool checkAttackDone = true;
			for (int i = 0; i < fakeBosses.size(); i++)
			{
				if (!fakeBosses[i]->hit && !fakeBosses[i]->didAttack)
				{
					checkAttackDone = false;
				}
			}
			auto maxIter = std::max_element(columnAttackStartTimer.begin(), columnAttackStartTimer.end());
			if (checkAttackDone) stay += dt;

			if (stay > 0.5 + *maxIter)
			{
				stay = 0.;
				phase2Timer = 0.;
				phase2Attack = 0;
				columnAttackStarted = false;
				int aliveBosses = 0;
				for (int i = 0; i < fakeBosses.size(); i++)
				{
					if (!fakeBosses[i]->hit) aliveBosses++;
				}
				if (aliveBosses > 5)
				{
					ChangeBossPhase2Pattern(Phase2Pattern::EllipseSpread);
				}
				else
				{
					ChangeBossPhase2Pattern(Phase2Pattern::RandomSpread);
				}
			}

		}break;
		case Phase2Pattern::RandomSpread:
		{
			bool checkMoveDone = false;
			for (int i = 0; i < fakeBosses.size(); i++)
			{
				if (fakeBosses[i]->alive)
				{
					checkMoveDone |= fakeBosses[i]->moveSwitch;
				}
			}
			if (checkMoveDone == false)
			{
				ChangeBossPhase2Pattern(Phase2Pattern::RandomSpreadAttack);
			}
		}break;
		case Phase2Pattern::RandomSpreadAttack:
		{
			if (phase2Timer > 0.5)
			{
				stay += dt;
				for (int i = 0; i < fakeBosses.size(); i++)
				{
					if (!fakeBosses[i]->didAttack && !fakeBosses[i]->hit)
					{
						fakeBosses[i]->didAttack = true;
						fakeBosses[i]->ChangeState(BossDoppleGanger::BossState::Appear);
					}
				}
			}
			else
			{
				phase2Timer += dt;
			}
			if (stay > 1.5)
			{
				phase2Timer = 0.;
				stay = 0.;
				ChangeBossPhase2Pattern(Phase2Pattern::RandomSpread);
			}
		}break;
		}
	} break;
	case BossSceneState::PlayPhase3:
	{
		if (pitch > 0.8)
		{
			pitch -= dt * 0.1;
		}
		else
		{
			pitchTimer += dt;
			float raw = cosf(2.0f * 3.14159f * pitchTimer / 20.0f);  // -1 ~ 1
			pitch = 0.3f * raw + 0.49f;
		}



		Engine::GetInstance().GetAudioManager()->SetPlaySpeed("song_mindgames", pitch);
		for (auto eye : eyes)
		{
			eye->Update(dt);
		}

		switch (phase3Pattern)
		{
		case Phase3Pattern::TwoEyes:
		{
			bool allDie = true;
			for (auto eye : eyes)
			{
				if (!eye->IsDead())
				{
					allDie = false;
				}
			}
			if (allDie)
			{
				phase3Timer += dt;
			}
			if (phase3Timer > 0.5)
			{
				phase3Timer = 0.;
				eyes.clear();
				phase3Pattern = Phase3Pattern::Trail;
				Engine::GetInstance().uPersistentTrails = true;
				std::shuffle(gridPositions.begin(), gridPositions.end(), std::mt19937(std::random_device{}()));
				for(int i = 0; i < ColorfulEyeNum; i++)
				{
					float offsetX = Engine::GetInstance().GetRandom()->get(-40, 40);
					float offsetY = Engine::GetInstance().GetRandom()->get(-40, 40);
					auto eye = std::make_shared<BossEye>(objectManager.get(), ObjectType::Boss, this);
					eye->GetComponent<TransformComponent>()->SetPosition(gridPositions[i] + glm::vec2{offsetX, offsetY});
					eyes.push_back(eye);
				}
			}
		} break;
		case Phase3Pattern::Colorful:
		{
			bool allDie = true;
			for (auto eye : eyes)
			{
				if (!eye->IsDead())
				{
					allDie = false;
				}
			}
			if (allDie)
			{
				phase3Timer += dt;
			}
			if (phase3Timer > 0.5)
			{
				phase3Timer = 0.;
				eyes.clear();
				phase3Pattern = Phase3Pattern::Quad;
				Engine::GetInstance().uColorCycleFull = false;
				Engine::GetInstance().uColorCycleQuadrant = true;
				std::shuffle(gridPositions.begin(), gridPositions.end(), std::mt19937(std::random_device{}()));
				for (int i = 0; i < QuadEyeNum; i++)
				{
					float offsetX = Engine::GetInstance().GetRandom()->get(-40, 40);
					float offsetY = Engine::GetInstance().GetRandom()->get(-40, 40);
					auto eye = std::make_shared<BossEye>(objectManager.get(), ObjectType::Boss, this);
					eye->GetComponent<TransformComponent>()->SetPosition(gridPositions[i] + glm::vec2{ offsetX, offsetY });
					eyes.push_back(eye);
				}
			}
		} break;
		case Phase3Pattern::Quad:
		{           
			bool allDie = true;
			for (auto eye : eyes)
			{
				if (!eye->IsDead())
				{
					allDie = false;
				}
			}
			if (allDie)
			{
				phase3Timer += dt;
			}
			if (phase3Timer > 0.5)
			{
				phase3Timer = 0.;
				eyes.clear();
				phase3Pattern = Phase3Pattern::FourDirFlip;
				Engine::GetInstance().uColorCycleQuadrant = false;
				Engine::GetInstance().uFlipRenderWithColor = true;
				std::shuffle(gridPositions.begin(), gridPositions.end(), std::mt19937(std::random_device{}()));
				for (int i = 0; i < FourDirFlipEyeNum; i++)
				{
					float offsetX = Engine::GetInstance().GetRandom()->get(-40, 40);
					float offsetY = Engine::GetInstance().GetRandom()->get(-40, 40);
					auto eye = std::make_shared<BossEye>(objectManager.get(), ObjectType::Boss, this);
					eye->GetComponent<TransformComponent>()->SetPosition(gridPositions[i] + glm::vec2{ offsetX, offsetY });
					eyes.push_back(eye);
				}
			}
		} break;
		case Phase3Pattern::FourDirFlip:
		{
         

			bool allDie = true;
			for (auto eye : eyes)
			{
				if (!eye->IsDead())
				{
					allDie = false;
				}
			}
			if (allDie)
			{
				phase3Timer += dt;
			}
			if (phase3Timer > 0.5)
			{
				phase3Timer = 0.;
				eyes.clear();
				phase3Pattern = Phase3Pattern::FilpOnly;
				Engine::GetInstance().uFlipRenderWithColor = false;
				Engine::GetInstance().uFlipYOnly = true;
				std::shuffle(gridPositions.begin(), gridPositions.end(), std::mt19937(std::random_device{}()));
				for (int i = 0; i < FilpOnlyEyeNum; i++)
				{
					float offsetX = Engine::GetInstance().GetRandom()->get(-40, 40);
					float offsetY = Engine::GetInstance().GetRandom()->get(-40, 40);
					auto eye = std::make_shared<BossEye>(objectManager.get(), ObjectType::Boss, this);
					eye->GetComponent<TransformComponent>()->SetPosition(gridPositions[i] + glm::vec2{ offsetX, offsetY });
					eyes.push_back(eye);
				}
			}
		} break;
		case Phase3Pattern::FilpOnly:
		{                 

			bool allDie = true;
			for (auto eye : eyes)
			{
				if (!eye->IsDead())
				{
					allDie = false;
				}
			}
			if (allDie)
			{
				phase3Timer += dt;
			}
			if (phase3Timer > 0.5)
			{
				phase3Timer = 0.;
				eyes.clear();
				phase3Pattern = Phase3Pattern::Trail;
				Engine::GetInstance().uFlipYOnly = false;
				Engine::GetInstance().uPersistentTrails = true;
				std::shuffle(gridPositions.begin(), gridPositions.end(), std::mt19937(std::random_device{}()));
				for (int i = 0; i < TrailEyeNum; i++)
				{
					float offsetX = Engine::GetInstance().GetRandom()->get(-40, 40);
					float offsetY = Engine::GetInstance().GetRandom()->get(-40, 40);
					auto eye = std::make_shared<BossEye>(objectManager.get(), ObjectType::Boss, this);
					eye->GetComponent<TransformComponent>()->SetPosition(gridPositions[i] + glm::vec2{ offsetX, offsetY });
					eyes.push_back(eye);
				}
			}
		} break;
		case Phase3Pattern::Trail:
		{                
			bool allDie = true;
			for (auto eye : eyes)
			{
				if (!eye->IsDead())
				{
					allDie = false;
				}
			}
			if (allDie)
			{
				Engine::GetInstance().GetAudioManager()->StopSound("song_mindgames");
				Engine::GetInstance().uPersistentTrails = false;
				filter->info.color.a = 1.0;
				player->GetComponent<TransformComponent>()->SetPosition(savePos);
				player->GetComponent<TransformComponent>()->SetScale(2, 2);
				phase3Timer += dt;
			}
			if (phase3Timer > 1.5)
			{
				filter->info.color.a = 0.0;
				eyes.clear();
				ChangeBossSceneState(BossSceneState::Phase4Intro);
			}
		} break;
		}
	} break;
	case BossSceneState::PlayPhase4:
	{
		if (!clear && mutation->mutationState == Mutation::MutationState::Scream)
		{
			stab->attackCount = 0;
			clear = true;
			auto bossAttacks = objectManager->GetObjectList(ObjectType::BossArm);
			for (auto b : bossAttacks)
			{
				b->SetDead();
			}
			auto bossFires = objectManager->GetObjectList(ObjectType::BossFire);
			for (auto b : bossFires)
			{
				b->SetDead();
			}
		}
		if (mutation->mutationState != Mutation::MutationState::Hurt && mutation->mutationState != Mutation::MutationState::Scream)
		{
			headTimer += dt;
			brainTimer += dt;

			if (mutation->tentacleState != Mutation::TentacleState::Attack)
			{
				stabTimer += dt;
				InjectionTimer += dt;

				if (InjectionTimer > 6.)
				{
					if (Engine::GetInstance().GetRandom()->get(0, 1) == 0) {
						Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akiraportal_wheel_01");
						for (int i = 0; i < 11; i++)
						{
							double angle = i * PI / 10;
							glm::vec2 center = { 640, 130 };
							auto injection = std::make_shared<Injection>(objectManager.get(), ObjectType::BossArm, center + glm::vec2(cos(angle), sin(angle)) * 500.f, angle + PI);
							injection->timer = -i * 0.1;
							objectManager->AddObject(ObjectType::BossArm, injection);
						}
					}
					else
					{
						Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akiraportal_line_01");
						for (int i = 0; i < 9; i++)
						{
							glm::vec2 startPos = { 128 + 128 * i, 630 };
							auto injection = std::make_shared<Injection>(objectManager.get(), ObjectType::BossArm, startPos, -PI / 2);
							injection->timer = -i * 0.1;
							objectManager->AddObject(ObjectType::BossArm, injection);
						}
					}

					InjectionTimer = Engine::GetInstance().GetRandom()->get(-4., 2.);
				}
			}

			if (headTimer > 15.)
			{
				auto head = std::make_shared<Head>(objectManager.get(), ObjectType::BossArm, mutation.get(), glm::vec2(Engine::GetInstance().GetRandom()->get(280, 1000), 120));
				objectManager->AddObject(ObjectType::BossArm, head);
				headTimer = Engine::GetInstance().GetRandom()->get(-4., 2.);
			}

			if (brainTimer > 6.)
			{
				int count = Engine::GetInstance().GetRandom()->get(2, 3);
				for (int i = 0; i < count; i++)
				{
					auto brain = std::make_shared<Brain>(objectManager.get(), ObjectType::BossArm, glm::vec2(Engine::GetInstance().GetRandom()->get(280, 1000), 120), player.get());
					objectManager->AddObject(ObjectType::BossArm, brain);
				}
				brainTimer = Engine::GetInstance().GetRandom()->get(-4., 2.);
			}

			if (stabTimer > 6.)
			{
				mutation->TentacleAttack();
				stabTimer = Engine::GetInstance().GetRandom()->get(-4., 2.);
			}
		}
	} break;
	case BossSceneState::Phase1Intro:
	{
		if (bg2->alpha > 0.)
		{
			bg2->alpha -= dt * 0.5;
			firePlace->info.color.a -= dt * 0.5;
			Engine::GetInstance().GetAudioManager()->SetPlaySpeed("sound_object_fireplace_loop_01", bg2->alpha);
			Engine::GetInstance().GetAudioManager()->SetPlaySpeed("song_nocturne", bg2->alpha);
		}
		else
		{
			bg2->SetDead();
			Engine::GetInstance().GetAudioManager()->StopSound("sound_object_fireplace_loop_01");
			Engine::GetInstance().GetAudioManager()->StopSound("song_nocturne");
		}

		if (filter->info.color.a < 0.1)
		{
			filter->info.color.a += dt * 0.05;
		}


	} break;
	case BossSceneState::Phase3Intro:
	{
		for (auto fakeBoss : fakeBosses)
		{
			fakeBoss->SetVelocity(fakeBosses[0]->GetVelocity() * 0.96f);
			fakeBoss->Update(dt);
		}
		for (int i = 0; i < fakeBosses.size(); ) {
			if (!fakeBosses[i]->alive) {
				fakeBosses.erase(fakeBosses.begin() + i);  // 자동으로 인덱스 이동됨
			}
			else {
				++i;
			}
		}
		if (fakeBosses.empty())
		{
			dynamic_cast<Background*>(objectManager->GetFrontObject(ObjectType::Background).get())->Off();
			fakeBosses.clear();
			phase = 3;
			phase3Timer += dt;
		}
		if (phase3Timer > 1.5)
		{
			phase3Timer = 0.f;
			ChangeBossSceneState(static_cast<BossSceneState>(phase));
		}
	} break;
	case BossSceneState::ZigZag:
	{
		Engine::GetInstance().zigzagStrength = 1.0f;
		Engine::GetInstance().zigzagOffset = zigzagTimer * 5.0;
		zigzagTimer += dt;
		if (zigzagTimer > 0.2)
		{
			Engine::GetInstance().zigzagStrength = 0.f;
			Engine::GetInstance().zigzagOffset = 0.f;
			ChangeBossSceneState(static_cast<BossSceneState>(phase));
		}
	}
	}
	player->memoryTimer = 60;
	if (currBossSceneState != BossSceneState::ZigZag && player->die) ChangeSceneState(SceneState::Die);
	switch (currState)
	{
	case SceneState::Die:
	{
		if (Engine::GetInstance().GetInputSystem()->IsKeyPressed(InputKey::LButton))
		{
			retry = true;
			ChangeSceneState(SceneState::Play);
			ChangeBossSceneState(BossSceneState::ZigZag);
		}
	} break;
	}

}

void BossScene::ChangeSceneState(SceneState newState) 
{
	currState = newState;
}


void BossScene::ChangeBossSceneState(BossSceneState newState)
{
	switch (newState)
	{
	case BossSceneState::TransitionIn:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("transition_end");
		Engine::GetInstance().mono = false;
		objectManager->SetState(ObjectManager::OMState::Idle);
		for (int i = 0; i < 20; i++)
		{
			auto fadeIn = std::make_shared<Transition>(objectManager.get(), ObjectType::EffectOverUI, true);
			fadeIn->GetComponent<TransformComponent>()->SetPosition(32 + 64 * (19 - i), Engine::GetInstance().GetWindowSize().bottom / 2.);
			fadeIn->SetStartTimer(i * 1 / 120.);
			RenderInfo info{};
			info.isFixed = true;
			fadeIn->SetRenderInfo(info);
			fadeIn->SetIsDeadBySpriteEnd(true);
			objectManager->AddObject(ObjectType::EffectOverUI, fadeIn);
		}
	}break;
	case BossSceneState::Phase1Intro:
	{
		retry = false;
		firePlace->GetComponent<SpriteManagerComponent>()->PlaySprite("fireplace_die");
		boss->ChangeState(Boss::BossState::Glasses);
	} break;
	case BossSceneState::Phase3Intro:
	{
		retry = false;
	} break;
	case BossSceneState::Phase4Intro:
	{
		phase = 4;
		retry = false;
		dynamic_cast<Background*>(objectManager->GetFrontObject(ObjectType::Background).get())->On();
		mutation = std::make_shared<Mutation>(objectManager.get(), ObjectType::Boss, this);
		mutation->GetComponent<TransformComponent>()->SetPosition(740, 224);
		objectManager->AddObject(ObjectType::Boss, mutation);
		stab = std::make_shared<Stab>(objectManager.get(), ObjectType::Neutral3, mutation.get(), player.get());
		objectManager->AddObject(ObjectType::Neutral3, stab);
	} break;
	case BossSceneState::PlayPhase1:
	{
		if (retry)
		{
			player->hit = false;
			player->willDie = false;
			player->die = false;
			player->GetComponent<TransformComponent>()->SetPosition(640, 206);
			player->GetComponent<TransformComponent>()->SetScale(2, 2);
			player->ChangeState(&player->stateIdle);

			objectManager->GetFrontObject(ObjectType::Boss)->SetDead();
			auto arms = objectManager->GetObjectList(ObjectType::BossArm);
			for (auto arm : arms)
			{
				arm->SetDead();
			}
			// 보스
			{
				boss = std::make_shared<Boss>(objectManager.get(), ObjectType::Boss, this);
				boss->GetComponent<TransformComponent>()->SetPosition(660, 500);
				objectManager->AddObject(ObjectType::Boss, boss);
			}
		}
		player->CanMove = true;
		boss->hp = bossHP;
		currState == SceneState::Play;
		boss->ChangeState(Boss::BossState::Fight);
		boss->sprite->PlaySprite("boss_float_loop");
	} break;
	case BossSceneState::PlayPhase2:
	{

		if (retry)
		{
			player->hit = false;
			player->willDie = false;
			player->die = false;
			player->GetComponent<TransformComponent>()->SetPosition(640, 206);
			player->GetComponent<TransformComponent>()->SetScale(2, 2);
			player->ChangeState(&player->stateIdle);
			auto fires = objectManager->GetObjectList(ObjectType::BossFire);
			for (auto f : fires)
			{
				f->SetDead();
			}
			auto blood1 = objectManager->GetObjectList(ObjectType::BackgroundEffect);
			for (auto b : blood1)
			{
				b->SetDead();
			}
			auto blood2 = objectManager->GetObjectList(ObjectType::Blood2);
			for (auto b : blood2)
			{
				b->SetDead();
			}
			auto blood3 = objectManager->GetObjectList(ObjectType::Blood3);
			for (auto b : blood3)
			{
				b->SetDead();
			}
			fakeBosses.clear();
			phase2Timer = 0.f;
			phase2Attack = 0;
			columnAttackStarted = false;
			stay = 0.0;
		}
		player->CanMove = true;
		fakeBosses.resize(DopplegangerNum);
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_split_01");
		for (int i = 0; i < DopplegangerNum; i++)
		{
			auto fakeBoss = std::make_shared<BossDoppleGanger>(objectManager.get(), ObjectType::Boss, this);
			fakeBoss->GetComponent<TransformComponent>()->SetPosition(660, 550);
			fakeBosses[i] = fakeBoss;
		}
		ChangeBossPhase2Pattern(Phase2Pattern::EllipseSpread);
	} break;
	case BossSceneState::PlayPhase3:
	{
		if (retry)
		{
			phase3Pattern = Phase3Pattern::TwoEyes;
			eyes.clear();
			player->hit = false;
			player->willDie = false;
			player->die = false;
			player->GetComponent<TransformComponent>()->SetPosition(640, 206);
			player->GetComponent<TransformComponent>()->SetScale(2, 2);
			player->ChangeState(&player->stateIdle);
			auto blood1 = objectManager->GetObjectList(ObjectType::BackgroundEffect);
			for (auto b : blood1)
			{
				b->SetDead();
			}
			auto blood2 = objectManager->GetObjectList(ObjectType::Blood2);
			for (auto b : blood2)
			{
				b->SetDead();
			}
			auto blood3 = objectManager->GetObjectList(ObjectType::Blood3);
			for (auto b : blood3)
			{
				b->SetDead();
			}
			auto effects = objectManager->GetObjectList(ObjectType::Effect);
			for (auto e : effects)
			{
				e->SetDead();
			}
		}
		filter->info.color.a = 0.0;
		player->CanMove = true;
		{
			auto eye = std::make_shared<BossEye>(objectManager.get(), ObjectType::Boss, this);
			eye->GetComponent<TransformComponent>()->SetPosition(540, 400);
			eyes.push_back(eye);
		}
		{
			auto eye = std::make_shared<BossEye>(objectManager.get(), ObjectType::Boss, this);
			eye->GetComponent<TransformComponent>()->SetPosition(740, 400);
			eyes.push_back(eye);
		}

	} break;
	case BossSceneState::PlayPhase4:
	{
		if (retry)
		{
			phase3Pattern = Phase3Pattern::TwoEyes;
			eyes.clear();
			player->hit = false;
			player->willDie = false;
			player->die = false;
			player->GetComponent<TransformComponent>()->SetPosition(640, 206);
			player->GetComponent<TransformComponent>()->SetScale(2, 2);
			player->ChangeState(&player->stateIdle);
			auto blood1 = objectManager->GetObjectList(ObjectType::BackgroundEffect);
			for (auto b : blood1)
			{
				b->SetDead();
			}
			auto blood2 = objectManager->GetObjectList(ObjectType::Blood2);
			for (auto b : blood2)
			{
				b->SetDead();
			}
			auto blood3 = objectManager->GetObjectList(ObjectType::Blood3);
			for (auto b : blood3)
			{
				b->SetDead();
			}
			auto effects = objectManager->GetObjectList(ObjectType::Effect);
			for (auto e : effects)
			{
				e->SetDead();
			}
			auto bossAttacks = objectManager->GetObjectList(ObjectType::BossArm);
			for (auto b : bossAttacks)
			{
				b->SetDead();
			}
			auto bossFires = objectManager->GetObjectList(ObjectType::BossFire);
			for (auto b : bossFires)
			{
				b->SetDead();
			}
			mutation->sprite->PlaySprite("mutation_idle");
			mutation->tentacle->PlaySprite("tentacle_idle");
			stab->currentState = Stab::StabState::Hide;
		}
		mutation->mutationState = Mutation::MutationState::Idle;
		mutation->tentacleState = Mutation::TentacleState::Idle;
		player->CanMove = true;
		headTimer = 3.;
		brainTimer = 2.;
		stabTimer = 5.;
		InjectionTimer = 7.;
		mutation->hp = MutationHP;


	} break;
	case BossSceneState::ZigZag:
	{
		Engine::GetInstance().uColorCycleFull = false;
		Engine::GetInstance().uColorCycleQuadrant = false;
		Engine::GetInstance().uFlipRenderWithColor = false;
		Engine::GetInstance().uFlipYOnly = false;
		Engine::GetInstance().uPersistentTrails = false;
		Engine::GetInstance().uGlitchEnabled = false;
		Engine::GetInstance().uTime = 0.0;
		Engine::GetInstance().GetAudioManager()->StartSound("degauss");
		zigzagTimer = 0.;
	} break;
	}
	currBossSceneState = newState;
}

void BossScene::ChangeBossPhase2Pattern(Phase2Pattern newPattern)
{
	switch (newPattern)
	{
	case Phase2Pattern::EllipseSpread:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_reposition_01");
		for (int i = 0; i < fakeBosses.size(); ) {
			if (!fakeBosses[i]->alive) {
				fakeBosses.erase(fakeBosses.begin() + i);  // 자동으로 인덱스 이동됨
			}
			else {
				++i;
			}
		}
		for (int i = 0; i < fakeBosses.size(); i++)
		{
			if (fakeBosses[i]->alive)
			{
				fakeBosses[i]->didAttack = false;
				fakeBosses[i]->SetGoal(PointOnEllipse(i * 2 * PI / fakeBosses.size()));
			}
			else
			{
				fakeBosses[i]->SetDead();
			}
		}
		
	} break;
	case Phase2Pattern::EllipseSpin:
	{

	} break;
	case Phase2Pattern::ColumnSpread:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_reposition_01");

		for (int i = 0; i < fakeBosses.size(); ) {
			if (!fakeBosses[i]->alive) {
				fakeBosses.erase(fakeBosses.begin() + i);  // 자동으로 인덱스 이동됨
			}
			else {
				++i;
			}
		}

		float startX = 640.0f - 500.0f;
		float gap = 1000.0f / (fakeBosses.size() - 1);  // n개의 점이 n-1 간격으로 나뉨

		for (int i = 0; i < fakeBosses.size(); i++)
		{
			if (fakeBosses[i]->alive)
			{
				float x = startX + i * gap;
				fakeBosses[i]->didAttack = false;
				fakeBosses[i]->SetGoal({ x, 550 });
			}
		}
	} break;
	case Phase2Pattern::ColumnAttack:
	{
		columnAttackStartTimer.clear();
		columnAttackStartTimer.resize(fakeBosses.size());
		for (auto& t : columnAttackStartTimer)
		{
			t = Engine::GetInstance().GetRandom()->get(0.0, 1.0);
		}
	} break;
	case Phase2Pattern::RandomSpread:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_reposition_01");

		for (int i = 0; i < fakeBosses.size(); ) {
			if (!fakeBosses[i]->alive) {
				fakeBosses.erase(fakeBosses.begin() + i);  // 자동으로 인덱스 이동됨
			}
			else {
				++i;
			}
		}

		std::shuffle(gridPositions.begin(), gridPositions.end(), std::mt19937(std::random_device{}()));

		for (int i = 0; i < fakeBosses.size(); i++)
		{
			if (fakeBosses[i]->alive)
			{
				glm::vec2 basePos = gridPositions[i];
				float offsetX = Engine::GetInstance().GetRandom()->get(-40, 40);
				float offsetY = Engine::GetInstance().GetRandom()->get(-40, 40);
				fakeBosses[i]->didAttack = false;
				fakeBosses[i]->SetGoal({ basePos.x + offsetX, basePos.y + offsetY });
			}
		}
	} break;
	case Phase2Pattern::RandomSpreadAttack:
	{

	} break;
	}
	phase2Pattern = newPattern;
}

void BossScene::Render()
{
	objectManager->Render();
	for (auto fakeBoss : fakeBosses)
	{
		fakeBoss->Render();
	}
	for (auto eye : eyes)
	{
		eye->Render();
	}
	filter->Render();
}


void BossScene::Unload()
{
}

