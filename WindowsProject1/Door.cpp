#include "pch.h"
#include "Door.h"

#include "SpriteManagerComponent.h"
#include "AABBCollisionComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

#include "Engine.h"
#include "AudioManager.h"

#include "ObjectManager.h"
#include "Bullet.h"
Door::Door(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_door/DoorSpriteData.txt");
	
	sprite->PlaySprite("Door_Close_");
	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 20.f, 120.f });
	AABB->SetOffset({-40, 0});
}

void Door::Update(double dt)
{
	if (owner->GetFrameIndex() == 0)
	{
		close = true;
		AABB->SetSize({ 20.f, 120.f });
		sprite->PlaySprite("Door_Close_");
	}

	Object::Update(dt);
	if (close)
	{
		auto playerBullet = owner->GetObjectList(ObjectType::PBullet);
		for (auto pb : playerBullet)
		{
			if (dynamic_cast<Bullet*>(pb.get()) != nullptr) continue; // 실제 총알은 무시
			if (pb->GetComponent<AABBCollisionComponent>()->CheckCollide(this))
			{
				Engine::GetInstance().GetAudioManager()->StartSound("object_door_kick");
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
				AABB->SetSize({ 0, 0 });
				sprite->PlaySprite("Door_Open_");
				close = false;
			}
		}
	}

	
}
