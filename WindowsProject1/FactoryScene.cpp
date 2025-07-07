#include "pch.h"
#include "FactoryScene.h"
#include "ObjectManager.h"

#include "Engine.h"
#include "InputSystem.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "SceneManager.h"
//Objects
#include "Background.h"
#include "Player.h"
#include "Terrain.h"
#include "Cursor.h"
#include "Item.h"
#include "UI.h"
#include "Fanblade.h"
#include "Grunt.h"
#include "LaserBody.h"
#include "LaserBeam.h"
#include "Transition.h"
#include "GO.h"
#include "Pause.h"
//Components
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"
#include "Light.h"
#include "SpriteManagerComponent.h"
#include "YouCanDoThis.h"
#include "ToStart.h"
#include "ToLeave.h"
void FactoryScene::Load()
{
	Scene::Load();
	
	// 플레이어
	{
		player = std::make_shared<Player>(objectManager.get(), ObjectType::Player);
		player->GetComponent<TransformComponent>()->SetPosition(200, 258);
		objectManager->AddObject(ObjectType::Player, player);
		player->ChangeState(&player->stateIdle);

	}
	{
		objectManager->pause = std::make_shared<Pause>(objectManager.get(), ObjectType::UI);
	}
	//배경
	{
		auto bg = std::make_shared<Background>(objectManager.get(), ObjectType::Background);
		auto texture = bg->AddComponent<TextureComponent>("bg_factory", "Resource/Texture/Background/bg_stage/stage1bg.png");
		auto transform = bg->AddComponent<TransformComponent>();
		sceneSize = texture->GetSize();
		transform->SetPosition(sceneSize / 2.f);
		Engine::GetInstance().playerCam->SetWorldBounds({ 0.f, 0.f }, sceneSize);
		objectManager->AddObject(ObjectType::Background, bg);
	}

	// 그런트 1
	{
		auto grunt = std::make_shared<Grunt>(objectManager.get(), ObjectType::Enemy);
		grunt->GetComponent<TransformComponent>()->SetPosition(730, 688);
		objectManager->AddObject(ObjectType::Enemy, grunt);
		grunt->initState = grunt->stateWalk;
		grunt->ChangeState(grunt->stateWalk);

	}
	
	{// 그런트 2
		auto grunt = std::make_shared<Grunt>(objectManager.get(), ObjectType::Enemy);
		grunt->GetComponent<TransformComponent>()->SetPosition(550, 688);
		grunt->GetComponent<TransformComponent>()->ScaleBy(-1, 1);
		objectManager->AddObject(ObjectType::Enemy, grunt);
		grunt->initState = grunt->stateWalk;
		grunt->ChangeState(grunt->stateWalk);
	}

	// 오브젝트
	{
		{ //환풍기
			auto fanBlade = std::make_shared<Fanblade>(objectManager.get(), ObjectType::Neutral3);
			auto transform = fanBlade->GetComponent<TransformComponent>();
			transform->SetPosition({ 1425, 1035 });
			objectManager->AddObject(ObjectType::Neutral3, fanBlade);
		}

		//레이저
		{
			auto laserBody = std::make_shared<LaserBody>(objectManager.get(), ObjectType::Laser);
			{
				auto transform = laserBody->GetComponent<TransformComponent>();
				transform->SetPosition({ 944, 1145 });
			}
			
			auto laserBeam = std::make_shared<LaserBeam>(objectManager.get(), ObjectType::Laser);
			{
				auto transform = laserBeam->GetComponent<TransformComponent>();
				transform->SetPosition({ 944, 898 });
				transform->SetScale(1, 0.79);
				auto AABB = laserBeam->GetComponent<AABBCollisionComponent>();
				AABB->SetSize({ 3., 500. });
			}
			objectManager->AddObject(ObjectType::Laser, laserBeam);
			objectManager->AddObject(ObjectType::Laser, laserBody);

		}
		{ //빛
			{
				auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, glm::vec3{1.f, 0.f , 0.f});
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 128, 288 });
				objectManager->AddObject(ObjectType::Light, light);
			}
			{
				auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, glm::vec3{ 1.f, 0.3f , .6f });
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 416, 352 });
				objectManager->AddObject(ObjectType::Light, light);
			}
			{
				auto light = std::make_shared	<Light>(objectManager.get(), ObjectType::Light, glm::vec3{ 0.f, 1.f , 0.f });
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 544, 768 });
				objectManager->AddObject(ObjectType::Light, light);
			}
			{
				auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, glm::vec3{ 0.f, 0.f , 1.f });
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 736, 704 });
				objectManager->AddObject(ObjectType::Light, light);
			}
			{
				auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, glm::vec3{ 0.f, 0.f , 1.f });
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 980, 758 });
				objectManager->AddObject(ObjectType::Light, light);
			}
			{
				auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, glm::vec3{ 0.f, 1.f , 0.f });
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 906, 993 });
				objectManager->AddObject(ObjectType::Light, light);
			}
			{
				auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, glm::vec3{ 0.f, 0.f , 1.f });
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 1148, 1042 });
				objectManager->AddObject(ObjectType::Light, light);
			} 
			{
				auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, glm::vec3{ 0.f, 0.f , 1.f });
				auto transform = light->GetComponent<TransformComponent>();
				transform->SetPosition({ 1643, 1017 });
				objectManager->AddObject(ObjectType::Light, light);
			}
		}
	}


	// 지형
	{
		{ 
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 0, 320 }); // Center
			t->AddAABB({ 32, 192 }, { 0, 0 }); // Size
			t->SetWallSidable(false);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 첫 지붕
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 192, 648 }); // Center
			t->AddAABB({ 384, 464 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ //첫 바닥
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {0, 224}, {544, 224} });
			objectManager->AddObject(ObjectType::Terrain, t);
			auto player = dynamic_cast<Player*>(objectManager->GetFrontObject(ObjectType::Player).get());
			player->currentGround = t->GetComponent<LineCollisionComponent>();
			player->firstGround = t->GetComponent<LineCollisionComponent>();
		}

		{ //첫 왼쪽벽
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 400, 672 }); // Center
			t->AddAABB({ 32, 512 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ //두번쨰 지붕
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 608, 1096 }); // Center
			t->AddAABB({ 384, 464 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 첫 오른쪽벽
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 860, 399 }); // Center
			t->AddAABB({ 632, 480 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 내려뛰기 바닥
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->SetWallThroughable(true);
			t->AddLine({ {416, 640}, {543, 640} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 두번째 바닥
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {543, 640}, {1024, 640} });
			objectManager->AddObject(ObjectType::Terrain, t);	
		}

		{ // 튀어나온 곳
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 816, 1064 }); // Center
			t->AddAABB({ 32, 528 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 두번째 왼쪽 벽
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 848, 1008 }); // Center
			t->AddAABB({ 32, 416 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 세번째 지붕
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 1408, 1184 }); // Center
			t->AddAABB({ 1088, 64 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 두번째 오른쪽 벽
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 1540, 751 }); // Center
			t->AddAABB({ 1032, 352 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ // 세번쨰 바닥
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {1023, 928}, {2500, 928} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}
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

		{//GO
			auto go = std::make_shared<GO>(objectManager.get(), ObjectType::UI);
			auto transform = go->GetComponent<TransformComponent>();
			transform->SetPosition({1850, 1030});
			objectManager->AddObject(ObjectType::UI, go);
		}
	}
	bgmName = "song_youwillneverknow";
	zigzagTimer = 0.0;
	showGO = false;
	FirstZigzag = true;
	done = false;
	ChangeSceneState(SceneState::TransitionIn);
}

