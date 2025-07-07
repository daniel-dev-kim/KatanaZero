#include "pch.h"
#include "HammerEnemy.h"
#include "TransformComponent.h"
#include "CollisionComponent.h"
#include "InfoComponent.h"
#include "PhysicComponent.h"
#include "Hammer.h"
#include "ObjectManager.h"
#include "SlowEffectComponent.h"
#include "Weapon.h"
#include "Tile.h"

HammerEnemy::HammerEnemy(ObjectManager* owner, ObjectType type)
	: Object(owner, type, RenderType::Ellipse)
{
	defaultDistance = 200.f;
	auto transform = AddComponent<TransformComponent>();
	transform->SetPosition(200.f, 700.f);
	transform->SetScale(50.f, 50.f);
	centerPos = transform->GetPosition();
	rightendPos = { centerPos.x + defaultDistance, centerPos.y };
	leftendPos = { centerPos.x - defaultDistance, centerPos.y };

	auto slowEffect = AddComponent<SlowEffectComponent>();
	slowEffect->Initialize();

	auto collision = AddComponent<CollisionComponent>();
	collision->Initialize();

	auto info = AddComponent<InfoComponent>();
	info->Initialize();
	info->SetInfo(1, 1, 300.f);

	auto physic = AddComponent<PhysicComponent>();
	physic->Initialize();

	ChangeState(&hammerenemyStart);

	m_pWeapon = std::make_shared<Hammer>(owner, ObjectType::EWeapon, this);

}

void HammerEnemy::Update(double dt)
{
	if (IsDead()) {
		m_pWeapon->SetDead();
		return;
	}
	Object::Update(dt);
	if (m_pWeapon)
	{
		m_pWeapon->Update(dt);
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

void HammerEnemy::Render(HDC hdc)
{
	Object::Render(hdc);
	m_pWeapon->Render(hdc);
}

void HammerEnemy::SetDead()
{
	ChangeState(&hammerenemyDead);
}

int HammerEnemy::GetWeaponType()
{
	return m_pWeapon->GetWeaponType();
}

void HammerEnemy::checkPlayerPos()
{
	//플레이어와 x좌표 차이를 계산
	playerPos = owner->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition();
	auto transform = GetComponent<TransformComponent>();
	playerGap = transform->GetPosition().x - playerPos.x;
	if (playerGap <= 0) {
		SetLookDir(1);
		setDir(1);
	}
	else {
		SetLookDir(-1);
		setDir(-1);
	}

	playerGap = abs(playerGap);

}

void HammerEnemy::HammerEnemyStart::Enter(Object* object)
{
	timer = 0;
}
void HammerEnemy::HammerEnemyStart::Update(Object* object, double dt)
{
	timer += dt;
	srand(timer);
}

void HammerEnemy::HammerEnemyStart::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);
	if (object->IsDead())
		object->ChangeState(&pObject->hammerenemyDead);
	else {
		pObject->checkPlayerPos();

		if (pObject->GetPGap() > pObject->defaultDistance) {
			int r = rand() % 100;
			if (r < 1)
				object->ChangeState(&pObject->hammerenemyIdle);
			else
				object->ChangeState(&pObject->hammerenemyMove);
		}
		else if (pObject->GetPGap() <= pObject->defaultDistance
			&& pObject->GetPGap() > 100.f)
		{
			object->ChangeState(&pObject->hammerenemyAttackMove);
		}
		else if (pObject->GetPGap() <= 100.f) {
			object->ChangeState(&pObject->hammerenemyAttack);
		}
	}
}

void HammerEnemy::HammerEnemyIdle::Enter(Object* object)
{
	timer = 0;
}

void HammerEnemy::HammerEnemyIdle::Update(Object* object, double dt)
{
	timer += dt;
}

void HammerEnemy::HammerEnemyIdle::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);

	if (timer > 1.f)						//가만히 서 있는 시간
		object->ChangeState(&pObject->hammerenemyStart);
}

