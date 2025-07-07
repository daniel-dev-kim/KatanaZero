#include "pch.h"
#include "EndingScene.h"
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
#include "BossDead.h"
void EndingScene::Load()
{
	Scene::Load();
	// 플레이어
	{
		player = std::make_shared<Player>(objectManager.get(), ObjectType::Player);
		player->GetComponent<TransformComponent>()->SetPosition(590, 229);
		player->CanMove = false;
		objectManager->AddObject(ObjectType::Player, player);
		auto sprite = player->GetComponent<SpriteManagerComponent>();
		sprite->PlaySprite("Player_Sit_");
	}

	// 시체
	{
		bossDead = std::make_shared<BossDead>(objectManager.get(), ObjectType::Enemy);
		bossDead->GetComponent<TransformComponent>()->SetPosition(725, 227);
		objectManager->AddObject(ObjectType::Enemy, bossDead);
	}

	{ //난로
		auto firePlace = std::make_shared<Light>(objectManager.get(), ObjectType::Background, glm::vec3{ 1.f, 0.27f , 0.f });
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
		auto bg = std::make_shared<Background>(objectManager.get(), ObjectType::Background);
		auto texture = bg->AddComponent<TextureComponent>("bg_boss_front", "Resource/Texture/Background/bg_boss/bg_psychiatrist_0.png");
		auto transform = bg->AddComponent<TransformComponent>();
		transform->SetPosition(texture->GetSize() / 1.9f);
		Engine::GetInstance().playerCam->SetWorldBounds({ 0.f, 0.f }, texture->GetSize());
		objectManager->AddObject(ObjectType::Background, bg);
	}
	
	//배경
	{
		ending = std::make_shared<Background>(objectManager.get(), ObjectType::Background);
		ending->alpha = 0.;
		auto texture = ending->AddComponent<TextureComponent>("bg_ending_credit", "Resource/Texture/Background/bg_boss/ending.png");
		auto transform = ending->AddComponent<TransformComponent>();
		transform->SetPosition({640, 360});
		transform->SetScale(1.6, 1.6);
		objectManager->AddObject(ObjectType::Background, ending);
	}

	// 지형
	{
		auto t1 = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		t1->AddLine({ {125, 219}, {301, 219}, {323, 195}, {1290, 195} });
		t1->SetWallThroughable(false);
		objectManager->AddObject(ObjectType::Terrain, t1);
		player->currentGround = t1->GetComponent<LineCollisionComponent>();

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
	Engine::GetInstance().GetAudioManager()->StartSound("sound_object_fireplace_loop_01");
	Engine::GetInstance().GetAudioManager()->StartSound("song_nocturne");
	currState = EndingState::Sit;
	timer = 0.;
	Engine::GetInstance().uGlitchEnabled = true;
	Engine::GetInstance().uGlitchSeed = 0.8;
}

void EndingScene::Update(double dt)
{
	
	objectManager->Update(dt);
	auto playerSprite = player->GetComponent<SpriteManagerComponent>();
	auto playerTransform = player->GetComponent<TransformComponent>();
	switch (currState)
	{
	case EndingState::Sit:
	{
		timer += dt;

		if (timer > 0.5)
		{
			Engine::GetInstance().uGlitchEnabled = false;
		}
		if (timer > 5.5)
		{
			timer = 0.;
			currState = EndingState::Stand;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_player_chair_stand");
			playerSprite->PlaySprite("Player_Stand_Up_");
		}
	} break;
	case EndingState::Stand:
	{
		if (playerSprite->IsDone())
		{
			timer += dt;
		}
		if (timer > 0.5)
		{
			timer = 0.;
			currState = EndingState::Walk;
			playerSprite->PlaySprite("Player_Casual_Walk_");

			player->SetVelocity({ 100, 0 });
		}
	} break;
	case EndingState::Walk:
	{
		if (playerSprite->GetCurrentSprite()->GetCurrentFrame() == 1 || playerSprite->GetCurrentSprite()->GetCurrentFrame() == 5)
		{
			if (!footstep)
			{
				footstep = true;
				Engine::GetInstance().GetAudioManager()->StartSound("generic_enemy_walk_1");
			}
		}
		else
		{
			footstep = false;
		}
		if (playerTransform->GetPosition().x > 690.)
		{
			currState = EndingState::BringSwordBack;
			bossDead->GetComponent<TransformComponent>()->SetScale(0, 0);
			playerTransform->SetPosition(690., playerTransform->GetPosition().y);
			player->SetVelocity({ 0, 0 });
			bossDead->GetComponent<SpriteManagerComponent>()->PlaySprite("sit_without_katana");
			playerSprite->PlaySprite("Player_Bring_Sword_Back_");
		}
	} break;
	case EndingState::BringSwordBack:
	{
		if (playerSprite->GetCurrentSprite()->GetCurrentFrame() == 2)
		{
			if (!footstep)
			{
				footstep = true;
				Engine::GetInstance().GetAudioManager()->StartSound("sound_player_sword_retrieve_01");
			}
		}
		else
		{
			footstep = false;
		}

		if (playerSprite->IsDone())
		{
			currState = EndingState::Wait;
			timer = 0.;
			playerSprite->PlaySprite("Player_Casual_Idle_");
			bossDead->GetComponent<TransformComponent>()->SetScale(2, 2);
		}
	} break;
	case EndingState::Wait:
	{
		timer += dt;
		if (timer > 1.0)
		{
			timer = 0.;
			playerTransform->ScaleBy(-1, 1);
			playerSprite->PlaySprite("Player_Casual_Walk_");
			player->SetVelocity({ -100, 0 });
			currState = EndingState::WalkBack;
		}
	} break;
	case EndingState::WalkBack:
	{
		if (playerSprite->GetCurrentSprite()->GetCurrentFrame() == 1 || playerSprite->GetCurrentSprite()->GetCurrentFrame() == 5)
		{
			if (!footstep)
			{
				footstep = true;
				Engine::GetInstance().GetAudioManager()->StartSound("generic_enemy_walk_1");
			}
		}
		else
		{
			footstep = false;
		}
		if (playerTransform->GetPosition().x < 640.)
		{
			player->SetVelocity({ 0, 0 });
			currState = EndingState::Wait2;
			timer = 0.;
			playerSprite->PlaySprite("Player_Casual_Idle_");
		}
	} break;
	case EndingState::Wait2:
	{
		timer += dt;
		if (timer > 4.0)
		{
			timer = 0.;
			playerSprite->PlaySprite("Player_Dance_");
			Engine::GetInstance().GetAudioManager()->StopSound("sound_object_fireplace_loop_01");
			Engine::GetInstance().GetAudioManager()->StopSound("song_nocturne");
			Engine::GetInstance().GetAudioManager()->StartSound("endingSong");

			currState = EndingState::Dance;
		}
	} break;
	case EndingState::Dance:
	{
		ending->alpha += dt;
	} break;
	}

}

void EndingScene::Render()
{
	objectManager->Render();
}

void EndingScene::Unload()
{
	objectManager->Clear();
}