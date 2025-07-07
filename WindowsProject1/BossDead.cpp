#include "pch.h"
#include "BossDead.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
BossDead::BossDead(ObjectManager* owner, ObjectType objType) : Object(owner, objType)
{
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/Boss_Dead/Boss_Dead.txt");
	sprite->PlaySprite("sit_with_katana");
	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
}
