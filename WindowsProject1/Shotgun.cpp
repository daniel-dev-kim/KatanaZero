#include "pch.h"
#include "Shotgun.h"

#include "Engine.h"
#include "InputSystem.h"
#include "TextureManager.h"
#include "Random.h"
#include "AudioManager.h"

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
#include "Warning.h"
#include "BloodEffect.h"
#include "Bullet.h"
#include "EnemyMark.h"
constexpr float RUN_SPEED = 400.;
constexpr float SHOTGUN_SCALE = 2.;

Shotgun::Shotgun(ObjectManager* owner, ObjectType type) : Enemy(owner, type)
{
	auto transform = AddComponent<TransformComponent>();
	transform->SetPosition(300, 200);
	transform->SetScale(SHOTGUN_SCALE, SHOTGUN_SCALE);

	auto collision = AddComponent<AABBCollisionComponent>();
	collision->SetSize({ 50, 80 });


	auto sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_shotgun/ShotgunSpriteData.txt");
	
	gunPart = std::make_shared<Object>(owner, ObjectType::EnemyArm);
	gunPart->AddComponent<TransformComponent>();
	gunPartSprite = gunPart->AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_shotgun/Arm/ShotgunArmSpriteData.txt");
	gunPartSprite->PlaySprite("Shotgun_Arm_");
	owner->AddObject(ObjectType::EnemyArm, gunPart);

	gunsmoke = "Resource/Texture/Effect/GunFire/gunSmokeData.txt";
	gunspark = "Resource/Texture/Effect/GunFire/gunSparkData.txt";



	initState = &shotgunStateIdle;
	ChangeState(initState);

	stateIdle = &shotgunStateIdle;
	stateWalk = &shotgunStateWalk;
	stateAlert = &shotgunStateAlert;
	stateSuspicious = &shotgunStateSuspicious;
	stateChase = &shotgunStateChase;
	stateAttack = &shotgunStateAttack;
	stateTurn = &shotgunStateTurn;
	stateHurtfly = &shotgunStateHurtFly;
	stateHurtground = &shotgunStateHurtGround;
}

void Shotgun::Update(double dt)
{
	if (owner->GetFrameIndex() == 0)
	{
		willDie = false;
		hit = false;
		if (currState != initState) ChangeState(initState);
	}
	Enemy::Update(dt);

	auto transform = GetComponent<TransformComponent>();
	auto pos = transform->GetPosition();
	auto gunPartTransform = gunPart->GetComponent<TransformComponent>();
	bool seeRight = transform->GetScale().x > 0;
	auto dir = glm::normalize(p->GetComponent<TransformComponent>()->GetPosition() - pos);

	gunPartTransform->SetPosition(pos + glm::vec2(seeRight ? -armPosOffset.x : armPosOffset.x, armPosOffset.y));
	gunPartTransform->SetRotation(atan2(dir.y, dir.x));
	if (currState == stateAttack)
	{
		gunPartTransform->SetScale(glm::vec2(2, seeRight ? 2 : -2));
	}
	else
	{
		gunPartTransform->SetScale(glm::vec2(0, 0));
	}


	if (!willDie && hit)
	{
		willDie = true;
		ChangeState(&shotgunStateHurtFly);
	}

	auto spriteManager = GetComponent<SpriteManagerComponent>();
	if (spriteManager->GetName() == "Shotgun_Run_")
	{
		int footstep = spriteManager->GetCurrentSprite()->GetCurrentFrame();
		if (footStepSwitch && footstep != 0 && footstep != 5)
		{
			footStepSwitch = false;
		}
		if (!footStepSwitch && (footstep == 0 || footstep == 5))
		{
			footStepSwitch = true;
			Engine::GetInstance().GetAudioManager()->StartSound("generic_enemy_run_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));
		}
	}
	else if (spriteManager->GetName() == "Shotgun_Walk_" && glm::length(GetComponent<TransformComponent>()->GetPosition() - owner->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition()) < 400)
	{
		int footstep = spriteManager->GetCurrentSprite()->GetCurrentFrame();
		if (footStepSwitch && footstep != 1 && footstep != 6)
		{
			footStepSwitch = false;
		}
		if (!footStepSwitch && (footstep == 1 || footstep == 6))
		{
			footStepSwitch = true;
			Engine::GetInstance().GetAudioManager()->StartSound("generic_enemy_walk_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));
		}
	}
}

void Shotgun::Render()
{
	Enemy::Render();
}

void Shotgun::SetDead()
{
	Object::SetDead();
	gunPart->SetDead();
}


void Shotgun::ShotgunStateIdle::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Shotgun_Idle_");
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	shotgun->velocity = { 0 ,0 };
}

void Shotgun::ShotgunStateIdle::Update(Object* obj, double dt) {}

void Shotgun::ShotgunStateIdle::TestForExit(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);

	if (shotgun->CanSeePlayer())
	{
		shotgun->myMark->currentMark = EnemyMark::MarkType::Chase;
		shotgun->ChangeState(&shotgun->shotgunStateChase);
	}
}

