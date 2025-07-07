#include "pch.h"
#include "Gangster.h"

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
constexpr float GANGSTER_SCALE = 2.;

Gangster::Gangster(ObjectManager* owner, ObjectType type) : Enemy(owner, type)
{
	auto transform = AddComponent<TransformComponent>();
	transform->SetPosition(300, 200);
	transform->SetScale(-GANGSTER_SCALE, GANGSTER_SCALE);

	auto collision = AddComponent<AABBCollisionComponent>();
	collision->SetSize({ 50, 80 });

	auto sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_gangster/GangsterSpriteData.txt");

	gunPart = std::make_shared<Object>(owner, ObjectType::EnemyArm);
	gunPart->AddComponent<TransformComponent>();
	gunPartTexture = gunPart->AddComponent<TextureComponent>("gunArmTexture", "Resource/Texture/Object/obj_gangster/Arm/0.png");
	owner->AddObject(ObjectType::EnemyArm, gunPart);

	gunsmoke = "Resource/Texture/Effect/GunFire/gunSmokeData.txt";
	gunspark = "Resource/Texture/Effect/GunFire/gunSparkData.txt";
	
	initState = &gangsterStateIdle;
	ChangeState(initState);

	stateIdle = &gangsterStateIdle;
	stateWalk = &gangsterStateWalk;
	stateAlert = &gangsterStateAlert;
	stateSuspicious = &gangsterStateSuspicious;
	stateChase = &gangsterStateChase;
	stateAttack = &gangsterStateAttack;
	stateTurn = &gangsterStateTurn;
	stateHurtfly = &gangsterStateHurtFly;
	stateHurtground = &gangsterStateHurtGround;
}

void Gangster::Update(double dt)
{
	if (owner->GetFrameIndex() == 0)
	{
		willDie = false;
		hit = false;
		if(currState != initState) ChangeState(initState);
	}
	Enemy::Update(dt);
	if (!willDie && hit)
	{
		willDie = true;
		ChangeState(&gangsterStateHurtFly);
	}

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

	auto spriteManager = GetComponent<SpriteManagerComponent>();
	if (spriteManager->GetName() == "Gangster_Run_")
	{
		int footstep = spriteManager->GetCurrentSprite()->GetCurrentFrame();
		if (footStepSwitch && footstep != 1 && footstep != 6)
		{
			footStepSwitch = false;
		}
		if (!footStepSwitch && (footstep == 1 || footstep == 6))
		{
			footStepSwitch = true;
			Engine::GetInstance().GetAudioManager()->StartSound("generic_enemy_run_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));
		}
	}
	else if (spriteManager->GetName() == "Gangster_Walk_" && glm::length(GetComponent<TransformComponent>()->GetPosition() - owner->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition()) < 400)
	{
		int footstep = spriteManager->GetCurrentSprite()->GetCurrentFrame();
		if (footStepSwitch && footstep != 1 && footstep != 5)
		{
			footStepSwitch = false;
		}
		if (!footStepSwitch && (footstep == 1 || footstep == 5))
		{
			footStepSwitch = true;
			Engine::GetInstance().GetAudioManager()->StartSound("generic_enemy_walk_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));
		}
	}
}

void Gangster::Render()
{
	Enemy::Render();
}


void Gangster::GangsterStateIdle::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Gangster_Idle_");
	auto gangster = dynamic_cast<Gangster*>(obj);
	gangster->velocity = { 0 ,0 };
}

void Gangster::GangsterStateIdle::Update(Object* obj, double dt) {}

void Gangster::GangsterStateIdle::TestForExit(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);

	if (gangster->CanSeePlayer())
	{
		gangster->myMark->currentMark = EnemyMark::MarkType::Chase;
		gangster->ChangeState(&gangster->gangsterStateChase);
	}
}

void Gangster::GangsterStateWalk::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Gangster_Walk_");
	dir = obj->GetComponent<TransformComponent>()->GetScale().x < 0 ? -1 : 1;
}

void Gangster::GangsterStateWalk::Update(Object* obj, double dt)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	auto gangster = dynamic_cast<Gangster*>(obj);
	if (spriteManager->GetName() == "Gangster_Walk_")
	{
		gangster->velocity.x = dir * RUN_SPEED / 4.f;
	}
	if (spriteManager->IsDone())
	{
		spriteManager->PlaySprite("Gangster_Walk_");
	}
}

