#include "pch.h"
#include "Warning.h"
#include "TransformComponent.h"
#include "ObjectManager.h"
#include "Enemy.h"
#include "EnemyMark.h"
#include "AABBCollisionComponent.h"
Warning::Warning(ObjectManager* owner, ObjectType type, float size, bool chase) : Object(owner, type), size(size), chase(chase)
{
	transform = AddComponent<TransformComponent>();
}

void Warning::Update(double dt)
{
	auto enemies = owner->GetObjectList(ObjectType::Enemy);
	for (auto e : enemies)
	{
		auto enemy = dynamic_cast<Enemy*>(e.get());
		if (enemy->willDie) continue;
		auto enemyPos = enemy->GetComponent<TransformComponent>()->GetPosition();
		auto dist = glm::distance(enemyPos, transform->GetPosition());
		if (dist < size)
		{
			bool CanSeeThrough = true;
			auto terrainList = owner->GetObjectList(ObjectType::Terrain);
			auto start = transform->GetPosition();
			auto dir = glm::normalize(enemyPos - start);
			for (float d = 0.f; d <= dist; d += 4.f)
			{
				glm::vec2 dot = start + dir * d;
				for (auto terrain : terrainList)
				{
					auto terrainAABB = terrain->GetComponent<AABBCollisionComponent>();
					if (terrainAABB && terrainAABB->CheckCollide(dot))
					{
						CanSeeThrough = false;
					}
				}
			}

			auto doorList = owner->GetObjectList(ObjectType::Door);
			for (float d = 0.f; d <= dist; d += 4.f)
			{
				glm::vec2 dot = start + dir * d;
				for (auto door : doorList)
				{
					auto doorAABB = door->GetComponent<AABBCollisionComponent>();
					if (doorAABB && doorAABB->CheckCollide(dot))
					{
						CanSeeThrough = false;
					}
				}
			}
			if (!CanSeeThrough) continue;
			
			if (!chase)
			{
				if (enemy->myMark->currentMark == EnemyMark::MarkType::None)
				{
					enemy->alertedPosition = transform->GetPosition();
					enemy->myMark->currentMark = EnemyMark::MarkType::Alert;
					enemy->ChangeState(enemy->stateAlert);
				}
				
			}
			else
			{
				if (enemy->myMark->currentMark != EnemyMark::MarkType::Chase)
				{
					enemy->myMark->currentMark = EnemyMark::MarkType::Chase;
					enemy->ChangeState(enemy->stateChase);
				}
			}
		}
	}



	SetDead();
}