void Shotgun::ShotgunStateWalk::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Shotgun_Walk_");
	dir = obj->GetComponent<TransformComponent>()->GetScale().x < 0 ? -1 : 1;
}

void Shotgun::ShotgunStateWalk::Update(Object* obj, double dt)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	if (spriteManager->GetName() == "Shotgun_Walk_")
	{
		shotgun->velocity.x = dir * RUN_SPEED / 4.f;
	}
	if (spriteManager->IsDone())
	{
		spriteManager->PlaySprite("Shotgun_Walk_");
	}
}

void Shotgun::ShotgunStateWalk::TestForExit(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	if (shotgun->wallStop)
	{
		shotgun->ChangeState(&shotgun->shotgunStateTurn);
	}
	if (shotgun->CanSeePlayer())
	{
		shotgun->myMark->currentMark = EnemyMark::MarkType::Chase;
		shotgun->ChangeState(&shotgun->shotgunStateChase);
	}
}

void Shotgun::ShotgunStateAlert::Enter(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	shotgun->myMark->currentMark = EnemyMark::MarkType::Alert;
	shotgun->myMark->marks->PlaySprite("Enemy_Alert_");
	auto transform = shotgun->GetComponent<TransformComponent>();
	auto dir = shotgun->alertedPosition - transform->GetPosition();

}

void Shotgun::ShotgunStateAlert::Update(Object* obj, double dt)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	auto transform = shotgun->GetComponent<TransformComponent>();
	auto dir = shotgun->alertedPosition - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!shotgun->wallStop)
	{

		if (spriteManager->GetName() == "Shotgun_Run_")
		{
			shotgun->velocity.x = dir.x > 0 ? RUN_SPEED : -RUN_SPEED;
		}
		if (spriteManager->IsDone())
		{
			spriteManager->PlaySprite("Shotgun_Run_");
		}

	}
}

void Shotgun::ShotgunStateAlert::TestForExit(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	auto transform = shotgun->GetComponent<TransformComponent>();
	auto dir = shotgun->alertedPosition - transform->GetPosition();
	if (transform->GetScale().x * dir.x < 0.)
	{
		shotgun->ChangeState(&shotgun->shotgunStateTurn);

	}
	if (glm::length(dir) < 200)
	{
		shotgun->myMark->currentMark = EnemyMark::MarkType::Suspicious;
		shotgun->ChangeState(&shotgun->shotgunStateSuspicious);
	}
	if (shotgun->CanSeePlayer())
	{
		shotgun->myMark->currentMark = EnemyMark::MarkType::Chase;
		shotgun->ChangeState(&shotgun->shotgunStateChase);
	}
}

void Shotgun::ShotgunStateSuspicious::Enter(Object* obj)
{
	timer = 0.f;
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	shotgun->velocity = { 0, 0 };
	shotgun->myMark->marks->PlaySprite("Enemy_Suspicious_");
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Shotgun_Idle_");
}

void Shotgun::ShotgunStateSuspicious::Update(Object* obj, double dt)
{
	timer += dt;
}

