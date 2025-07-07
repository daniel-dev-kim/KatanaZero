#include "pch.h"
#include "BossFire.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "AABBCollisionComponent.h"
#include "CameraComponent.h"
#include "Engine.h"
#include "AudioManager.h"
#include "ObjectManager.h"
#include "LineCollisionComponent.h"
#include "Random.h"
#include "Player.h"
BossFire::BossFire(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	transform = AddComponent<TransformComponent>();
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Fire/Fire.txt");
	sprite->PlaySprite("fire_loop");
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 30, 30 });
}

void BossFire::Update(double dt)
{
	Object::Update(dt);
	transform->Translate(velocity * (float)dt);

	if (!reflected)
	{
		auto attacks = owner->GetObjectList(ObjectType::PBullet);
		for (auto a : attacks)
		{
			if (!hit && AABB->CheckCollide(a.get()))
			{
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
				Engine::GetInstance().GetAudioManager()->StartSound("slash_bullet");
				reflected = true;
				velocity *= -1.f;
			}
		}
		auto player = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());
		if (!player->isInvincible && !reflected && !hit && !player->hit && AABB->CheckCollide(player))
		{
			player->hit = true;
			player->willDie = true;
			player->currentGround = nullptr;
			player->ChangeState(&player->stateHurtFlyBegin);
			player->SetVelocity({ velocity.x, 400 });
			Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
			Crash();
		}

	}
	if(hit && sprite->IsDone())
	{
		SetDead();
	}

	auto terrains = owner->GetObjectList(ObjectType::Terrain);
	for (auto terrain : terrains)
	{
		auto Line = terrain->GetComponent<LineCollisionComponent>();
		if (Line && Line->CheckCollide(this).first)
		{
			SetDead();
		}
	}
}

void BossFire::Crash()
{
	hit = true;
	SetVelocity({ 0, 0 });
	sprite->PlaySprite("fire_die");
	Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_blastimpact_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));

}
