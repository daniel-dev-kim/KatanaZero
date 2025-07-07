#include "pch.h"
#include "BossSpark.h"

#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "Boss.h"

#include "Engine.h"
#include "AudioManager.h"
BossSpark::BossSpark(ObjectManager* owner, ObjectType type, Boss* boss) : Object(owner, type), boss(boss)
{
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/Boss/spark/sparkSpriteData.txt");
	sprite->PlaySprite("spark");

	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
}

void BossSpark::Update(double dt)
{
	Object::Update(dt);
	transform->SetPosition(boss->GetComponent<TransformComponent>()->GetPosition());
}
