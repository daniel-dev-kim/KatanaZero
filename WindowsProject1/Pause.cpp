#include "pch.h"
#include "Pause.h"
#include "TextureComponent.h"
#include "Engine.h"
#include "InputSystem.h"
#include "ObjectManager.h"
#include "AudioManager.h"
#include "SceneManager.h"
#include "Scene.h"
Pause::Pause(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	blue = std::make_shared<TextureComponent>("pausemenu_blue", "Resource/Texture/UI/ui_pause/spr_pausemenu_bg_0.png");
	pink = std::make_shared<TextureComponent>("pausemenu_pink", "Resource/Texture/UI/ui_pause/spr_pausemenu_bg_2.png");
	white = std::make_shared<TextureComponent>("pausemenu_white", "Resource/Texture/UI/ui_pause/pauseBrighter.png");
	pauseChoose = std::make_shared<TextureComponent>("pausemenu_choose", "Resource/Texture/UI/ui_pause/pauseChoose.png");
	menuKatana = std::make_shared<TextureComponent>("pausemenu_katana", "Resource/Texture/UI/ui_pause/spr_menukatana_0.png");
	pauseString = std::make_shared<TextureComponent>("pause_string", "Resource/Texture/String/KATANA_ZERO_PauseMenu.png");
	keepString = std::make_shared<TextureComponent>("continue_game", "Resource/Texture/String/Continue_Game.png");
	menuString = std::make_shared<TextureComponent>("main_menu", "Resource/Texture/String/Main_Menu.png");
}

void Pause::Update(double dt)
{
	pinkColorChange += dt * PI;
	timer += dt;
	if (timer > 0.5)
	{
		drawKatana = !drawKatana;
		timer = 0.;
	}
	auto inputSystem = Engine::GetInstance().GetInputSystem();
	if (inputSystem->IsKeyPressed(InputKey::S))
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_menubeep_2");

		pinkColorChange = 0.;
		selected++;
	}
	else if (inputSystem->IsKeyPressed(InputKey::W))
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_menubeep_2");

		pinkColorChange = 0.;
		selected--;
	}

	selected %= 2;


	if (inputSystem->IsKeyPressed(InputKey::Space))
	{
		Engine::GetInstance().GetAudioManager()->StartSound("sound_menubeep_1");
		selectTimer = 0.;
		switch (selected)
		{
		case 0:
		{
			confirmed = 0;
		} break;
		case 1:
		{
			confirmed = 1;
		} break;
		}
	}

	if (confirmed >= 0)
	{
		selectTimer += dt;
		if (selectTimer > 0.7)
		{
			switch (confirmed)
			{
			case 0:
			{
				Engine::GetInstance().GetAudioManager()->ResumeSound(Engine::GetInstance().GetSceneManager()->activeScene->bgmName);
				Engine::GetInstance().GetAudioManager()->StopSound("static");

				confirmed = -1;
				owner->pauseSwitch = false;
			} break;
			case 1:
			{
				Engine::GetInstance().GetAudioManager()->StopSound(Engine::GetInstance().GetSceneManager()->activeScene->bgmName);
				Engine::GetInstance().GetAudioManager()->StopSound("static");
				Engine::GetInstance().GetSceneManager()->SetActiveScene("TitleScene");
			} break;
			}
		}
	}
}

void Pause::Render()
{
	RenderInfo info{};
	info.timer = pinkColorChange;
	info.isFixed = true;
	{
		auto blueTransform = glm::translate(glm::mat4(1.0), glm::vec3(640.f, 333.f, 0.f));
		blueTransform = glm::scale(blueTransform, glm::vec3(2.0, 2.0, 1.0));
		blue->Render(blueTransform, info);
	}
	{
		auto pinkTransform = glm::translate(glm::mat4(1.0), glm::vec3(640.f, 693.f, 0.f));
		pinkTransform = glm::scale(pinkTransform, glm::vec3(2.0, 2.0, 1.0));
		pink->Render(pinkTransform, info);
	}


	{
		info.uEnableTVNoise = true;
		info.uTVNoiseWidth = 0.0001;
		info.color.a = 0.3 ;
		auto whiteTransform = glm::translate(glm::mat4(1.0), glm::vec3(640.f, 360.f, 0.f));
		whiteTransform = glm::scale(whiteTransform, glm::vec3(2.0, 2.0, 1.0));
		white->Render(whiteTransform, info);
		info.color.a = 1.0;
		info.uEnableTVNoise = false;
	}

	auto pauseStringTransform = glm::translate(glm::mat4(1.0), glm::vec3(230.f, 693.f, 0.f));
	pauseStringTransform = glm::scale(pauseStringTransform, glm::vec3(3.0, 3.0, 1.0));
	pauseString->Render(pauseStringTransform, info);


	auto keepStringTransform = glm::translate(glm::mat4(1.0), glm::vec3(132.5f, 110.f, 0.f));
	keepStringTransform = glm::scale(keepStringTransform, glm::vec3(3.0, 3.0, 1.0));
	

	auto menuStringTransform = glm::translate(glm::mat4(1.0), glm::vec3(140.f, 200.f, 0.f));
	menuStringTransform = glm::scale(menuStringTransform, glm::vec3(3.0, 3.0, 1.0));
	
	{
		if(confirmed != -1 &&selectTimer < 0.5) info.color = glm::vec4{ 1.0, 1.0, 1.0, 0.2 } * (float)(0.75f + sin(selectTimer * 80) * 0.25);
		else info.color = { 1.0, 1.0, 1.0, 0.2 };
		info.uEnableTVNoise = true;
		info.uTVNoiseWidth = 0.003;
		glm::mat4 allign = glm::translate(glm::mat4(1.0), glm::vec3(500.f, 0.f, 0.f));

		switch (selected)
		{
		case 0:
			pauseChoose->Render(allign * keepStringTransform, info);
			break;
		case 1:
			pauseChoose->Render(allign * menuStringTransform, info);
			break;
		}
		info.uEnableTVNoise = false;
	}
	{
		
		switch (selected)
		{
		case 0:
			info.color = { 1.0, sin(pinkColorChange) * 0.5 + 0.5, 1.0 , 1.0 };  // ºÐÈ«»ö
			keepString->Render(keepStringTransform, info);
			info.color = { 0.2, 1.0, 1.0 , 1.0 };  // ÇÏ´Ã»ö
			menuString->Render(menuStringTransform, info);
			break;
		case 1:
			info.color = { 0.2, 1.0, 1.0 , 1.0 };  // ÇÏ´Ã»ö
			keepString->Render(keepStringTransform, info);
			info.color = { 1.0, sin(pinkColorChange) * 0.5 + 0.5, 1.0 , 1.0 };  // ºÐÈ«»ö
			menuString->Render(menuStringTransform, info);
			break;
		}

	}
	if(drawKatana)
	{
		info.color = { 1.0, 1.0, 1.0 , 1.0 };
		auto menuKatanaTransform = glm::translate(glm::mat4(1.0), glm::vec3(1210.f, 693.f, 0.f));
		menuKatanaTransform = glm::scale(menuKatanaTransform, glm::vec3(4.0, 4.0, 1.0));
		menuKatana->Render(menuKatanaTransform, info);

	}
}