void Gangster::GangsterStateWalk::TestForExit(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	if (gangster->wallStop)
	{
		gangster->ChangeState(&gangster->gangsterStateTurn);
	}
	if (gangster->CanSeePlayer())
	{
		gangster->myMark->currentMark = EnemyMark::MarkType::Chase;
		gangster->ChangeState(&gangster->gangsterStateChase);
	}
}

void Gangster::GangsterStateAlert::Enter(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	gangster->myMark->currentMark = EnemyMark::MarkType::Alert;
	gangster->myMark->marks->PlaySprite("Enemy_Alert_");
	auto transform = gangster->GetComponent<TransformComponent>();
	auto dir = gangster->alertedPosition - transform->GetPosition();

}

void Gangster::GangsterStateAlert::Update(Object* obj, double dt)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	auto transform = gangster->GetComponent<TransformComponent>();
	auto dir = gangster->alertedPosition - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!gangster->wallStop)
	{

		if (spriteManager->GetName() == "Gangster_Run_")
		{
			gangster->velocity.x = dir.x > 0 ? RUN_SPEED : -RUN_SPEED;
		}
		if (spriteManager->IsDone())
		{
			spriteManager->PlaySprite("Gangster_Run_");
		}

	}
}

void Gangster::GangsterStateAlert::TestForExit(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	auto transform = gangster->GetComponent<TransformComponent>();
	auto dir = gangster->alertedPosition - transform->GetPosition();
	if (transform->GetScale().x * dir.x < 0.)
	{
		gangster->ChangeState(&gangster->gangsterStateTurn);

	}
	if (glm::length(dir) < 200)
	{
		gangster->myMark->currentMark = EnemyMark::MarkType::Suspicious;
		gangster->ChangeState(&gangster->gangsterStateSuspicious);
	}
	if (gangster->CanSeePlayer())
	{
		gangster->myMark->currentMark = EnemyMark::MarkType::Chase;
		gangster->ChangeState(&gangster->gangsterStateChase);
	}
}

void Gangster::GangsterStateSuspicious::Enter(Object* obj)
{
	timer = 0.f;
	auto gangster = dynamic_cast<Gangster*>(obj);
	gangster->velocity = { 0, 0 };
	gangster->myMark->marks->PlaySprite("Enemy_Suspicious_");
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Gangster_Idle_");
}

void Gangster::GangsterStateSuspicious::Update(Object* obj, double dt)
{
	timer += dt;
}

void Gangster::GangsterStateSuspicious::TestForExit(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	if (timer > 4.f)
	{
		gangster->myMark->currentMark = EnemyMark::MarkType::None;
		gangster->ChangeState(gangster->initState);
	}
	if (gangster->CanSeePlayer())
	{
		gangster->myMark->currentMark = EnemyMark::MarkType::Chase;
		gangster->ChangeState(&gangster->gangsterStateChase);
	}
}

void Gangster::GangsterStateChase::Enter(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	if (!chaseFirst)
	{
		chaseFirst = true;
		gangster->myMark->marks->PlaySprite("Enemy_Chase_");
		auto warning = std::make_shared<Warning>(gangster->owner, ObjectType::Effect, 300.f, true);
		warning->GetComponent<TransformComponent>()->SetPosition(gangster->GetComponent<TransformComponent>()->GetPosition());
		gangster->owner->AddObject(ObjectType::Effect, warning);
	}
}

void Gangster::GangsterStateChase::Update(Object* obj, double dt) {
	auto gangster = dynamic_cast<Gangster*>(obj);
	auto transform = gangster->GetComponent<TransformComponent>();
	auto playerTransform = gangster->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!gangster->wallStop)
	{
		if (glm::length(dir) > 1000.)
		{
			if (spriteManager->GetName() == "Gangster_Run_")
			{
				gangster->velocity.x = dir.x > 0 ? RUN_SPEED : -RUN_SPEED;
			}
			if (spriteManager->IsDone())
			{
				spriteManager->PlaySprite("Gangster_Run_");
			}
		}
	}

}

void Gangster::GangsterStateChase::TestForExit(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	auto transform = gangster->GetComponent<TransformComponent>();
	auto playerTransform = gangster->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();

	if (glm::length(dir) < 1000)
	{
		//std::cout << glm::length(dir) << std::endl;
		gangster->ChangeState(&gangster->gangsterStateAttack);
	}

	else if (transform->GetScale().x * dir.x < 0.)
	{
		//std::cout << "turn" << std::endl;
		gangster->ChangeState(&gangster->gangsterStateTurn);
	}
	else if (abs(dir.x) <= 20 || gangster->wallStop)
	{
		gangster->velocity.x = 0.;
		if (spriteManager->GetName() == "Gangster_Run_" || spriteManager->IsDone()) spriteManager->PlaySprite("Gangster_Idle_");
	}
}

