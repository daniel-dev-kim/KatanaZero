#include "pch.h"
#include "BossAura.h"

#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "Boss.h"

BossAura::BossAura(ObjectManager* owner, ObjectType type, Boss* boss) : Object(owner, type), boss(boss)
{
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Aura/auraSpriteData.txt");
	sprite->PlaySprite("aura");

	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
}

void BossAura::Update(double dt)
{
	Object::Update(dt);
	alpha += dt;
	std::clamp(alpha, 0.0, 0.6);
	transform->SetPosition(boss->GetComponent<TransformComponent>()->GetPosition());
}

void BossAura::Render()
{
	RenderInfo info{};
	info.color.a = alpha;
	sprite->Render(info);
}