void Shotgun::ShotgunStateSuspicious::TestForExit(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	if (timer > 4.f)
	{
		shotgun->myMark->currentMark = EnemyMark::MarkType::None;
		shotgun->ChangeState(shotgun->initState);
	}
	if (shotgun->CanSeePlayer())
	{
		shotgun->myMark->currentMark = EnemyMark::MarkType::Chase;
		shotgun->ChangeState(&shotgun->shotgunStateChase);
	}
}

void Shotgun::ShotgunStateChase::Enter(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	if (!chaseFirst)
	{
		chaseFirst = true;
		shotgun->myMark->marks->PlaySprite("Enemy_Chase_");
		auto warning = std::make_shared<Warning>(shotgun->owner, ObjectType::Effect, 300.f, true);
		warning->GetComponent<TransformComponent>()->SetPosition(shotgun->GetComponent<TransformComponent>()->GetPosition());
		shotgun->owner->AddObject(ObjectType::Effect, warning);
	}
}

void Shotgun::ShotgunStateChase::Update(Object* obj, double dt) {
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	auto transform = shotgun->GetComponent<TransformComponent>();
	auto playerTransform = shotgun->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!shotgun->wallStop)
	{
		if (glm::length(dir) > 1000.)
		{
			if (spriteManager->GetName() == "Shotgun_Run_")
			{
				shotgun->velocity.x = dir.x > 0 ? RUN_SPEED : -RUN_SPEED;
			}
			if (spriteManager->IsDone())
			{
				spriteManager->PlaySprite("Shotgun_Run_");
			}
		}
	}

}

void Shotgun::ShotgunStateChase::TestForExit(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	auto transform = shotgun->GetComponent<TransformComponent>();
	auto playerTransform = shotgun->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();

	if (glm::length(dir) < 1000)
	{
		//std::cout << glm::length(dir) << std::endl;
		shotgun->ChangeState(&shotgun->shotgunStateAttack);
	}

	else if (transform->GetScale().x * dir.x < 0.)
	{
		//std::cout << "turn" << std::endl;
		shotgun->ChangeState(&shotgun->shotgunStateTurn);
	}
	else if (abs(dir.x) <= 20 || shotgun->wallStop)
	{
		shotgun->velocity.x = 0.;
		if (spriteManager->GetName() == "Shotgun_Run_" || spriteManager->IsDone()) spriteManager->PlaySprite("Shotgun_Idle_");
	}
}

void Shotgun::ShotgunStateAttack::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Shotgun_Aim_");
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	shotgun->velocity = { 0 ,0 };
	shotgun->gunPartSprite->PlaySprite("Shotgun_Arm_");
}

