#include "pch.h"
#include "EnemyMark.h"

#include "SpriteManagerComponent.h"
#include "TransformComponent.h"

EnemyMark::EnemyMark(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	auto transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
	marks = AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_enemy_mark/enemyMarkData.txt");
}

void EnemyMark::Update(double dt)
{
	if (currentMark != MarkType::None)
	{
		Object::Update(dt);
		switch (currentMark)
		{
		case MarkType::Alert:
		{
			if (marks->GetName() != "Enemy_Alert_")
			{
				marks->PlaySprite("Enemy_Alert_");
			}
		} break;
		case MarkType::Suspicious:
		{
			if (marks->GetName() != "Enemy_Suspicious_")
			{
				marks->PlaySprite("Enemy_Suspicious_");
			}
		} break;
		case MarkType::Chase:
		{
			if (marks->GetName() != "Enemy_Chase_")
			{
				marks->PlaySprite("Enemy_Chase_");
			}
		} break;
		}
		
	}

}



void EnemyMark::Render()
{
	if (currentMark != MarkType::None)
	{
		Object::Render();
	}
}
