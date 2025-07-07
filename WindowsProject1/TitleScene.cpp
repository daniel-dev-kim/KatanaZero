#include "pch.h"
#include "TitleScene.h"
#include "Object.h"
#include "TextureManager.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "SpriteManagerComponent.h"
#include "CameraComponent.h"
#include "AABBCollisionComponent.h"
#include "ObjectManager.h"
#include "Engine.h"
#include "Random.h"
#include "AudioManager.h"
#include "InputSystem.h"
#include "SceneManager.h"
void TitleScene::Load()
{
	Scene::Load();
	Engine::GetInstance().playerCam = nullptr;
	{
		bg = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		
		auto transform = bg->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(bgStartPosition);
		auto texture = bg->AddComponent<TextureComponent>("bg_title", "Resource/Texture/Background/Title/spr_title_background_0.png");
		objectManager->AddObject(ObjectType::Background, bg);
	}
	{
		fence = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = fence->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(fenceStartPosition);
		auto texture = fence->AddComponent<TextureComponent>("bg_title_fence", "Resource/Texture/Background/Title/spr_title_fence_0.png");
		objectManager->AddObject(ObjectType::Background, fence);
	}
	constexpr double LETTERSIZE = 1.5;
	{
		ZERO = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = ZERO->AddComponent<TransformComponent>();
		transform->SetScale(LETTERSIZE, LETTERSIZE);
		transform->SetPosition(ZEROStartPosition);
		auto texture = ZERO->AddComponent<TextureComponent>("bg_title_ZERO", "Resource/Texture/Background/Title/spr_titlegraphic_big_0.png");
		objectManager->AddObject(ObjectType::Background, ZERO);
	}

	{
		ZER = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = ZER->AddComponent<TransformComponent>();
		transform->SetScale(LETTERSIZE, LETTERSIZE);
		transform->SetPosition(ZERStartPosition);
		auto texture = ZER->AddComponent<TextureComponent>("bg_title_ZER", "Resource/Texture/Background/Title/spr_titlegraphic_big_1.png");
		objectManager->AddObject(ObjectType::Background, ZER);
	}
	{
		O = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = O->AddComponent<TransformComponent>();
		transform->SetScale(LETTERSIZE, LETTERSIZE);
		transform->SetPosition(OStartPosition);
		auto texture = O->AddComponent<TextureComponent>("bg_title_O", "Resource/Texture/Background/Title/spr_titlegraphic_big_2.png");
		objectManager->AddObject(ObjectType::Background, O);
	}

	{
		KATANA = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = KATANA->AddComponent<TransformComponent>();
		transform->SetScale(LETTERSIZE, LETTERSIZE);
		transform->SetPosition(KATANAStartPosition);
		auto texture = KATANA->AddComponent<TextureComponent>("bg_title_KATANA", "Resource/Texture/Background/Title/spr_titlegraphic_big2_0.png");
		objectManager->AddObject(ObjectType::Background, KATANA);
	}

	{
		grass = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = grass->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(grassStartPosition);
		auto texture = grass->AddComponent<TextureComponent>("bg_title_grass", "Resource/Texture/Background/Title/spr_title_grass_0.png");
		objectManager->AddObject(ObjectType::Background, grass);
	}

	{
		plant = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = plant->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(plantStartPosition);
		auto sprite = plant->AddComponent<SpriteManagerComponent>("Resource/Texture/Background/Title/bg_plant.txt");
		sprite->PlaySprite("plant");
		objectManager->AddObject(ObjectType::Background, plant);
	}

	{
		textBack = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = textBack->AddComponent<TransformComponent>();
		transform->SetScale(1, 0.7);
		transform->SetPosition(textBackStartPosition);
		auto texture = textBack->AddComponent<TextureComponent>("textBack", "Resource/Texture/Background/Title/textBack.png");
		objectManager->AddObject(ObjectType::Background, textBack);
	}

	{
		chooseBack = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		chooseBack->useSpecial = true;

		auto transform = chooseBack->AddComponent<TransformComponent>();
		transform->SetScale(1, 1);
		transform->SetPosition(chooseBackStartPosition);
		auto texture = chooseBack->AddComponent<TextureComponent>("chooseBack", "Resource/Texture/Background/Title/chooseBack.png");
		objectManager->AddObject(ObjectType::Background, chooseBack);
	}

	constexpr double textColor = 0.85;

	{
		newGame = std::make_shared<Object>(objectManager.get(), ObjectType::Background); 
		newGame->useSpecial = true;
		newGame->specialInfo.color = { textColor, textColor, textColor, 1.0 };
		auto transform = newGame->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(newGameStartPosition);
		auto texture = newGame->AddComponent<TextureComponent>("new_game", "Resource/Texture/String/new_game.png");
		objectManager->AddObject(ObjectType::Background, newGame);
	}
	{
		continueGame = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		continueGame->useSpecial = true;
		continueGame->specialInfo.color = { textColor, textColor, textColor, 1.0 };
		auto transform = continueGame->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(continueGameStartPosition);
		auto texture = continueGame->AddComponent<TextureComponent>("continue_game", "Resource/Texture/String/Continue_Game.png");
		objectManager->AddObject(ObjectType::Background, continueGame);
	}
	{
		setting = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		setting->useSpecial = true;
		setting->specialInfo.color = { textColor, textColor, textColor, 1.0 };
		auto transform = setting->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(settingStartPosition);
		auto texture = setting->AddComponent<TextureComponent>("setting", "Resource/Texture/String/settings.png");
		objectManager->AddObject(ObjectType::Background, setting);
	}
	{
		language = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		language->useSpecial = true;
		language->specialInfo.color = { textColor, textColor, textColor, 1.0 };
		auto transform = language->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(languageStartPosition);
		auto texture = language->AddComponent<TextureComponent>("language", "Resource/Texture/String/language_korean.png");
		objectManager->AddObject(ObjectType::Background, language);
	}
	{
		exit = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		exit->useSpecial = true;
		exit->specialInfo.color = { textColor, textColor, textColor, 1.0 };
		auto transform = exit->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(exitStartPosition);
		auto texture = exit->AddComponent<TextureComponent>("exit", "Resource/Texture/String/exit.png");
		objectManager->AddObject(ObjectType::Background, exit);
	}
	{
		black = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
		auto transform = black->AddComponent<TransformComponent>();
		transform->SetScale(2, 2);
		transform->SetPosition(blackStartPosition);
		auto texture = black->AddComponent<TextureComponent>("bg_dark", "Resource/Texture/Background/Title/bg_dark.png");
		objectManager->AddObject(ObjectType::Background, black);
	}
	choose = 0;
	Off = false;
	neonTimer = 0.;
	neonCooldown = 0.;
	flickTimer = 0.;
	flickCount = 0;

	pivot = 180.0;
	pivot2 = 0.0;
	pivot3 = 0.0;
	state = TitleState::Down;
	upTimer = 0.;
	menuTimer = 0.;
	downTimer = 0.;
}