void Gangster::GangsterStateAttack::Enter(Object* obj)
{
	Engine::GetInstance().GetAudioManager()->StartSound("sound_gun_fire_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));

	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Gangster_Aim_");
	auto gangster = dynamic_cast<Gangster*>(obj);
	gangster->velocity = { 0 ,0 };
	timer = 0.;
}

void Gangster::GangsterStateAttack::Update(Object* obj, double dt)
{
	timer += dt;
	if (timer > 1.0)
	{
		auto gangster = dynamic_cast<Gangster*>(obj);
		auto transform = gangster->GetComponent<TransformComponent>();
		auto playerTransform = gangster->p->GetComponent<TransformComponent>();
		auto dir = glm::normalize(playerTransform->GetPosition() - transform->GetPosition());
		
		float angle = atan2(dir.y, dir.x);
		bool isRight = transform->GetScale().x > 0;
		auto armOffset = glm::vec2{ (isRight ? -1 : 1) * gangster->armPosOffset.x, gangster->armPosOffset.y };
		auto gunOffset = glm::vec2(98, isRight ? -4 : 4);
		glm::vec2 rotateOffset = glm::rotate(gunOffset, angle);
		auto bulletStartPosition = gangster->GetComponent<TransformComponent>()->GetPosition() + armOffset + rotateOffset;
		{
			auto particle = std::make_shared<Particle>(gangster->owner, ObjectType::Effect);
			particle->SetIsDeadBySpriteEnd(true);
			auto transform = particle->GetComponent<TransformComponent>();
			transform->SetPosition(bulletStartPosition);
			transform->SetScale(2, isRight ? 2 : -2);
			transform->SetRotation(angle);
			auto particleSprite = particle->AddComponent<SpriteManagerComponent>(gangster->gunspark);
			particleSprite->PlaySprite("Gunspark_");
			gangster->owner->AddObject(ObjectType::Effect, particle);
		}
		{
			auto particle = std::make_shared<Particle>(gangster->owner, ObjectType::Effect);
			particle->SetIsDeadBySpriteEnd(true);
			auto transform = particle->GetComponent<TransformComponent>();
			transform->SetPosition(bulletStartPosition);
			transform->SetScale(2, isRight ? 2 : -2);
			transform->SetRotation(angle);
			auto particleSprite = particle->AddComponent<SpriteManagerComponent>(gangster->gunsmoke);
			particleSprite->PlaySprite("Gunsmoke_");
			gangster->owner->AddObject(ObjectType::Effect, particle);
		}

		auto bullet = std::make_shared<Bullet>(gangster->owner, ObjectType::EBullet);
		bullet->transform->SetPosition(bulletStartPosition);
		bullet->transform->SetScale(1.5f, 1.5f);
		bullet->transform->SetRotation(angle);
		bullet->SetVelocity(dir * 2000.f);
		gangster->owner->AddObject(ObjectType::EBullet, bullet);
		timer = 0.;
	}
}

void Gangster::GangsterStateAttack::TestForExit(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	auto transform = gangster->GetComponent<TransformComponent>();
	auto playerTransform = gangster->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	if(glm::length(dir) > 1000){
		auto gangster = dynamic_cast<Gangster*>(obj);
		gangster->ChangeState(&gangster->gangsterStateChase);
	}
	else if (transform->GetScale().x * dir.x < 0.)
	{
		//std::cout << "turn" << std::endl;
		gangster->ChangeState(&gangster->gangsterStateTurn);
	}
}

void Gangster::GangsterStateTurn::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Gangster_Turn_");
	auto gangster = dynamic_cast<Gangster*>(obj);
	gangster->velocity = { 0 ,0 };
	gangster->GetComponent<TransformComponent>()->ScaleBy(-1, 1);

}

void Gangster::GangsterStateTurn::Update(Object* obj, double dt)
{
}

void Gangster::GangsterStateTurn::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		auto gangster = dynamic_cast<Gangster*>(obj);
		gangster->wallStop = false;
		auto transform = gangster->GetComponent<TransformComponent>();
		transform->Translate(transform->GetScale().x < 0 ? -1 : 1, 0);
		switch (gangster->myMark->currentMark)
		{
		case EnemyMark::MarkType::Chase:
		{
			gangster->ChangeState(&gangster->gangsterStateChase);
		} break;
		case EnemyMark::MarkType::None:
		{
			gangster->ChangeState(&gangster->gangsterStateWalk);
		} break;
		case EnemyMark::MarkType::Alert:
		{
			gangster->ChangeState(&gangster->gangsterStateAlert);
		} break;
		}
	}
}

