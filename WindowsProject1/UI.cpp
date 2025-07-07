#include "pch.h"
#include "UI.h"
#include "TextureComponent.h"
#include "Engine.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Random.h"
#include "SceneManager.h"
#include "Scene.h"
UI::UI(ObjectManager* owner, ObjectType objType) : Object(owner, objType)
{
	p = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());
	mainHUD.first = std::make_shared<TextureComponent>("mainHUD", "Resource/Texture/UI/ui_mainHUD/0.png");
	mainHUD.second = std::make_shared<TextureComponent>("mainHUD_darkened", "Resource/Texture/UI/ui_mainHUD/0_darkened.png");

	batteryCells.resize(p->totalBattery + 1);
	for (int i = 0; i <= p->totalBattery; i++)
	{
		batteryCells[i].first = std::make_shared<TextureComponent>("BatteryCells_" + std::to_string(i), "Resource/Texture/UI/ui_battery/" + std::to_string(i) + ".png");
		batteryCells[i].second = std::make_shared<TextureComponent>("BatteryCells_" + std::to_string(i) + "_darkened", "Resource/Texture/UI/ui_battery/" + std::to_string(i) + "_darkened.png");
	}
	ShiftDown = std::make_shared<TextureComponent>("ShiftDown", "Resource/Texture/UI/ui_battery/s" + std::to_string(1) + ".png");

	ShiftUp = std::make_shared<TextureComponent>("ShiftUp", "Resource/Texture/UI/ui_battery/s" + std::to_string(0) + ".png");

	timerTotal.first = std::make_shared<TextureComponent>("timerTotal", "Resource/Texture/UI/ui_timerbar/0.png");
	timerTotal.second = std::make_shared<TextureComponent>("timerTotal_darkened", "Resource/Texture/UI/ui_timerbar/0_darkened.png");

	timerRemain.first = std::make_shared<TextureComponent>("timerRemain", "Resource/Texture/UI/ui_timerbar/1.png");
	timerRemain.second = std::make_shared<TextureComponent>("timerRemain_darkened", "Resource/Texture/UI/ui_timerbar/1_darkened.png");

	inven.push_back({std::make_shared<TextureComponent>("invenHand", "Resource/Texture/UI/ui_inven/0.png"), std::make_shared<TextureComponent>("invenHand_darkened", "Resource/Texture/UI/ui_inven/0_darkened.png") });
	inven.push_back({std::make_shared<TextureComponent>("invenKnife", "Resource/Texture/UI/ui_inven/1.png"), std::make_shared<TextureComponent>("invenKnife_darkened", "Resource/Texture/UI/ui_inven/1_darkened.png") });
	inven.push_back({std::make_shared<TextureComponent>("invenGas", "Resource/Texture/UI/ui_inven/2.png"), std::make_shared<TextureComponent>("invenGas_darkened", "Resource/Texture/UI/ui_inven/2_darkened.png") });
	inven.push_back({std::make_shared<TextureComponent>("invenExplosive", "Resource/Texture/UI/ui_inven/3.png"), std::make_shared<TextureComponent>("invenExplosive_darkened", "Resource/Texture/UI/ui_inven/3_darkened.png") });
	
	mouseLeft = std::make_shared<TextureComponent>("mouseLeft", "Resource/Texture/UI/ui_inven/4.png");
	mouseRight = std::make_shared<TextureComponent>("mouseRight", "Resource/Texture/UI/ui_inven/5.png");

	gameoverFrame = std::make_shared<TextureComponent>("gameoverFrame", "Resource/Texture/UI/ui_gameover/GameOverFrame.png");
	gameover.push_back(std::make_shared<TextureComponent>("gameover_0", "Resource/Texture/UI/ui_gameover/0.png"));
	gameover.push_back(std::make_shared<TextureComponent>("gameover_1", "Resource/Texture/UI/ui_gameover/1.png"));
	gameover.push_back(std::make_shared<TextureComponent>("gameover_2", "Resource/Texture/UI/ui_gameover/2.png"));
}

void UI::Update(double dt)
{
	Object::Update(dt);
	if (yGap != 0.0 && Engine::GetInstance().GetSceneManager()->activeScene->currState == Scene::SceneState::Play)
	{
		yGap += dt * 120;
		yGap = std::clamp(yGap, -80.0, 0.0);
	}
}
constexpr int CHANCE = 100;

