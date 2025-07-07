#include "pch.h"
#include "Grunt.h"

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
#include "EnemyMark.h"
#include "Melee.h"
constexpr float RUN_SPEED = 400.;
constexpr float GRUNT_SCALE = 2.;

Grunt::Grunt(ObjectManager* owner, ObjectType type) : Enemy(owner, type)
{
	auto transform = AddComponent<TransformComponent>();
	transform->SetPosition(300, 200);
	transform->SetScale(-GRUNT_SCALE, GRUNT_SCALE);

	auto collision = AddComponent<AABBCollisionComponent>();
	collision->SetSize({ 50, 74 });

	auto sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_grunt/gruntSpriteData.txt");
	initState = &gruntStateIdle;
	ChangeState(initState);

	stateIdle = &gruntStateIdle;
	stateWalk = &gruntStateWalk;
	stateAlert = &gruntStateAlert;
	stateSuspicious = &gruntStateSuspicious;
	stateChase = &gruntStateChase;
	stateAttack = &gruntStateAttack;
	stateTurn = &gruntStateTurn;
	stateHurtfly = &gruntStateHurtFly;
	stateHurtground = &gruntStateHurtGround;
}

void Grunt::Update(double dt)
{
	if (owner->GetFrameIndex() == 0)
	{
		willDie = false;
		hit = false;
		if (currState != initState) ChangeState(initState);
	}
	Enemy::Update(dt);
	if (!willDie && hit)
	{
		willDie = true;
		ChangeState(&gruntStateHurtFly);
	}

	auto spriteManager = GetComponent<SpriteManagerComponent>();
	if (spriteManager->GetName() == "Grunt_Run_")
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
	else if (spriteManager->GetName() == "Grunt_Walk_" && glm::length(GetComponent<TransformComponent>()->GetPosition() - owner->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition()) < 400)
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

void Grunt::GruntStateIdle::Enter(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Grunt_Idle_");
	auto grunt = dynamic_cast<Grunt*>(obj);
	grunt->velocity = { 0 ,0 };
}

void Grunt::GruntStateIdle::Update(Object* obj, double dt) {}

void Grunt::GruntStateIdle::TestForExit(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);

	if (grunt->CanSeePlayer())
	{
		grunt->myMark->currentMark = EnemyMark::MarkType::Chase;
		grunt->ChangeState(&grunt->gruntStateChase);
	}
}

void Grunt::GruntStateWalk::Enter(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Grunt_Walk_");
	dir = obj->GetComponent<TransformComponent>()->GetScale().x < 0 ? -1 : 1;
}

void Grunt::GruntStateWalk::Update(Object* obj, double dt) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	auto grunt = dynamic_cast<Grunt*>(obj);
	if (spriteManager->GetName() == "Grunt_Walk_")
	{
		grunt->velocity.x = dir * RUN_SPEED / 4.f;
	}
	if (spriteManager->IsDone())
	{
		spriteManager->PlaySprite("Grunt_Walk_");
	}
}

void Grunt::GruntStateWalk::TestForExit(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	if (grunt->wallStop)
	{
		grunt->ChangeState(&grunt->gruntStateTurn);
	}
	if (grunt->CanSeePlayer())
	{
		grunt->myMark->currentMark = EnemyMark::MarkType::Chase;
		grunt->ChangeState(&grunt->gruntStateChase);
	}
}

void Grunt::GruntStateAlert::Enter(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	grunt->myMark->currentMark = EnemyMark::MarkType::Alert;
	grunt->myMark->marks->PlaySprite("Enemy_Alert_");
	auto transform = grunt->GetComponent<TransformComponent>();
	auto dir = grunt->alertedPosition - transform->GetPosition();
}

void Grunt::GruntStateAlert::Update(Object* obj, double dt) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	auto transform = grunt->GetComponent<TransformComponent>();
	auto dir = grunt->alertedPosition - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!grunt->wallStop)
	{

		if (spriteManager->GetName() == "Grunt_Run_")
		{
			grunt->velocity.x = dir.x > 0 ? RUN_SPEED : -RUN_SPEED;
		}
		if (spriteManager->IsDone())
		{
			spriteManager->PlaySprite("Grunt_Run_");
		}
		
	}
}

void Grunt::GruntStateAlert::TestForExit(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	auto transform = grunt->GetComponent<TransformComponent>();
	auto dir = grunt->alertedPosition - transform->GetPosition();
	if (transform->GetScale().x * dir.x < 0.)
	{
		grunt->ChangeState(&grunt->gruntStateTurn);

	}
	if (glm::length(dir) < 200)
	{
		grunt->myMark->currentMark = EnemyMark::MarkType::Suspicious;
		grunt->ChangeState(&grunt->gruntStateSuspicious);
	}
	if (grunt->CanSeePlayer())
	{
		grunt->myMark->currentMark = EnemyMark::MarkType::Chase;
		grunt->ChangeState(&grunt->gruntStateChase);
	}
}

void Grunt::GruntStateSuspicious::Enter(Object* obj) 
{
	timer = 0.f;
	auto grunt = dynamic_cast<Grunt*>(obj);
	grunt->velocity = { 0, 0 };
	grunt->myMark->marks->PlaySprite("Enemy_Suspicious_");
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Grunt_Idle_");
}