void Gangster::GangsterStateHurtFly::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Gangster_Hurt_Fly_");
	auto gangster = dynamic_cast<Gangster*>(obj);
	gangster->currentGround = nullptr;
	gangster->myMark->currentMark = EnemyMark::MarkType::None;
	timer = 0.2;
	Engine::GetInstance().GetAudioManager()->StartSound("enemy_blood_squirt_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
	Engine::GetInstance().GetAudioManager()->StartSound("enemy_bloodsplat" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));

}

void Gangster::GangsterStateHurtFly::Update(Object* obj, double dt)
{
	timer += dt;
	if (timer > 0.1)
	{
		timer = 0.;
		auto gangster = dynamic_cast<Gangster*>(obj);
		for (int i = 0; i < 3; i++)
		{
			auto blood = std::make_shared<BloodEffect>(gangster->owner, ObjectType::Blood3);
			blood->SetIsGettingDisappearByTime(true);
			int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
			blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
			glm::vec2 dir = -glm::normalize(gangster->velocity);
			blood->SetVelocity(glm::rotate(dir * 200.f, (float)(-PI / 6.f + PI / 6.f * i)));
			auto bloodTransform = blood->GetComponent<TransformComponent>();
			bloodTransform->SetRotation(atan2(blood->GetVelocity().y, blood->GetVelocity().x));
			bloodTransform->SetScale(1.f, 1.f);
			bloodTransform->SetPosition(gangster->GetComponent<TransformComponent>()->GetPosition());
			gangster->owner->AddObject(ObjectType::Blood3, blood);
		}

		auto blood = std::make_shared<BloodEffect>(gangster->owner, ObjectType::BackgroundEffect);
		int textureNumber = abs(gangster->velocity.y) < 100. ? Engine::GetInstance().GetRandom()->get(0, 1) : Engine::GetInstance().GetRandom()->get(2, 4);
		auto texture = blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
		auto AABB = blood->AddComponent<AABBCollisionComponent>();
		AABB->SetSize(texture->GetSize() / 2.f);
		glm::vec2 dir = -glm::normalize(gangster->velocity);
		auto bloodTransform = blood->GetComponent<TransformComponent>();
		bloodTransform->SetRotation(atan2(dir.y, dir.x));
		bloodTransform->SetScale(2.f, 2.f);
		bloodTransform->SetPosition(gangster->GetComponent<TransformComponent>()->GetPosition());
		RenderInfo info{};
		info.color = { 0.5, 0.5, 0.5, 0.9 };
		blood->SetRenderInfo(info);
		gangster->owner->AddObject(ObjectType::BackgroundEffect, blood);
	}
}

void Gangster::GangsterStateHurtFly::TestForExit(Object* obj)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	if (gangster->currentGround)
	{
		gangster->ChangeState(&gangster->gangsterStateHurtGround);
	}
}

void Gangster::GangsterStateHurtGround::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Gangster_Hurt_Ground_");
	bloodSwitch = false;
}

void Gangster::GangsterStateHurtGround::Update(Object* obj, double dt)
{
	auto gangster = dynamic_cast<Gangster*>(obj);
	gangster->velocity.x *= 0.9;
}

void Gangster::GangsterStateHurtGround::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!bloodSwitch && spriteManager->IsDone())
	{
		bloodSwitch = true;
		auto gangster = dynamic_cast<Gangster*>(obj);
		gangster->velocity.x = 0;

		auto gangsterTrnasform = gangster->GetComponent<TransformComponent>();
		for (int i = 0; i < 3; i++)
		{
			auto blood = std::make_shared<BloodEffect>(gangster->owner, ObjectType::Blood3);
			blood->SetIsGettingDisappearByTime(true);
			int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
			blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
			float angle = gangsterTrnasform->GetScale().x < 0 ? PI / 3.f : 2 * PI / 3.f;
			glm::vec2 dir = { cos(angle), sin(angle) };
			blood->SetVelocity(glm::rotate(dir * 200.f, (float)(-PI / 6.f + PI / 3.f * i)));
			auto bloodTransform = blood->GetComponent<TransformComponent>();
			bloodTransform->SetRotation(atan2(blood->GetVelocity().y, blood->GetVelocity().x));
			bloodTransform->SetScale(1.f, 1.f);
			bloodTransform->SetPosition(gangsterTrnasform->GetPosition());
			gangster->owner->AddObject(ObjectType::Blood3, blood);
		}
	}
}