void HammerEnemy::HammerEnemyMove::Enter(Object* object)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);
	auto transform = object->GetComponent<TransformComponent>();


	startPos = transform->GetPosition();
	timer = 0;
	speed = object->GetComponent<InfoComponent>()->GetSpeed();
	pObject->SetLookDir(pObject->lastDir);
}

void HammerEnemy::HammerEnemyMove::Update(Object* object, double dt)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);
	auto transform = object->GetComponent<TransformComponent>();
	math::vec2 nowPos{ 0, 0 };
	float slowRatio = object->GetComponent<SlowEffectComponent>()->getSlowRatio();

	//범위 내에서 벗어나면 방향 전환
	if (transform->GetPosition().x < pObject->leftendPos.x) {
		pObject->SetLookDir(1);
	}
	else if (transform->GetPosition().x > pObject->rightendPos.x) {
		pObject->SetLookDir(-1);
	}

	// 방향으로 움직이기
	switch (pObject->GetLookDir()) {
	case -1:
		nowPos.x -= speed;
		break;
	case 1:
		nowPos.x += speed;
		break;
	}
	transform->Translate(nowPos * slowRatio * dt);
	timer += dt;

}

void HammerEnemy::HammerEnemyMove::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);
	if (timer > 0.1f) {
		pObject->lastDir = pObject->GetLookDir();
		object->ChangeState(&pObject->hammerenemyStart);
	}
}

void HammerEnemy::HammerEnemyAttack::Enter(Object* object)
{
	timer = 0;
	object->setDir(object->getDir());
	dynamic_cast<HammerEnemy*>(object)->m_pWeapon->SetAttackState(true);
}

void HammerEnemy::HammerEnemyAttack::Update(Object* object, double dt)
{
	timer += dt;
}

void HammerEnemy::HammerEnemyAttack::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);
	auto hammer = dynamic_cast<Hammer*>(pObject->m_pWeapon.get());
	if (timer > 1.f) {						//가만히 서 있는 시간
		pObject->m_pWeapon->SetAttackState(false);
		hammer->resetACnt();
		object->ChangeState(&pObject->hammerenemyStart);
	}
}

void HammerEnemy::HammerEnemyAttackMove::Enter(Object* object)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);
	pObject->checkPlayerPos();

	timer = 0.f;
	speed = object->GetComponent<InfoComponent>()->GetSpeed();
	startPos = object->GetComponent<TransformComponent>()->GetPosition();

	switch (pObject->GetLookDir()) {
	case -1:
		dstPos.x = object->GetOwner()->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition().x + 70.f;
		break;
	case 1:
		dstPos.x = object->GetOwner()->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition().x - 70.f;
		break;
	}

	dstPos.y = startPos.y;
}

void HammerEnemy::HammerEnemyAttackMove::Update(Object* object, double dt)
{
	auto transform = object->GetComponent<TransformComponent>();
	auto nowPos = transform->GetPosition();
	endPos = { 0, 0 };
	auto pObject = dynamic_cast<HammerEnemy*>(object);
	object->setDir(object->getDir());

	if (nowPos.x < dstPos.x) {
		pObject->SetLookDir(1);
	}
	else if (nowPos.x > dstPos.x) {
		pObject->SetLookDir(-1);
	}

	switch (pObject->GetLookDir()) {
	case -1:
		endPos.x -= speed;
		break;
	case 1:
		endPos.x += speed;
		break;
	}
	transform->Translate(endPos * dt);
	timer += dt;

}

void HammerEnemy::HammerEnemyAttackMove::TestForExit(Object* object)
{
	auto pObject = dynamic_cast<HammerEnemy*>(object);

	if (timer > 1.f)

		pObject->ChangeState(&pObject->hammerenemyStart);
}

void HammerEnemy::HammerEnemyDead::Enter(Object* object)
{
	//object->RemoveComponent<CollisionComponent>();
}

void HammerEnemy::HammerEnemyDead::Update(Object* object, double dt)
{
}

void HammerEnemy::HammerEnemyDead::TestForExit(Object* object)
{
	dynamic_cast<HammerEnemy*>(object)->m_pWeapon->SetDead();
	object->Object::SetDead();
}
