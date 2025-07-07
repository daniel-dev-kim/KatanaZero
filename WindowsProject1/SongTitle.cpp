#include "pch.h"
#include "SongTitle.h"

#include "TextureComponent.h"
#include "TransformComponent.h"

#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "random.h"
SongTitle::SongTitle(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	glm::vec2 position = {1000, 100};
	back = std::make_shared<TextureComponent>("SongTitleBack", "Resource/Texture/UI/ui_songTitleBack/songTitleBack.png");
	backTransform = std::make_shared<TransformComponent>();
	backTransform->SetPosition(position + glm::vec2{0., 0.});
	backTransform->SetScale(1.0, 1.0);

	middle = std::make_shared<TextureComponent>("SongTitleMiddle", "Resource/Texture/UI/ui_songTitleBack/songTitleMiddle.png");
	middleTransform = std::make_shared<TransformComponent>();
	middleTransform->SetPosition(position + glm::vec2{ -99., -1. });
	middleTransform->SetScale(1.2, 1.);

	string1 = std::make_shared<TextureComponent>("PlayingSong", "Resource/Texture/string/Now_Playing.png");
	string1Transform = std::make_shared<TransformComponent>();
	string1Transform->SetPosition(position + glm::vec2{ -111., 18});
	string1Transform->SetScale(2., 2.);

	string2 = std::make_shared<TextureComponent>("YouWillNeverKnow", "Resource/Texture/string/You_Will_Never_Know_-_Bill_Kiley.png");
	string2Transform = std::make_shared<TransformComponent>();
	string2Transform->SetPosition(position + glm::vec2{ -34., -16});
	string2Transform->SetScale(1.7, 1.7);
	currStatus = Status::BackBegin;
}

void SongTitle::Update(double dt)
{
	switch (currStatus)
	{
		case Status::BackBegin:
		{
			backWidth += dt * 2;
			backWidth = std::clamp(backWidth, 0., 1.);
			if (backWidth == 1.0) currStatus = Status::String1Begin;
		} break;
		case Status::String1Begin:
		{
			string1Alpha += dt * 2;
			string1Alpha = std::clamp(string1Alpha, 0., 1.);
			if (string1Alpha == 1.0) currStatus = Status::MiddleString2Begin;
		} break;
		case Status::MiddleString2Begin:
		{
			middleWidth += dt * 2;
			middleWidth = std::clamp(middleWidth, 0., 1.);

			string2Alpha += dt * 2;
			string2Alpha = std::clamp(string2Alpha, 0., 1.);
			if (string2Alpha == 1.0) currStatus = Status::Blinking;
		} break;
		case Status::Blinking:
		{
			if (Engine::GetInstance().GetSceneManager()->activeScene->currState == Scene::SceneState::Play)
			{
				currStatus = Status::DisappearInside;
			}
		} break;
		case Status::DisappearInside:
		{
			string1Alpha -= dt * 2;
			string1Alpha = std::clamp(string1Alpha, 0., 1.);

			middleWidth -= dt * 2;
			middleWidth = std::clamp(middleWidth, 0., 1.);

			string2Alpha -= dt * 2;
			string2Alpha = std::clamp(string2Alpha, 0., 1.);
			if (string2Alpha == 0.0) currStatus = Status::DisappearOutside;
		} break;
		case Status::DisappearOutside:
		{
			backHeight -= dt * 2;
			if (backHeight == 0.0) SetDead();
		} break;
	}
}


void SongTitle::Render()
{
	glm::vec3 pink(1.0, 0.2, 0.6);  // ºÐÈ«»ö
	glm::vec3 skyblue(0.2, 1.0, 1.0);  // ÇÏ´Ã»ö
	RenderInfo info{};
	info.isFixed = true;
	info.renderRatio = { backWidth , backHeight };
	back->Render(backTransform->GetModelMatrix(), info);
	
	info.renderRatio = { middleWidth , middleHeight };
	middle->Render(middleTransform->GetModelMatrix(), info);
	
	info.renderRatio = { 1, 1 };
	info.color = { pink , string1Alpha };
	string1->Render(string1Transform->GetModelMatrix(), info);
	info.color = { skyblue , string1Alpha };
	double gap = Engine::GetInstance().GetRandom()->get(0., 1.);
	string1Transform->Translate(gap, 0);
	string1->Render(string1Transform->GetModelMatrix(), info);
	string1Transform->Translate(-gap, 0);
	info.color = { skyblue , string2Alpha };
	string2->Render(string2Transform->GetModelMatrix(), info);
}