void TitleScene::Update(double dt)
{
	objectManager->Update(dt);

	int ranNum = Engine::GetInstance().GetRandom()->get(0, 100);
	if(state != TitleState::Up) neonCooldown += dt;
	if (neonCooldown > 0. && !ranNum)
	{
		neonCooldown = -1.0;
		Off = true;
		O->GetComponent<TransformComponent>()->SetScale(0, 0);
	}
	if (Off)
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_object_neon_flicker_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 6)));
		neonTimer += dt;
		if (neonTimer > 0.05)
		{
			O->GetComponent<TransformComponent>()->SetScale(1.5, 1.5);
			neonTimer = 0.;
			Off = false;
		}
	}

	switch (state)
	{
	case TitleState::Down:
	{
		downTimer += dt;
		if (downTimer < 1.)
		{
			if (downTimer > 1.)
				downTimer = 1.;

			float t = downTimer / 1.;

			pivot = 720.f * t * (2 - t);       // 결과: 0 → 720 (감속)
			pivot2 = (pivot > 540.f) ? 720.f * ((t - 0.5f) * 2) * (2 - (t - 0.5f) * 2) : 0.f;
			if (pivot2 > 720.f) pivot2 = 720.f;
		}
		else
		{
			pivot = 720.f;
			pivot2 = 720.f;
			Engine::GetInstance().GetAudioManager()->StartSound("song_rainonbrick");

			state = TitleState::MenuUp;
		}

		bg->GetComponent<TransformComponent>()->SetPosition(bgStartPosition + glm::vec2(0, pivot));
		fence->GetComponent<TransformComponent>()->SetPosition(fenceStartPosition + glm::vec2(0, pivot));
		ZERO->GetComponent<TransformComponent>()->SetPosition(ZEROStartPosition + glm::vec2(0, pivot));
		ZER->GetComponent<TransformComponent>()->SetPosition(ZERStartPosition + glm::vec2(0, pivot));
		O->GetComponent<TransformComponent>()->SetPosition(OStartPosition + glm::vec2(0, pivot));
		KATANA->GetComponent<TransformComponent>()->SetPosition(KATANAStartPosition + glm::vec2(0, pivot));
		grass->GetComponent<TransformComponent>()->SetPosition(grassStartPosition + glm::vec2(0, pivot2));
		plant->GetComponent<TransformComponent>()->SetPosition(plantStartPosition + glm::vec2(0, pivot2));
		black->GetComponent<TransformComponent>()->SetPosition(blackStartPosition + glm::vec2(0, pivot2));
	} break;
	case TitleState::MenuUp:
	{
		menuTimer += dt;
		if (menuTimer < 1.)
		{
			if (menuTimer > 1.)
				menuTimer = 1.;

			float t = menuTimer / 1.;

			pivot3 = 570. * t * (2 - t);       // 결과: 0 → 720 (감속)
		}
		else
		{
			pivot3 = 570.;
			state = TitleState::Idle;
		}
		textBack->GetComponent<TransformComponent>()->SetPosition(textBackStartPosition + glm::vec2(0, pivot3));
		newGame->GetComponent<TransformComponent>()->SetPosition(newGameStartPosition + glm::vec2(0, pivot3));
		continueGame->GetComponent<TransformComponent>()->SetPosition(continueGameStartPosition + glm::vec2(0, pivot3));
		setting->GetComponent<TransformComponent>()->SetPosition(settingStartPosition + glm::vec2(0, pivot3));
		language->GetComponent<TransformComponent>()->SetPosition(languageStartPosition + glm::vec2(0, pivot3));
		exit->GetComponent<TransformComponent>()->SetPosition(exitStartPosition + glm::vec2(0, pivot3));
	} break;
	case TitleState::Idle:
	{
		pivot3 = 570.;
		auto inputSystem = Engine::GetInstance().GetInputSystem();
		if (inputSystem->IsKeyPressed(InputKey::S))
		{
			Engine::GetInstance().GetAudioManager()->StartSound("sound_menubeep_2");
			choose++;
		}
		else if (inputSystem->IsKeyPressed(InputKey::W))
		{
			Engine::GetInstance().GetAudioManager()->StartSound("sound_menubeep_2");
			choose--;
		}

		choose %= 5;
		switch (choose)
		{
		case 0:		
			chooseBack->GetComponent<TransformComponent>()->SetPosition(newGameStartPosition + glm::vec2(0, pivot3));
			break;
		case 1:
			chooseBack->GetComponent<TransformComponent>()->SetPosition(continueGameStartPosition + glm::vec2(0, pivot3));
			break;
		case 2:
			chooseBack->GetComponent<TransformComponent>()->SetPosition(settingStartPosition + glm::vec2(0, pivot3));
			break;
		case 3:
			chooseBack->GetComponent<TransformComponent>()->SetPosition(languageStartPosition + glm::vec2(0, pivot3));
			break;
		case 4:
			chooseBack->GetComponent<TransformComponent>()->SetPosition(exitStartPosition + glm::vec2(0, pivot3));
			break;
		}

		if (inputSystem->IsKeyPressed(InputKey::Space))
		{
			Engine::GetInstance().GetAudioManager()->StartSound("sound_menubeep_1");
			state = TitleState::Choose;
		}
	} break;
	case TitleState::Choose:
	{
		if (flickTimer < 0.025)
		{
			flickCount++;
			chooseBack->specialInfo.color = {0.4, 0.4 , 0.4, 1.0};
		}
		else if (flickTimer < 0.05)
		{
			chooseBack->specialInfo.color = { 1., 1. , 1., 1.0 };
		}
		else if(flickTimer > 0.05)
		{
			flickTimer = 0.;
		}
		if (flickCount < 20)
		{
			flickTimer += dt;
		}
		else
		{
			chooseBack->specialInfo.color = { 1., 1. , 1., 1.0 };
			upTimer += dt;
		}
		if (upTimer > 0.3)
		{
			upTimer = 0.;
			//Engine::GetInstance().GetAudioManager()->StartSound("sound_ui_transition_drone_01");
			Engine::GetInstance().GetAudioManager()->StopSound("song_rainonbrick");
			state = TitleState::Up;
			{
				grass = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
				auto transform = grass->AddComponent<TransformComponent>();
				transform->SetScale(2, 2);
				transform->SetPosition(grassStartPosition + glm::vec2{ 0., pivot2 });
				auto texture = grass->AddComponent<TextureComponent>("bg_title_grass", "Resource/Texture/Background/Title/spr_title_grass_0.png");
				objectManager->AddObject(ObjectType::Background, grass);
			}

			{
				plant = std::make_shared<Object>(objectManager.get(), ObjectType::Background);
				auto transform = plant->AddComponent<TransformComponent>();
				transform->SetScale(2, 2);
				transform->SetPosition(plantStartPosition + glm::vec2{0., pivot2});
				auto sprite = plant->AddComponent<SpriteManagerComponent>("Resource/Texture/Background/Title/bg_plant.txt");
				sprite->PlaySprite("plant");
				objectManager->AddObject(ObjectType::Background, plant);
			}
		}
	} break;
	case TitleState::Up:
	{
		upTimer += dt;
		if (upTimer < 1.)
		{
			if (upTimer > 1.)
				upTimer = 1.;

			float t = upTimer / 1.;
			pivot = 720 + 720 * t * (2 - t);
			pivot2 = 720 + 720 * t * (2 - t);
		}
		else
		{
			pivot = 1440;
			pivot2 = 1440;
			Engine::GetInstance().GetSceneManager()->SetActiveScene("BossScene");
		}
		bg->GetComponent<TransformComponent>()->SetPosition(bgStartPosition + glm::vec2(0, pivot));
		fence->GetComponent<TransformComponent>()->SetPosition(fenceStartPosition + glm::vec2(0, pivot));
		ZERO->GetComponent<TransformComponent>()->SetPosition(ZEROStartPosition + glm::vec2(0, pivot));
		ZER->GetComponent<TransformComponent>()->SetPosition(ZERStartPosition + glm::vec2(0, pivot));
		O->GetComponent<TransformComponent>()->SetPosition(OStartPosition + glm::vec2(0, pivot));
		KATANA->GetComponent<TransformComponent>()->SetPosition(KATANAStartPosition + glm::vec2(0, pivot));
		grass->GetComponent<TransformComponent>()->SetPosition(grassStartPosition + glm::vec2(0, pivot2));
		plant->GetComponent<TransformComponent>()->SetPosition(plantStartPosition + glm::vec2(0, pivot2));
		black->GetComponent<TransformComponent>()->SetPosition(blackStartPosition + glm::vec2(0, pivot2));
	} break;
	}
}

void TitleScene::Render()
{
	objectManager->Render();
}

void TitleScene::Unload()
{
	objectManager->Clear();
}