void Grunt::GruntStateSuspicious::Update(Object* obj, double dt) 
{
	timer += dt;
}

void Grunt::GruntStateSuspicious::TestForExit(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	if (timer > 4.f)
	{
		grunt->myMark->currentMark = EnemyMark::MarkType::None;
		grunt->ChangeState(grunt->initState);
	}
	if (grunt->CanSeePlayer())
	{
		grunt->myMark->currentMark = EnemyMark::MarkType::Chase;
		grunt->ChangeState(&grunt->gruntStateChase);
	}
}

void Grunt::GruntStateChase::Enter(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	if (!chaseFirst)
	{
		chaseFirst = true;
		grunt->myMark->marks->PlaySprite("Enemy_Chase_");
		auto warning = std::make_shared<Warning>(grunt->owner, ObjectType::Effect, 300.f, true);
		warning->GetComponent<TransformComponent>()->SetPosition(grunt->GetComponent<TransformComponent>()->GetPosition());
		grunt->owner->AddObject(ObjectType::Effect, warning);
	}
	attackCool = 0.;
}

void Grunt::GruntStateChase::Update(Object* obj, double dt) {
	auto grunt = dynamic_cast<Grunt*>(obj);
	auto transform = grunt->GetComponent<TransformComponent>();
	auto playerTransform = grunt->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!grunt->wallStop)
	{
		if (abs(dir.x) > 20)
		{
			if (spriteManager->GetName() == "Grunt_Run_")
			{
				grunt->velocity.x = dir.x > 0 ? RUN_SPEED : -RUN_SPEED;
			}
			if (spriteManager->IsDone())
			{
				spriteManager->PlaySprite("Grunt_Run_");
			}
		}
	}

}

void Grunt::GruntStateChase::TestForExit(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	auto transform = grunt->GetComponent<TransformComponent>();
	auto playerTransform = grunt->p->GetComponent<TransformComponent>();
	auto dir = playerTransform->GetPosition() - transform->GetPosition();
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (glm::length(dir) < 100)
	{
		grunt->ChangeState(&grunt->gruntStateAttack);
	}
	else if (abs(dir.x) > 20 && transform->GetScale().x * dir.x < 0.)
	{
		//std::cout << "turn" << std::endl;
		grunt->ChangeState(&grunt->gruntStateTurn);
	}
	else if (abs(dir.x) <= 20 || grunt->wallStop)
	{
		grunt->velocity.x = 0.;
		if (spriteManager->GetName() == "Grunt_Run_" || spriteManager->IsDone()) spriteManager->PlaySprite("Grunt_Idle_");
	}
}

void Grunt::GruntStateAttack::Enter(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	grunt->velocity = { 0 ,0 };
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Grunt_Attack_");
	attack = false;
}

void Grunt::GruntStateAttack::Update(Object* obj, double dt) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if(!attack && spriteManager->GetName() == "Grunt_Attack_" && spriteManager->GetCurrentSprite()->GetCurrentFrame() == 3)
	{
		attack = true;
		auto transform = grunt->GetComponent<TransformComponent>();
		auto dir = glm::normalize(grunt->owner->GetFrontObject(ObjectType::Player)->GetComponent<TransformComponent>()->GetPosition() - transform->GetPosition());
		auto attackEffect = std::make_shared<Melee>(grunt->owner, ObjectType::EBullet);
		attackEffect->SetAttackerTransform(transform);
		auto attackEffectTransform = attackEffect->GetComponent<TransformComponent>();
		attackEffectTransform->SetPosition(transform->GetPosition() + dir * 50.f);
		attackEffectTransform->SetRotation(atan2(dir.y, dir.x));
		attackEffectTransform->SetScale(1.2, dir.x < 0. ? -2.2 : 2.2);
		auto attackEffectSprite = attackEffect->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_gruntslash/gruntSlashSpt.txt");
		attackEffectSprite->PlaySprite("Grunt_Slash_");
		grunt->owner->AddObject(ObjectType::EBullet, attackEffect);
		auto attackEffectAABB = attackEffect->GetComponent<AABBCollisionComponent>();
		attackEffectAABB->SetSize({ 70, 100 });
		Engine::GetInstance().GetAudioManager()->StartSound("enemy_punch");
	}
	
}

void Grunt::GruntStateAttack::TestForExit(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		auto grunt = dynamic_cast<Grunt*>(obj);
		grunt->ChangeState(&grunt->gruntStateChase);
	}
}

void Grunt::GruntStateTurn::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Grunt_Turn_");
	auto grunt = dynamic_cast<Grunt*>(obj);
	grunt->velocity = { 0 ,0 };
	grunt->GetComponent<TransformComponent>()->ScaleBy(-1, 1);

}

void Grunt::GruntStateTurn::Update(Object* obj, double dt)
{
}