void Shotgun::ShotgunStateAttack::Update(Object* obj, double dt)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);

	if (shotgun->gunPartSprite->IsDone())
	{
		if (shotgun->gunPartSprite->GetName() == "Shotgun_Arm_")
		{
			shotgun->gunPartSprite->PlaySprite("Shotgun_Arm_Load_");
		}
		else
		{
			Engine::GetInstance().GetAudioManager()->StartSound("sound_shotgun_fire");

			shotgun->gunPartSprite->PlaySprite("Shotgun_Arm_");
			auto transform = shotgun->GetComponent<TransformComponent>();
			auto playerTransform = shotgun->p->GetComponent<TransformComponent>();
			auto dir = glm::normalize(playerTransform->GetPosition() - transform->GetPosition());
			float mainAngle = atan2(dir.y, dir.x);
			bool isRight = transform->GetScale().x > 0;
			auto armOffset = glm::vec2{ (isRight ? -1 : 1) * shotgun->armPosOffset.x, shotgun->armPosOffset.y };
			auto gunOffset = glm::vec2(98, isRight ? 4 : -4);
			glm::vec2 rotateOffset = glm::rotate(gunOffset, mainAngle);
			auto bulletStartPosition = shotgun->GetComponent<TransformComponent>()->GetPosition() + armOffset + rotateOffset;
			{
				auto particle = std::make_shared<Particle>(shotgun->owner, ObjectType::Effect);
				particle->SetIsDeadBySpriteEnd(true);
				auto transform = particle->GetComponent<TransformComponent>();
				transform->SetPosition(bulletStartPosition);
				transform->SetScale(2, isRight ? 2 : -2);
				transform->SetRotation(mainAngle);
				auto particleSprite = particle->AddComponent<SpriteManagerComponent>(shotgun->gunspark);
				particleSprite->PlaySprite("Gunspark3_");
				shotgun->owner->AddObject(ObjectType::Effect, particle);
			}
			{
				auto particle = std::make_shared<Particle>(shotgun->owner, ObjectType::Effect);			
				particle->SetIsDeadBySpriteEnd(true);
				auto transform = particle->GetComponent<TransformComponent>();
				transform->SetPosition(bulletStartPosition);
				transform->SetScale(2, isRight ? 2 : -2);
				transform->SetRotation(mainAngle);
				auto particleSprite = particle->AddComponent<SpriteManagerComponent>(shotgun->gunsmoke);
				particleSprite->PlaySprite("Gunsmoke3_");
				shotgun->owner->AddObject(ObjectType::Effect, particle);
			}
			for (int i = 0; i < 8; i++)
			{
				auto angle = atan2(dir.y, dir.x) + Engine::GetInstance().GetRandom()->get(-PI / 8, PI / 8);

				auto bullet = std::make_shared<Bullet>(shotgun->owner, ObjectType::EBullet);
				bullet->transform->SetPosition(bulletStartPosition);
				bullet->transform->SetScale(1.f, 1.f);
				bullet->transform->SetRotation(angle);
				bullet->SetVelocity(glm::vec2{cos(angle), sin(angle)} * 2000.f);
				shotgun->owner->AddObject(ObjectType::EBullet, bullet);
			}
		}
		
	}
}

void Shotgun::ShotgunStateAttack::TestForExit(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	auto transform = shotgun->GetComponent<TransformComponent>();
	auto playerTransform = shotgun->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	if (glm::length(dir) > 1000) {
		auto shotgun = dynamic_cast<Shotgun*>(obj);
		shotgun->ChangeState(&shotgun->shotgunStateChase);
	}
	else if (transform->GetScale().x * dir.x < 0.)
	{
		//std::cout << "turn" << std::endl;
		shotgun->ChangeState(&shotgun->shotgunStateTurn);
	}
}

void Shotgun::ShotgunStateTurn::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Shotgun_Turn_");
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	shotgun->velocity = { 0 ,0 };
	shotgun->GetComponent<TransformComponent>()->ScaleBy(-1, 1);

}

void Shotgun::ShotgunStateTurn::Update(Object* obj, double dt)
{
}

void Shotgun::ShotgunStateTurn::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		auto shotgun = dynamic_cast<Shotgun*>(obj);
		shotgun->wallStop = false;
		auto transform = shotgun->GetComponent<TransformComponent>();
		transform->Translate(transform->GetScale().x < 0 ? -1 : 1, 0);
		switch (shotgun->myMark->currentMark)
		{
		case EnemyMark::MarkType::Chase:
		{
			shotgun->ChangeState(&shotgun->shotgunStateChase);
		} break;
		case EnemyMark::MarkType::None:
		{
			shotgun->ChangeState(&shotgun->shotgunStateWalk);
		} break;
		case EnemyMark::MarkType::Alert:
		{
			shotgun->ChangeState(&shotgun->shotgunStateAlert);
		} break;
		}
	}
}

void Shotgun::ShotgunStateHurtFly::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Shotgun_Hurt_Fly_");
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	shotgun->currentGround = nullptr;
	shotgun->myMark->currentMark = EnemyMark::MarkType::None;
	timer = 0.2;
	Engine::GetInstance().GetAudioManager()->StartSound("enemy_blood_squirt_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
	Engine::GetInstance().GetAudioManager()->StartSound("enemy_bloodsplat" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));

}

