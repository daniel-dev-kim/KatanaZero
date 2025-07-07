#include "pch.h"
#include "YouCanDoThis.h"

#include "Engine.h"
#include "TextureManager.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "Player.h"
#include "ObjectManager.h"
YouCanDoThis::YouCanDoThis(ObjectManager* owner, ObjectType obyType) : Object(owner, objType)
{
	texture = AddComponent<TextureComponent>("YouCanDoThis", "Resource/Texture/Effect/YouCanDoThis/0.png");
	auto transform = AddComponent<TransformComponent>();
	transform->SetPosition(texture->GetSize() * 0.5f);
	transform->SetScale(1.05, 1.05);
}

void YouCanDoThis::Update(double dt)
{
	timer += dt;
	if (!OMReset && timer > 1.0)
	{
		OMReset = true;
		owner->SetState(ObjectManager::OMState::SetUpReplaying);
		Engine::GetInstance().mono = true;
	}
	if (timer > 2.5)
	{
		SetDead();
	}
}

void YouCanDoThis::Render()
{
	RenderInfo info{};
	if (timer < 0.5)
	{
		info.color.a = timer * 2;
	}
	else if (timer < 2.0)
	{
		info.color.a = 1.;
	}
	else
	{
		info.color.a = 1. - 2 * (timer - 2);
	}
	info.isFixed = true;
	texture->Render(GetComponent<TransformComponent>()->GetModelMatrix(), info);
}
