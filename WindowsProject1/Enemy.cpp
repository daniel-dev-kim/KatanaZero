#include "pch.h"
#include "Enemy.h"

#include "Engine.h"
#include "AudioManager.h"
#include "InputSystem.h"
#include "TextureManager.h"
#include "Random.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "TextureComponent.h"

#include "SpriteManagerComponent.h"
#include "ObjectManager.h"

#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"

#include "Terrain.h"
#include "Item.h"
#include "Player.h"
#include "Particle.h"
#include "LaserBeam.h"
#include "EnemyMark.h"
constexpr float DETECTING_RANGE = 500;
constexpr float MARK_GAP = 55;
Enemy::Enemy(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	p = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());

	myMark = std::make_shared<EnemyMark>(owner, ObjectType::UI);

	owner->AddObject(ObjectType::UI, myMark);
}

void Enemy::Update(double dt) 
{
	
	if (owner->GetFrameIndex() == 0)
	{
		inSmoke = false;
		hit = false;
		willDie = false;
		wallStop = false;
		myMark->currentMark = EnemyMark::MarkType::None;
		GetComponent<SpriteManagerComponent>()->Freeze(false);
	}
	auto transform = GetComponent<TransformComponent>();

	auto markTransform = myMark->GetComponent<TransformComponent>();
	markTransform->SetPosition(transform->GetPosition() + glm::vec2(0, MARK_GAP));


	auto terrains = owner->GetObjectList(ObjectType::Terrain);
	if (!currentGround) {
		double enemyAnkle = transform->GetPosition().y - GetComponent<AABBCollisionComponent>()->GetSize().y / 2 + 20.;
		double highestUnderAnkle = 0.;
		for (auto terrain : terrains)
		{
			auto terrainLineCollision = terrain->GetComponent<LineCollisionComponent>();
			if (terrainLineCollision)
			{
				auto [result, y] = terrainLineCollision->CheckCollide(this);
				if (!result || y > enemyAnkle) continue;
				if (y > highestUnderAnkle)
				{
					highestUnderAnkle = y;
					currentGround = terrainLineCollision;
				}
			}
		}
	}

	wallStop = false;
	for (auto terrain : terrains)
	{
		auto terrainAABB = terrain->GetComponent<AABBCollisionComponent>();
		if (terrainAABB == nullptr) continue;

		if (terrainAABB->CheckCollide(this))
		{
			terrainAABB->ResolveCollide(this);
			wallStop = true;
		}
	}

	auto doors = owner->GetObjectList(ObjectType::Door);
	for (auto door : doors)
	{
		auto doorAABB = door->GetComponent<AABBCollisionComponent>();

		if (doorAABB->CheckCollide(this))
		{
			doorAABB->ResolveCollide(this);
			wallStop = true;
		}
	}
	if (!hit)
	{
		auto lasers = owner->GetObjectList(ObjectType::Laser);
		for (auto laser : lasers)
		{
			LaserBeam* laserBeam = dynamic_cast<LaserBeam*>(laser.get());
			if (laserBeam)
			{
				auto laserAABB = laserBeam->GetComponent<AABBCollisionComponent>();
				auto temp = laserAABB->GetSize();
				laserAABB->SetSize(temp * 5.0f);
				if (laserAABB->CheckCollide(this))
				{
					laserAABB->ResolveCollide(this);
					wallStop = true;
				}
				laserAABB->SetSize(temp);

			}
		}
	}



	Object::Update(dt);

	if(!currentGround) velocity.y -= GRAVITY * dt;
	if (velocity.y > 0) currentGround = nullptr;
	transform->Translate(velocity * (float)dt);

	if (currentGround)
	{
		// 지형 위에 여전히 있는지 체크
		if (!currentGround->CheckInside(this))
		{
			currentGround = nullptr; // 지형 위에서 벗어났다면 낙하 시작
			auto terrains = GetOwner()->GetObjectList(ObjectType::Terrain);
			double enemyAnkle = transform->GetPosition().y - GetComponent<AABBCollisionComponent>()->GetSize().y / 2 + 20.;
			double highestUnderAnkle = 0.;
			for (auto terrain : terrains)
			{
				auto terrainLineCollision = terrain->GetComponent<LineCollisionComponent>();
				if (terrainLineCollision)
				{
					auto [result, y] = terrainLineCollision->CheckCollide(this);
					if (!result || y > enemyAnkle) continue;
					if (y > highestUnderAnkle)
					{
						highestUnderAnkle = y;
						currentGround = terrainLineCollision;

					}
				}
			}
		}
		else
		{
			// 여전히 지형 위에 있다면 위치를 조정
			currentGround->ResolveCollide(this);
		}
	}
	if (inSmoke)
	{
		myMark->currentMark = EnemyMark::MarkType::Suspicious;
		if (currState != stateSuspicious) ChangeState(stateSuspicious);
	}
	if (!inSmoke)
	{
		smokeTimer += dt;
	}
	inSmoke = false;
}

