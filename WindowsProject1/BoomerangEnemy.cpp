#include "pch.h"
#include "Engine.h"
#include "BoomerangEnemy.h"
#include "ObjectManager.h"
#include "Boomerang.h"
#include "SlowEffectComponent.h"
#include "TransformComponent.h"
#include "CollisionComponent.h"
#include "InfoComponent.h"
#include "PhysicComponent.h"
#include "Tile.h"
BoomerangEnemy::BoomerangEnemy(ObjectManager* _owner, ObjectType objType):Object(_owner,objType)
{
	dir = 1;

	auto transform = AddComponent<TransformComponent>();
	auto collision = AddComponent<CollisionComponent>();
	auto info = AddComponent<InfoComponent>();
	auto physic = AddComponent<PhysicComponent>();
	auto slowEffect = AddComponent<SlowEffectComponent>();

	transform->SetPosition(500.f, 500.f);
	transform->SetScale(50.f, 50.f);

	info->SetInfo(100, 10, 200.f);

	slowEffect->Initialize();

	collision->Initialize();

	physic->Initialize();

	/*-------------무기 추가----------------*/
	pWeapon = std::make_shared<Boomerang>(owner, ObjectType::EWeapon, this);

	/*-------------------------------------*/

	ChangeState(&boomerangEnemyIdle);
}

void BoomerangEnemy::Update(double dt)
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

void BoomerangEnemy::Render(HDC hdc)
{
	Object::Render(hdc);
	if (pWeapon)
	{
		pWeapon->Render(hdc);
	}
}

void BoomerangEnemy::SetDead()
{
	isDead = true;
}

int BoomerangEnemy::GetWeaponType()
{
	return pWeapon->GetWeaponType();
}

void BoomerangEnemy::BoomerangEnemyIdle::Enter(Object* object)
{
	timer = 0.;
}

void BoomerangEnemy::BoomerangEnemyIdle::Update(Object* object, double dt)
{
	timer += dt;

	math::vec2 targetPos = object->GetOwner()->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition();
	math::vec2 pos = object->GetComponent<TransformComponent>()->GetPosition();
	distance = (targetPos - pos).Length();
}

void BoomerangEnemy::BoomerangEnemyIdle::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<BoomerangEnemy*>(object);
	if (timer > ATTACK_COOLDOWN)
	{
		if (distance <= ATTACK_RANGE)
		{
			object->ChangeState(&pObject->boomerangEnemyAttack);
		}
		else
		{
			object->ChangeState(&pObject->boomerangEnemyMove);
		}
	}
	
}

void BoomerangEnemy::BoomerangEnemyMove::Enter(Object* object)
{
	timer = 0.;
	speed = object->GetComponent<InfoComponent>()->GetSpeed();

	direction = object->getDir();
	direction *= -1;
	object->setDir(direction);
}

void BoomerangEnemy::BoomerangEnemyMove::Update(Object* object, double dt)
{
	timer += dt;
	auto transform = object->GetComponent<TransformComponent>();
	float slowRatio = object->GetComponent<SlowEffectComponent>()->getSlowRatio();
	transform->Translate(direction * speed *slowRatio* dt, 0.f);
}

void BoomerangEnemy::BoomerangEnemyMove::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<BoomerangEnemy*>(object);
	if (timer > 1.5)
	{
		object->ChangeState(&pObject->boomerangEnemyIdle);
	}
}

void BoomerangEnemy::BoomerangEnemyAttack::Enter(Object* object)
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

	auto pObject = dynamic_cast<BoomerangEnemy*>(object);
	pObject->pWeapon->GetAttackState() = true;
}

void BoomerangEnemy::BoomerangEnemyAttack::Update(Object* object, double dt)
{
	timer += dt;
}

void BoomerangEnemy::BoomerangEnemyAttack::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<BoomerangEnemy*>(object);
	auto pWeapon = dynamic_cast<Boomerang*>(pObject->pWeapon.get());
	if (!pWeapon->getThrowState())
	{
		object->ChangeState(&pObject->boomerangEnemyIdle);
		pWeapon->GetAttackState() = false;
	}
}
