#include "pch.h"
#include "ObjectManager.h"
#include "SlowGunEnemy.h"
#include "SlowGun.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "InfoComponent.h"
#include "PhysicComponent.h"
#include "SlowEffectComponent.h"
#include "Tile.h"
SlowGunEnemy::SlowGunEnemy(ObjectManager* _owner, ObjectType objType):Object(_owner,objType)
{
	dir = 1;

	auto transform = AddComponent<TransformComponent>();
	auto collision = AddComponent<CollisionComponent>();
	auto info = AddComponent<InfoComponent>();
	auto physic = AddComponent<PhysicComponent>();
	auto slowEffect = AddComponent<SlowEffectComponent>();


	transform->SetPosition(500.f, 500.f);
	transform->SetScale(50.f, 50.f);

	slowEffect->Initialize();

	info->SetInfo(100, 10, 200.f);

	collision->Initialize();

	physic->Initialize();

	/*-------------무기 추가----------------*/
	pWeapon = std::make_shared<SlowGun>(owner, ObjectType::EWeapon, this);

	/*-------------------------------------*/

	ChangeState(&slowgunEnemyIdle);
}

void SlowGunEnemy::Update(double dt)
{
	Object::Update(dt);
	if (pWeapon)
	{
		pWeapon->Update(dt);
	}

	auto collision = GetComponent<CollisionComponent>();
	auto physics = GetComponent<PhysicComponent>();
	auto transform = GetComponent<TransformComponent>();
	double fSpeed = GetComponent<InfoComponent>()->GetSpeed();

	if (!physics->IsOnGround())
	{
		physics->ApplyGravity(dt);
	}
	else
	{
		physics->SetVelocity(math::vec2{ physics->GetVelocity().x, 0. });
	}

	if (!physics->IsOnSlope())
	{
		physics->SetVelocity({ fSpeed, physics->GetVelocity().y });
	}
	else
	{
		physics->SetVelocity({ fSpeed, 0. });

		math::vec2 slopeDir = physics->GetSlope();
		float projected = physics->GetVelocity().Dot(slopeDir * 0.7071);
		physics->SetVelocity(slopeDir * projected);
	}

	transform->Translate(physics->GetVelocity() * dt);

	bool wasOnSlope = physics->IsOnSlope();
	physics->SetOnGround(false);
	for (auto obj : owner->GetObjectList(ObjectType::Tile))
	{
		Tile* targetTile = dynamic_cast<Tile*>(obj.get());
		const TileType tileType = targetTile->GetType();
		auto tileCollision = targetTile->GetComponent<CollisionComponent>();
		if (tileType == TileType::Square)
		{
			if (collision->CheckAABBCollision(tileCollision))
			{
				//위치 보정 및 상태 설정만
				collision->ResolveAABBCollision(targetTile);
				//slope에서 평지 진입 시 우선권
				if (wasOnSlope) break;
			}
		}
		else
		{
			if (collision->CheckDiagonalCollision(targetTile))
			{
				//위치 보정 및 상태 설정만
				collision->ResolveDiagonalSlide(targetTile);
			}
		}
	}
}

void SlowGunEnemy::Render(HDC hdc)
{
	Object::Render(hdc);
	if (pWeapon)
	{
		pWeapon->Render(hdc);
	}
}

void SlowGunEnemy::SetDead()
{
	isDead = true;
}

int SlowGunEnemy::GetWeaponType()
{
	return pWeapon->GetWeaponType();
}

void SlowGunEnemy::SlowGunEnemyIdle::Enter(Object* object)
{
	timer = 0.;
}

void SlowGunEnemy::SlowGunEnemyIdle::Update(Object* object, double dt)
{
	timer += dt;

	math::vec2 targetPos = object->GetOwner()->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition();
	math::vec2 pos = object->GetComponent<TransformComponent>()->GetPosition();
	distance = (targetPos - pos).Length();
}

void SlowGunEnemy::SlowGunEnemyIdle::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<SlowGunEnemy*>(object);
	if (timer > ATTACK_COOLDOWN)
	{
		if (distance <= ATTACK_RANGE)
		{
			object->ChangeState(&pObject->slowgunEnemyAttack);
		}
		else
		{
			object->ChangeState(&pObject->slowgunEnemyMove);
		}
	}
}

void SlowGunEnemy::SlowGunEnemyMove::Enter(Object* object)
{
	timer = 0.;
	speed = object->GetComponent<InfoComponent>()->GetSpeed();

	direction = object->getDir();
	direction *= -1;
	object->setDir(direction);
}

void SlowGunEnemy::SlowGunEnemyMove::Update(Object* object, double dt)
{
	timer += dt;
	float slowRatio = object->GetComponent<SlowEffectComponent>()->getSlowRatio();
	auto transform = object->GetComponent<TransformComponent>();
	transform->Translate(direction * speed * slowRatio * dt, 0.f);
}

void SlowGunEnemy::SlowGunEnemyMove::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<SlowGunEnemy*>(object);
	if (timer > 1.5)
	{
		object->ChangeState(&pObject->slowgunEnemyIdle);
	}
}

void SlowGunEnemy::SlowGunEnemyAttack::Enter(Object* object)
{
	timer = 0.;
	targetPos = object->GetOwner()->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition();
	math::vec2 pos = object->GetComponent<TransformComponent>()->GetPosition();

	if ((targetPos - pos).Normalize().x < 0)
	{
		direction = -1.f;
	}
	else direction = 1.f;

	object->setDir(direction);

	auto pObject = dynamic_cast<SlowGunEnemy*>(object);
	pObject->pWeapon->GetAttackState() = true;
}

void SlowGunEnemy::SlowGunEnemyAttack::Update(Object* object, double dt)
{
	timer += dt;
}

void SlowGunEnemy::SlowGunEnemyAttack::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<SlowGunEnemy*>(object);
	auto pWeapon = dynamic_cast<SlowGun*>(pObject->pWeapon.get());
	if (timer>1.5)
	{
		object->ChangeState(&pObject->slowgunEnemyIdle);
		pWeapon->GetAttackState() = false;
	}
}