void FactoryScene::Update(double dt)
{

	switch (currState)
	{
	case SceneState::TransitionIn:
	{
		if (objectManager->GetBackObject(ObjectType::EffectOverUI)->GetComponent<SpriteManagerComponent>()->IsDone())
		{
			ChangeSceneState(SceneState::ChooseMusic);
			objectManager->ClearObjectList(ObjectType::EffectOverUI);
		}
	} break;
	case SceneState::ChooseMusic:
	{
		if (player->GetCurrState() == &player->stateIdle)
		{
			ChangeSceneState(SceneState::ClickToStart);
			player->currentGround = player->firstGround;
		}
	} break;
	case SceneState::ClickToStart:
	{
		clickToStartTimer += dt;
		if (clickToStartTimer > 1.0 && Engine::GetInstance().GetInputSystem()->IsKeyPressed(InputKey::LButton))
		{
			Engine::GetInstance().GetAudioManager()->StartSound("level_start", false);
			ChangeSceneState(SceneState::ZigZag);
		}
	} break;
	case SceneState::ZigZag:
	{
		Engine::GetInstance().zigzagStrength = 1.0f;
		Engine::GetInstance().zigzagOffset = zigzagTimer * 4.0;
		zigzagTimer += dt;
		if(zigzagTimer > (FirstZigzag ? 0.75 : 0.25)) ChangeSceneState(SceneState::Play);
	} break;
	case SceneState::Play:
	{
		auto monsters = objectManager->GetObjectList(ObjectType::Enemy);
		bool allDie = true;
		for (auto monster : monsters)
		{
			if (!dynamic_cast<Enemy*>(monster.get())->willDie)
			{
				allDie = false;
			}
		}

		if(allDie) showGO = true;
		if(showGO && player->GetComponent<TransformComponent>()->GetPosition().x > sceneSize.x) ChangeSceneState(SceneState::YouCanDoThis);
		if (player->die) ChangeSceneState(SceneState::Die);
	} break;
	case SceneState::Die:
	{
		if(Engine::GetInstance().GetInputSystem()->IsKeyPressed(InputKey::LButton)) ChangeSceneState(SceneState::Rewind);
	} break;
	case SceneState::Rewind:
	{
		if (objectManager->GetFrameIndex() == 0)
		{
			player->currentGround = player->firstGround;
			objectManager->SetState(ObjectManager::OMState::Idle);
			ChangeSceneState(SceneState::ZigZag);
		}
	} break;
	case SceneState::YouCanDoThis:
	{
		if (youCanDoThis->IsDead())
		{
			ChangeSceneState(SceneState::Replay);
			objectManager->ClearObjectList(ObjectType::EffectOverUI);
		}
	} break;
	case SceneState::Replay:
	{
		objectManager->GetFrontObject(ObjectType::UI)->Update(dt);
		if (objectManager->GetFrameIndex() == objectManager->GetMaxIndex() || Engine::GetInstance().GetInputSystem()->IsKeyPressed(InputKey::RButton))
		{
			ChangeSceneState(SceneState::TransitionOut);
		}
	} break;
	case SceneState::TransitionOut:
	{
		if (!done && objectManager->GetBackObject(ObjectType::EffectOverUI)->GetComponent<SpriteManagerComponent>()->IsDone())
		{
			done = true;
			Engine::GetInstance().GetSceneManager()->SetActiveScene("SmokeScene");
		}
	} break;
	case SceneState::Pause:
	{

	} break;
	}

	objectManager->Update(dt);

}