void UI::Render()
{
	RenderInfo info{};
	info.isFixed = true;

	if (Engine::GetInstance().GetSceneManager()->activeScene->currState == Scene::SceneState::Die)
	{
		glm::vec2 middle = glm::vec2{ Engine::GetInstance().GetWindowSize().right, Engine::GetInstance().GetWindowSize().bottom } / 2.f;
		glm::mat4 translate = glm::translate(glm::mat4(1.0), { middle, 0. });
		gameoverFrame->Render(translate, info);

		int num = Engine::GetInstance().GetRandom()->get(0, CHANCE);
		if (num == 0)
		{
			gameoverMessage = 2;
		}
		else if (gameoverMessage || num < 20)
		{
			if (gameoverMessage == 0)
			{
				gameoverMessage = 1;
			}
			else
			{
				gameoverMessage = 0;
			}
		}
		gameover[gameoverMessage]->Render(translate, info);
	}


	{
		int num = Engine::GetInstance().GetRandom()->get(0, CHANCE / 2);
		glm::mat4 translate = glm::translate(glm::mat4(1.0), { 640., 23. + yGap, 0. });
		glm::mat4 scale = glm::scale(glm::mat4(1.0), { 2., 2., 1 });

		if(num == 0) mainHUD.second->Render( translate * scale, info);
		else mainHUD.first->Render( translate * scale, info);
	}

	{
		int num = Engine::GetInstance().GetRandom()->get(0, CHANCE);
		glm::mat4 translate = glm::translate(glm::mat4(1.0), { 90., 23. + yGap, 0. });
		glm::mat4 scale = glm::scale(glm::mat4(1.0), { 2.3, 2.1 , 1 });

		if (num == 0) batteryCells[p->currentBattery].second->Render( translate * scale, info);
		else batteryCells[p->currentBattery].first->Render( translate * scale, info);

	}

	{
		glm::mat4 translateDown = glm::translate(glm::mat4(1.0), {210., 24. + yGap, 0.});
		glm::mat4 translateUp = glm::translate(glm::mat4(1.0), { 210., 23. + yGap, 0. });
		glm::mat4 scale = glm::scale(glm::mat4(1.0), { 1.8, 1.8, 1 });
		
		if(p->slowSkill) ShiftDown->Render( translateDown * scale, info);
		else ShiftUp->Render( translateUp * scale, info);
	}

	{
		int num = Engine::GetInstance().GetRandom()->get(0, CHANCE);

		{
		glm::mat4 translate = glm::translate(glm::mat4(1.0), {640., 23. + yGap, 0. });
		glm::mat4 scale = glm::scale(glm::mat4(1.0), { 2.3, 2.3, 1 });

		if (num == 0) timerTotal.second->Render( translate * scale, info);
		else timerTotal.first->Render( translate * scale, info);
		}
		{
			
			info.renderRatio.x = p->memoryTimer / 60.;
			glm::mat4 translate = glm::translate(glm::mat4(1.0), {658., 19. + yGap, 0. });
			glm::mat4 scale = glm::scale(glm::mat4(1.0), { 2.31, 2.32, 1 });

			if (num == 0) timerRemain.second->Render( translate * scale, info);
			else timerRemain.first->Render( translate * scale, info);
		}
	}
	info.renderRatio.x = 1.0; //게이지바
	{
		int num = Engine::GetInstance().GetRandom()->get(0, CHANCE);

		glm::mat4 translate = glm::translate(glm::mat4(1.0), {1200., 23. + yGap, 0. });
		glm::mat4 scale = glm::scale(glm::mat4(1.0), { 2.3, 2.3, 1 });
		if (num == 0) 
			inven[static_cast<int>(p->currItem)].second->Render( translate * scale, info);
		else 
			inven[static_cast<int>(p->currItem)].first->Render( translate * scale, info);
	}

	{
		glm::mat4 translate = glm::translate(glm::mat4(1.0), {1188., 43. + yGap, 0. });
		glm::mat4 scale = glm::scale(glm::mat4(1.0), { 2.3, 2.3, 1 });
		mouseLeft->Render( translate * scale, info);
	}

	{
		glm::mat4 translate = glm::translate(glm::mat4(1.0), {1260., 43. + yGap, 0. });
		glm::mat4 scale = glm::scale(glm::mat4(1.0), { 2.3, 2.3, 1 });
		mouseRight->Render( translate * scale, info);
	}
}
