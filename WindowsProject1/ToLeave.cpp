#include "pch.h"
#include "ToLeave.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
ToLeave::ToLeave(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	click1 = std::make_shared<TextureComponent>("ToLeaveClick_0", "Resource/Texture/String/Press_Button_To_Exit_0.png");
	click2 = std::make_shared<TextureComponent>("ToLeaveClick_1", "Resource/Texture/String/Press_Button_To_Exit_1.png");
	clickTransform = std::make_shared<TransformComponent>();
	clickTransform->SetPosition(1100, 50);
	clickTransform->SetScale(2, 2);
}

void ToLeave::Update(double dt)
{
	useClick1Timer += dt;
	if (useClick1Timer > 0.5)
	{
		useClick1 = !useClick1;
		useClick1Timer = 0.0;
	}
}

void ToLeave::Render()
{
	RenderInfo info{};
	info.isFixed = true;
	if (useClick1)
	{
		click1->Render(clickTransform->GetModelMatrix(), info);
	}
	else
	{
		click2->Render(clickTransform->GetModelMatrix(), info);
	}
}