void FactoryScene::ChangeSceneState(SceneState newState)
{
	switch (newState)
	{
		case SceneState::TransitionIn:
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
		} break;
		case SceneState::ChooseMusic:
		{
			player->ChangeState(&player->statePlaySong);
		} break;
		case SceneState::ClickToStart:
		{
			objectManager->AddObject(ObjectType::EffectOverUI, std::make_shared<ToStart>(objectManager.get(), ObjectType::EffectOverUI));
		} break;
		case SceneState::ZigZag:
		{
			Engine::GetInstance().GetAudioManager()->StartSound("degauss");
			zigzagTimer = 0.;
		} break;
		case SceneState::Play:
		{
			FirstZigzag = false;
			Engine::GetInstance().zigzagStrength = 0.0f;
			Engine::GetInstance().zigzagOffset = 0.0f;
			objectManager->SetState(ObjectManager::OMState::Recording);
			player->CanMove = true;
		} break;
		case SceneState::Die:
		{
			objectManager->SetState(ObjectManager::OMState::SetUpRewinding);
			player->CanMove = false;
		} break;
		case SceneState::Rewind:
		{
			Engine::GetInstance().GetAudioManager()->StartSound("Rewind");
			objectManager->SetState(ObjectManager::OMState::Rewinding);
			player->ChangeState(&player->stateIdle);

		} break;
		case SceneState::YouCanDoThis:
		{
			player->CanMove = false;
			objectManager->ClearObjectList(ObjectType::UI);
			youCanDoThis = std::make_shared<YouCanDoThis>(objectManager.get(), ObjectType::EffectOverUI);
			objectManager->AddObject(ObjectType::EffectOverUI, youCanDoThis);
		} break;
		case SceneState::Replay:
		{
			objectManager->SetState(ObjectManager::OMState::Replaying);
			objectManager->AddObject(ObjectType::UI, std::make_shared<ToLeave>(objectManager.get(), ObjectType::UI));

		} break;
		case SceneState::TransitionOut:
		{
			objectManager->SetState(ObjectManager::OMState::Transition);
			Engine::GetInstance().GetAudioManager()->StartSound("transition_begin");

			for (int i = 0; i < 20; i++)
			{
				auto fadeOut = std::make_shared<Transition>(objectManager.get(), ObjectType::EffectOverUI, false);

				fadeOut->GetComponent<TransformComponent>()->SetPosition(32 + 64 * (19 - i), Engine::GetInstance().GetWindowSize().bottom / 2.);
				fadeOut->SetStartTimer(i * 1 / 120.);
				fadeOut->SetTimer(5.0);
				RenderInfo info{};
				info.isFixed = true;
				fadeOut->SetRenderInfo(info);
				fadeOut->SetIsDeadBySpriteEnd(false);
				objectManager->AddObject(ObjectType::EffectOverUI, fadeOut);
			}
		} break;
		case SceneState::Pause:
		{

		} break;
	}
	currState = newState;
}

void FactoryScene::Unload()
{
	objectManager->Clear();

}

void FactoryScene::Render()
{
	objectManager->Render();
}

