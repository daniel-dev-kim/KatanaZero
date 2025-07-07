#include "pch.h"
#include "ToStart.h"

#include "TextureComponent.h"
#include "TransformComponent.h"

#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "random.h"
ToStart::ToStart(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	glm::vec2 position = glm::vec2{ Engine::GetInstance().GetOriginalWindowSize().right , Engine::GetInstance().GetOriginalWindowSize().bottom} * 0.5f;
	
	name = std::make_shared<TextureComponent>("ToStartName", "Resource/Texture/String/factory_hideout.png");
	nameTransform1 = std::make_shared<TransformComponent>();
	nameTransform2 = std::make_shared<TransformComponent>();
	nameTransform1->SetPosition(position + glm::vec2{ string1X, 0. });
	nameTransform1->SetScale(4, 4);
	nameTransform2->SetPosition(position + glm::vec2{ string2X, 0. });
	nameTransform2->SetScale(4, 4);

	click1 = std::make_shared<TextureComponent>("ToStartClick_0", "Resource/Texture/String/Start_Plan_By_Attacking_0.png");
	click2 = std::make_shared<TextureComponent>("ToStartClick_1", "Resource/Texture/String/Start_Plan_By_Attacking_1.png");
	clickTransform = std::make_shared<TransformComponent>();
	clickTransform->SetPosition(position + glm::vec2{ 0., -80. });
	clickTransform->SetScale(2, 2);

	back = std::make_shared<TextureComponent>("ToStartBack", "Resource/Texture/UI/ui_tostartback/tostartback.png");
	backTransform = std::make_shared<TransformComponent>();
	backTransform->SetPosition(position + glm::vec2{ 0., 0. });

	currStatus = Status::Begin;
}

void ToStart::Update(double dt)
{
	glm::vec2 position = glm::vec2{ Engine::GetInstance().GetOriginalWindowSize().right , Engine::GetInstance().GetOriginalWindowSize().bottom } *0.5f;

	switch (currStatus)
	{
	case Status::Begin:
	{
		backAlpha += dt * 2;
		backAlpha = std::clamp(backAlpha, 0.0, 1.0);
		if (backAlpha == 1.0) currStatus = Status::StringMove;
	} break;
	case Status::StringMove:
	{
		auto lerp = [](double a, double b, double t) {
			return a + (b - a) * t;
			};
		double speed = 5.0; // 속도 조절용 (값이 클수록 빠르게 멈춤)
		string1X = lerp(string1X, 0.0, dt * speed);
		string2X = lerp(string2X, 0.0, dt * speed);

		nameTransform1->SetPosition(position + glm::vec2{ string1X, 0. });
		nameTransform2->SetPosition(position + glm::vec2{ string2X, 0. });

		// 근사값을 비교해서 0에 거의 도달했을 때 상태 변경
		if (std::abs(string1X) < 1.0 && std::abs(string2X) < 1.0)
		{
			string1X = 0.0;
			string2X = 0.0;
			currStatus = Status::Blinking;
		}
	} break;
	case Status::Blinking:
	{
		useClick1Timer += dt;
		if (useClick1Timer > 0.5)
		{
			useClick1 = !useClick1;
			useClick1Timer = 0.0;
		}

		nameTransform1->SetPosition(position + glm::vec2{ Engine::GetInstance().GetRandom()->get(-2.0, 0.0), Engine::GetInstance().GetRandom()->get(-2.0, 0.0) });
		nameTransform2->SetPosition(position + glm::vec2{ Engine::GetInstance().GetRandom()->get(0.0, 2.0), Engine::GetInstance().GetRandom()->get(0.0, 2.0) });

		if (Engine::GetInstance().GetSceneManager()->activeScene->currState == Scene::SceneState::ZigZag)
		{
			SetDead();
		}
	} break;
	}
}

void ToStart::Render()
{
	glm::vec3 pink(1.0, 0.2, 0.6);  // 분홍색
	glm::vec3 skyblue(0.2, 1.0, 1.0);  // 하늘색
	RenderInfo info{};
	info.isFixed = true;
	info.color.a = backAlpha;
	back->Render(backTransform->GetModelMatrix(), info);
	info.color.a = 1.f;
	if (currStatus == Status::Blinking)
	{
		if (useClick1)
		{
			click1->Render(clickTransform->GetModelMatrix(), info);
		}
		else
		{
			click2->Render(clickTransform->GetModelMatrix(), info);
		}
	}

	info.color = glm::vec4(pink, 0.9);
	name->Render(nameTransform1->GetModelMatrix(), info);
	info.color = glm::vec4(skyblue, 0.9);
	name->Render(nameTransform2->GetModelMatrix(), info);
}

