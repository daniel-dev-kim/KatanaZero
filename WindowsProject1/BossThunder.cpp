#include "pch.h"
#include "BossThunder.h"

#include "TextureComponent.h"
#include "TransformComponent.h"
#include "Boss.h"

BossThunder::BossThunder(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	texture = AddComponent<TextureComponent>("Boss_Thunder", "Resource/Texture/Effect/Boss/thunder/boss_thunder.png");

	transform = AddComponent<TransformComponent>();
	transform->SetPosition(640, 360);
	alpha = 1.0;
}

void BossThunder::Update(double dt)
{
	Object::Update(dt);
	alpha -= dt * 3;
	if (alpha < 0.0)
	{
		SetDead();
	}
}

void BossThunder::Render()
{
	RenderInfo info{};
	info.color.a = alpha;
	texture->Render(transform->GetModelMatrix(), info);
}