void Grunt::GruntStateTurn::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		auto grunt = dynamic_cast<Grunt*>(obj);
		grunt->wallStop = false;
		auto transform = grunt->GetComponent<TransformComponent>();
		transform->Translate(transform->GetScale().x < 0 ? -1 : 1, 0);
		switch (grunt->myMark->currentMark)
		{
		case EnemyMark::MarkType::Chase:
		{
			grunt->ChangeState(&grunt->gruntStateChase);
		} break;
		case EnemyMark::MarkType::None:
		{
			grunt->ChangeState(&grunt->gruntStateWalk);
		} break;
		case EnemyMark::MarkType::Alert:
		{
			grunt->ChangeState(&grunt->gruntStateAlert);
		} break;
		}
	}
}

void Grunt::GruntStateHurtFly::Enter(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Grunt_Hurt_Fly_");
	auto grunt = dynamic_cast<Grunt*>(obj);
	grunt->currentGround = nullptr;
	grunt->myMark->currentMark = EnemyMark::MarkType::None;
	timer = 0.2;
	Engine::GetInstance().GetAudioManager()->StartSound("enemy_blood_squirt_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
	Engine::GetInstance().GetAudioManager()->StartSound("enemy_bloodsplat" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));
	Engine::GetInstance().GetAudioManager()->StartSound("enemy_death_sword_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));

}

void Grunt::GruntStateHurtFly::Update(Object* obj, double dt) 
{
	timer += dt;
	if (timer > 0.1)
	{
		timer = 0.;
		auto grunt = dynamic_cast<Grunt*>(obj);
		for (int i = 0; i < 3; i++)
		{
			auto blood = std::make_shared<BloodEffect>(grunt->owner, ObjectType::Blood3);
			blood->SetIsGettingDisappearByTime(true);
			int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
			blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
			glm::vec2 dir = -glm::normalize(grunt->velocity);
			blood->SetVelocity(glm::rotate(dir * 200.f, (float)(-PI / 6.f + PI / 6.f*i)));
			auto bloodTransform = blood->GetComponent<TransformComponent>();
			bloodTransform->SetRotation(atan2(blood->GetVelocity().y, blood->GetVelocity().x));
			bloodTransform->SetScale(1.f, 1.f);
			bloodTransform->SetPosition(grunt->GetComponent<TransformComponent>()->GetPosition());
			grunt->owner->AddObject(ObjectType::Blood3, blood);
		}

		auto blood = std::make_shared<BloodEffect>(grunt->owner, ObjectType::BackgroundEffect);
		int textureNumber = abs(grunt->velocity.y) < 100. ? Engine::GetInstance().GetRandom()->get(0, 1) : Engine::GetInstance().GetRandom()->get(2, 4);
		auto texture = blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
		auto AABB = blood->AddComponent<AABBCollisionComponent>();
		AABB->SetSize(texture->GetSize() / 2.f);
		glm::vec2 dir = -glm::normalize(grunt->velocity);
		auto bloodTransform = blood->GetComponent<TransformComponent>();
		bloodTransform->SetRotation(atan2(dir.y, dir.x));
		bloodTransform->SetScale(2.f, 2.f);
		bloodTransform->SetPosition(grunt->GetComponent<TransformComponent>()->GetPosition());
		RenderInfo info{};
		info.color = { 0.5, 0.5, 0.5, 0.9 };
		blood->SetRenderInfo(info);
		grunt->owner->AddObject(ObjectType::BackgroundEffect, blood);
	}
}

void Grunt::GruntStateHurtFly::TestForExit(Object* obj) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	if (grunt->currentGround)
	{
		grunt->ChangeState(&grunt->gruntStateHurtGround);
	}
}

void Grunt::GruntStateHurtGround::Enter(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Grunt_Hurt_Ground_");
	bloodSwitch = false;
}

void Grunt::GruntStateHurtGround::Update(Object* obj, double dt) 
{
	auto grunt = dynamic_cast<Grunt*>(obj);
	grunt->velocity.x *= 0.9;
}

void Grunt::GruntStateHurtGround::TestForExit(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!bloodSwitch && spriteManager->IsDone())
	{
		bloodSwitch = true;
		auto grunt = dynamic_cast<Grunt*>(obj);
		grunt->velocity.x = 0;

		auto gruntTrnasform = grunt->GetComponent<TransformComponent>();
		for (int i = 0; i < 3; i++)
		{
			auto blood = std::make_shared<BloodEffect>(grunt->owner, ObjectType::Blood3);
			blood->SetIsGettingDisappearByTime(true);
			int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
			blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
			float angle = gruntTrnasform->GetScale().x < 0 ? PI / 3.f : 2 * PI / 3.f;
			glm::vec2 dir = { cos(angle), sin(angle) };
			blood->SetVelocity(glm::rotate(dir * 200.f, (float)(-PI / 6.f + PI / 3.f * i)));
			auto bloodTransform = blood->GetComponent<TransformComponent>();
			bloodTransform->SetRotation(atan2(blood->GetVelocity().y, blood->GetVelocity().x));
			bloodTransform->SetScale(1.f, 1.f);			
			bloodTransform->SetPosition(gruntTrnasform->GetPosition());
			grunt->owner->AddObject(ObjectType::Blood3, blood);
		}
	}
}