void Enemy::Render()
{
	if (currState == &stateLaserContact)
	{
		RenderInfo info{};
		info.texture2 = Engine::GetInstance().GetTextureManager()->GetTexture(laserContactKey);
		info.blendFactor = 0.5;
		info.renderRatio.y = stateLaserContact.GetTimer();
		auto sprite = GetComponent<SpriteManagerComponent>();
		stateLaserContact.SetSparkHeight(sprite->GetCurrentSprite()->GetCurrentData()->height * (info.renderRatio.y - 0.5));
		sprite->Render(info);
	}
	else
	{
		Object::Render();
	}
}

bool Enemy::CanSeePlayer()
{
	if (inSmoke) return false;
	if (p->hidden) return false;
	auto playerPos = p->GetComponent<TransformComponent>()->GetPosition();
	auto transform = GetComponent<TransformComponent>();
	auto position = transform->GetPosition();	
	glm::vec2 dir = playerPos - position;
	if (dir.y > 100 || dir.y < -50) return false; 
	float length = glm::length(dir);
	if (length > DETECTING_RANGE) return false; // 너무 멂
	dir = glm::normalize(dir);
	if (transform->GetScale().x * dir.x < 0.) 
	{
		return false; // 다른 방향 쳐다봄
	}
	return LineCasting(position, dir, length);
}

bool Enemy::LineCasting(glm::vec2 start, glm::vec2 dir, float length)
{
	auto terrainList = owner->GetObjectList(ObjectType::Terrain);
	for (float d = 0.f; d <= length; d+=4.f)
	{
		glm::vec2 dot = start + dir * d;
		for (auto terrain : terrainList)
		{
			auto terrainAABB = terrain->GetComponent<AABBCollisionComponent>();
			if (terrainAABB && terrainAABB->CheckCollide(dot))
			{
				return false;
			}
		}
	}

	auto doorList = owner->GetObjectList(ObjectType::Door);
	for (float d = 0.f; d <= length; d += 4.f)
	{
		glm::vec2 dot = start + dir * d;
		for (auto door : doorList)
		{
			auto doorAABB = door->GetComponent<AABBCollisionComponent>();
			if (doorAABB && doorAABB->CheckCollide(dot))
			{
				return false;
			}
		}
	}
	return true;
}

double Enemy::StateLaserContact::GetTimer()
{
	return timer;
}

void Enemy::StateLaserContact::Enter(Object* obj)
{
	Engine::GetInstance().GetAudioManager()->StartSound("actor_disintegrate_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));

	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->Freeze(true);
	auto enemy = dynamic_cast<Enemy*>(obj);
	//enemy->willDie = true;
	timer = 1.5;
	enemy->velocity = { 0, 0 };
	enemy->myMark->currentMark = EnemyMark::MarkType::None;
}

void Enemy::StateLaserContact::Update(Object* obj, double dt)
{
	timer -= dt * 2.;

	if (timer > 0.)
	{
		auto sprite = obj->GetComponent<SpriteManagerComponent>();
		auto enemy = dynamic_cast<Enemy*>(obj);

		auto spark = std::make_shared<Particle>(enemy->owner, ObjectType::BackgroundEffect);
		auto spartTexture = spark->AddComponent<TextureComponent>("electron");
		auto sparkTransform = spark->GetComponent<TransformComponent>();
		spark->SetGravityFactor(3.0);
		spark->SetTimer(0.2);
		spark->SetIsGettingDisappearByTime(true);

		double angle = Engine::GetInstance().GetRandom()->get(-PI - (PI / 4.), PI / 4.);
		sparkTransform->SetPosition(enemy->GetComponent<TransformComponent>()->GetPosition() + glm::vec2{ 0.f, sparkHeight } + glm::vec2{ cos(angle), sin(angle) } *10.f);
		int size = Engine::GetInstance().GetRandom()->get(1., 2.);
		sparkTransform->SetScale(size, size);
		sparkTransform->SetRotation(angle);
		float speed = Engine::GetInstance().GetRandom()->get(300., 2000.);
		spark->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
		enemy->owner->AddObject(ObjectType::BackgroundEffect, spark);
	}
}

void Enemy::StateLaserContact::TestForExit(Object* obj)
{
	if (timer < 1.0);
	{
		auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
		spriteManager->Freeze(false);
	}

}

void Enemy::StateLaserContact::SetSparkHeight(float h)
{
	sparkHeight = h;
}