void Shotgun::ShotgunStateHurtFly::Update(Object* obj, double dt)
{
	timer += dt;
	if (timer > 0.1)
	{
		timer = 0.;
		auto shotgun = dynamic_cast<Shotgun*>(obj);
		for (int i = 0; i < 3; i++)
		{
			auto blood = std::make_shared<BloodEffect>(shotgun->owner, ObjectType::Blood3);
			blood->SetIsGettingDisappearByTime(true);
			int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
			blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
			glm::vec2 dir = -glm::normalize(shotgun->velocity);
			blood->SetVelocity(glm::rotate(dir * 200.f, (float)(-PI / 6.f + PI / 6.f * i)));
			auto bloodTransform = blood->GetComponent<TransformComponent>();
			bloodTransform->SetRotation(atan2(blood->GetVelocity().y, blood->GetVelocity().x));
			bloodTransform->SetScale(1.f, 1.f);
			bloodTransform->SetPosition(shotgun->GetComponent<TransformComponent>()->GetPosition());
			shotgun->owner->AddObject(ObjectType::Blood3, blood);
		}

		auto blood = std::make_shared<BloodEffect>(shotgun->owner, ObjectType::BackgroundEffect);
		int textureNumber = abs(shotgun->velocity.y) < 100. ? Engine::GetInstance().GetRandom()->get(0, 1) : Engine::GetInstance().GetRandom()->get(2, 4);
		auto texture = blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
		auto AABB = blood->AddComponent<AABBCollisionComponent>();
		AABB->SetSize(texture->GetSize() / 2.f);
		glm::vec2 dir = -glm::normalize(shotgun->velocity);
		auto bloodTransform = blood->GetComponent<TransformComponent>();
		bloodTransform->SetRotation(atan2(dir.y, dir.x));
		bloodTransform->SetScale(2.f, 2.f);
		bloodTransform->SetPosition(shotgun->GetComponent<TransformComponent>()->GetPosition());
		RenderInfo info{};
		info.color = { 0.5, 0.5, 0.5, 0.9 };
		blood->SetRenderInfo(info);
		shotgun->owner->AddObject(ObjectType::BackgroundEffect, blood);
	}
}

void Shotgun::ShotgunStateHurtFly::TestForExit(Object* obj)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	if (shotgun->currentGround)
	{
		shotgun->ChangeState(&shotgun->shotgunStateHurtGround);
	}
}

void Shotgun::ShotgunStateHurtGround::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Shotgun_Hurt_Ground_");
	bloodSwitch = false;
}

void Shotgun::ShotgunStateHurtGround::Update(Object* obj, double dt)
{
	auto shotgun = dynamic_cast<Shotgun*>(obj);
	shotgun->velocity.x *= 0.9;
}

void Shotgun::ShotgunStateHurtGround::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!bloodSwitch && spriteManager->IsDone())
	{
		bloodSwitch = true;
		auto shotgun = dynamic_cast<Shotgun*>(obj);
		shotgun->velocity.x = 0;

		auto shotgunTrnasform = shotgun->GetComponent<TransformComponent>();
		for (int i = 0; i < 3; i++)
		{
			auto blood = std::make_shared<BloodEffect>(shotgun->owner, ObjectType::Blood3);
			blood->SetIsGettingDisappearByTime(true);
			int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
			blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
			float angle = shotgunTrnasform->GetScale().x < 0 ? PI / 3.f : 2 * PI / 3.f;
			glm::vec2 dir = { cos(angle), sin(angle) };
			blood->SetVelocity(glm::rotate(dir * 200.f, (float)(-PI / 6.f + PI / 3.f * i)));
			auto bloodTransform = blood->GetComponent<TransformComponent>();
			bloodTransform->SetRotation(atan2(blood->GetVelocity().y, blood->GetVelocity().x));
			bloodTransform->SetScale(1.f, 1.f);
			bloodTransform->SetPosition(shotgunTrnasform->GetPosition());
			shotgun->owner->AddObject(ObjectType::Blood3, blood);
		}
	}
}
