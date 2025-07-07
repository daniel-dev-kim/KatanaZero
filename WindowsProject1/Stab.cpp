#include "pch.h"
#include "Stab.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "AABBCollisionComponent.h"
#include "SpriteManagerComponent.h"
#include "CameraComponent.h"
#include "Engine.h"
#include "Random.h"
#include "AudioManager.h"
#include "Player.h"
#include "Mutation.h"
Stab::Stab(ObjectManager* owner, ObjectType type, Mutation* mutation, Player* player) : Object(owner, type), mutation(mutation), player(player)
{
	stab = AddComponent<TextureComponent>("stab", "Resource/Texture/Object/boss/obj_stab/0.png");
	transform = AddComponent<TransformComponent>();
	transform->SetPosition(640, 1800);
	transform->SetScale(2, 2);
	transform->SetRotation(-PI / 2);
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 40, 40 });
	AABB->SetOffset({0, -450});
	currentState = StabState::Hide;
}

void Stab::Update(double dt)
{
	switch (currentState)
	{
	case StabState::Hide:
	{
		if (transform->GetPosition().y < 1800)
		{
			velocity.y = 1000;
		}
		else
		{
			velocity.y = 0;
		}
	} break;
	case StabState::Aim:
	{
		auto gapX = player->GetComponent<TransformComponent>()->GetPosition().x - transform->GetPosition().x;
		auto dirX = gapX > 0 ? 1 : -1;
		
		velocity.x = dirX * 1000;
		if (transform->GetPosition().y > 1000)
		{
			velocity.y = -1000;
		}
		else
		{
			velocity.y = 0;
		}
		if (attackCount > 0 && abs(gapX) < 10)
		{
			attackCount--;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akirasword_fly_03");
			currentState = StabState::Attack;
		}
		else if (attackCount == 0)
		{
			mutation->tentacleState = Mutation::TentacleState::Stab_End;
			mutation->tentacle->PlaySprite("tentacle_stab_end");
			velocity.x = 0;
			currentState = StabState::Hide;
			attackCount = Engine::GetInstance().GetRandom()->get(3, 4);
		}
	} break;
	case StabState::Attack:
	{
		velocity.x = 0;
		if (transform->GetPosition().y > 550)
		{
			velocity.y = -2000;
			if (!player->isInvincible && !player->hit && AABB->CheckCollide(player))
			{
				player->hit = true;
				player->willDie = true;
				player->currentGround = nullptr;
				player->ChangeState(&player->stateHurtFlyBegin);
				player->SetVelocity({ velocity.x, 400 });
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
			}
		}
		else
		{
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akirasyringe_stab_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));

			Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG * 1.5);
			velocity.y = 0;
			currentState = StabState::Back;
		}
	} break;
	case StabState::Back:
	{
		auto gapX = player->GetComponent<TransformComponent>()->GetPosition().x - transform->GetPosition().x;
		auto dirX = gapX > 0 ? 1 : -1;

		velocity.x = dirX * 1000;
		if (transform->GetPosition().y < 1000)
		{
			velocity.y = 1000;
		}
		else
		{
			currentState = StabState::Aim;
		}
	} break;

	}

	transform->Translate(velocity * (float)dt);
}

void Stab::Render()
{
	stab->Render(transform->GetModelMatrix());
	AABB->Render();
}

void Stab::StartAttack()
{
	currentState = StabState::Aim;
}
