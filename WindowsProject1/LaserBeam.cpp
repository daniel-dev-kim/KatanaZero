#include "pch.h"
#include "LaserBeam.h"
#include "SpriteManagerComponent.h"
#include "ObjectManager.h"
#include "AABBCollisionComponent.h"
#include "Player.h"
#include "Enemy.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "CameraComponent.h"
#include "AudioManager.h"
LaserBeam::LaserBeam(ObjectManager* owner, ObjectType objType) : Object(owner, objType)
{
	AddComponent<TransformComponent>();
	auto sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_laser/laserSpriteData.txt");
	sprite->PlaySprite("Laser_No_Collide_");
	AddComponent<AABBCollisionComponent>();
}

void LaserBeam::Update(double dt)
{
	if (contactTimer > 0.) contactTimer -= dt;
	Object::Update(dt);
	auto collision = GetComponent<AABBCollisionComponent>();
	auto player = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());
	auto sprite = GetComponent<SpriteManagerComponent>();
	if (contactTimer < 0.2)
	{
		if (player->GetCurrState() != &player->stateLaserContact && collision->CheckCollide(player))
		{
			if (!player->isInvincible)
			{
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
				player->ChangeState(&player->stateLaserContact);
			}
			sprite->PlaySprite("Laser_Collide_");
			contactTimer = 0.3;
			Engine::GetInstance().GetAudioManager()->StartSound("laser_explosion");
		}

		auto enemies = owner->GetObjectList(ObjectType::Enemy);
		for (auto e : enemies)
		{
			auto enemy = dynamic_cast<Enemy*>(e.get());
			if (enemy->GetCurrState() != &enemy->stateLaserContact && collision->CheckCollide(enemy))
			{
				sprite->PlaySprite("Laser_Collide_");
				enemy->ChangeState(&enemy->stateLaserContact);
				contactTimer = 0.3;
				Engine::GetInstance().GetAudioManager()->StartSound("laser_explosion");
			}
		}
	}

	GetComponent<TransformComponent>()->SetScale(1. + contactTimer * 5.0, 0.79);
	if (contactTimer < 0.0)
	{
		contactTimer = 0.0;
		sprite->PlaySprite("Laser_No_Collide_");
	}

}

void LaserBeam::Render()
{
	RenderInfo info{};
	info.frameSize.y = GetComponent<TransformComponent>()->GetScale().y;
	Object::Render();
}
