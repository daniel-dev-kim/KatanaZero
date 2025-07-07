#include "pch.h"
#include "Mutation.h"
#include "TextureComponent.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"

#include "Engine.h"
#include "Random.h"
#include "AudioManager.h"
#include "BossScene.h"
#include "Filter.h"
#include "ObjectManager.h"
#include "Background.h"
#include "Stab.h"
#include "SceneManager.h"
Mutation::Mutation(ObjectManager* owner, ObjectType type, BossScene* bossScene) : Object(owner, type), bossScene(bossScene)
{
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_mutation/obj_mutation.txt");
	sprite->PlaySprite("mutation_mutate");
	
	body = std::make_shared<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_body/body.txt");
	body->PlaySprite("obj_body");
	tentacle = std::make_shared<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_tentacle/obj_tentacle.txt");
	tentacle->PlaySprite("tentacle_idle");
	illguy = std::make_shared<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_ill_guy/illguy.txt");
	illguy->PlaySprite("obj_ill_guy");
	headBehind = std::make_shared<TextureComponent>("boss_head_behind", "Resource/Texture/Object/Boss/obj_mutation/0.png");

	backgroundBehind = std::make_shared<TextureComponent>("BackgroundBehind", "Resource/Texture/Background/bg_boss/spr_psychboss_background_0.png");
	background = std::make_shared<TextureComponent>("Background", "Resource/Texture/Background/bg_boss/spr_psychboss_bg_1_0.png");
	Foreground = std::make_shared<TextureComponent>("Floor", "Resource/Texture/Background/bg_boss/spr_psychboss_fg_0.png");
	floor = std::make_shared<TextureComponent>("Foreground", "Resource/Texture/Background/bg_boss/spr_psychboss_floor_0.png");


	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
	Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_mutate_01");

	bgInfo.uEnableObjectWave = true;
	bgInfo.uObjectWaveWidth = 2.f;
}

void Mutation::Update(double dt)
{
	Object::Update(dt);
	illguy->Update(dt);
	tentacle->Update(dt);
	switch (tentacleState)
	{
	case TentacleState::Idle:
	{

	}break;
	case TentacleState::Stab:
	{
		if (tentacle->IsDone())
		{
			tentacleState = TentacleState::Attack;
			bossScene->stab->StartAttack();
		}
	}break;
	case TentacleState::Attack:
	{

	}break;
	case TentacleState::Stab_End:
	{
		if (tentacle->IsDone())
		{
			tentacleState = TentacleState::Idle;
			tentacle->PlaySprite("tentacle_idle");
		}
	}break;
	}

	switch (mutationState)
	{
	case MutationState::Mutate:
	{
		if (sprite->IsDone())
		{
			sprite->PlaySprite("mutation_mutate_hold");
			mutationState = MutationState::Mutate_Hold;
		}
	}break;
	case MutationState::Mutate_Hold:
	{
		timer += dt;
		if (timer > 0.5)
		{
			mutationState = MutationState::Glitch1;
			timer = 0.f;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_ui_screen_glitch_03");
			Engine::GetInstance().uGlitchEnabled = true;
			Engine::GetInstance().uGlitchSeed = 0.1;
		}
	}break;
	case MutationState::Glitch1:
	{
		timer += dt;
		if (timer > 0.35)
		{
			Engine::GetInstance().uGlitchSeed = 0.3;
		}
		if (timer > 0.5)
		{
			Engine::GetInstance().uGlitchEnabled = false;
			timer = 0.0;
			transform->Translate(0, 35);
			sprite->PlaySprite("mutation_mutate_end");
			mutationState = MutationState::Mutate_End;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_mutate_02");
			auto& targetInfo = dynamic_cast<Background*>(owner->GetFrontObject(ObjectType::Background).get())->info;
			targetInfo.uEnableObjectWave = true;
			targetInfo.uObjectWaveOffset = 0.;
			targetInfo.uObjectWaveWidth = 15.f;

		}
	}break;
	case MutationState::Mutate_End:
	{
		auto& targetInfo = dynamic_cast<Background*>(owner->GetFrontObject(ObjectType::Background).get())->info;

		if (timer > 3.0)
		{
			//배경 일렁거림 끝
			targetInfo.uEnableObjectWave = false;
			mutationState = MutationState::Glitch2;
			timer = 0.f;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_ui_screen_glitch_01");
			Engine::GetInstance().uGlitchEnabled = true;
			Engine::GetInstance().uGlitchSeed = 0.6;
		}
		else
		{
			timer += dt;
			targetInfo.uObjectWaveStrength = sin(timer * 3) * 0.8;
		}
	}break;
	case MutationState::Glitch2:
	{
		timer += dt;
		if (timer > 0.3)
		{
			Engine::GetInstance().uGlitchSeed = 0.6;
		}
		else if (timer > 0.2)
		{
			Engine::GetInstance().uGlitchSeed = 1.5;
		}
		else if (timer > 0.1)
		{
			Engine::GetInstance().uGlitchSeed = 0.9;
		}
		if (timer > 0.4)
		{
			timer = 0.0;
			Engine::GetInstance().uGlitchEnabled = false;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_therapist_mutate_03");
			bossScene->filter->info.color = { 1.0, 0.0, 0.0, 1.0 };
			sprite->PlaySprite("mutation_idle");
			transform->SetPosition(headPos);
			bossScene->ChangeTerrain();
			mutationState = MutationState::Appear;
		}
	}break;
	case MutationState::Appear:
	{
		timer += dt;

		if (timer > 1.0)
		{
			if (!songSwitch)
			{
				songSwitch = true;
				Engine::GetInstance().GetAudioManager()->StartSound("song_monster");
			}
			bossScene->filter->info.color.a -= dt;
		}

		if (timer > 3.0)
		{
			bossScene->filter->SetDead();
			bossScene->ChangeBossSceneState(BossScene::BossSceneState::PlayPhase4);
		}
	}break;
	case MutationState::Idle:
	{
		timer = 0.f;
	} break;
	case MutationState::Hurt:
	{
		timer += dt;
		if (timer > 2.0)
		{
			sprite->PlaySprite("mutation_idle");
			mutationState = MutationState::Idle;
		}
	}break;
	case MutationState::Scream:
	{
		timer += dt;
		if (!scream && timer > 2.0)
		{
			scream = true;
			timer = 0.;
			Engine::GetInstance().GetAudioManager()->StopSound("boss_scream");
			Engine::GetInstance().GetAudioManager()->StopSound("song_monster");
			Engine::GetInstance().uGlitchEnabled = true;
			Engine::GetInstance().uGlitchSeed = 0.6;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_ui_screen_glitch_02");
		}
		if (scream && timer > 0.5)
		{
			Engine::GetInstance().GetSceneManager()->SetActiveScene("EndingScene");
		}
	}break;
	}


}

void Mutation::Render()
{
	static double waver = 0.0;
	switch (mutationState)
	{
	case MutationState::Mutate:
	case MutationState::Mutate_Hold:
	case MutationState::Mutate_End:
	case MutationState::Glitch1:
	case MutationState::Glitch2:
	{
		sprite->Render();
	} break;
	case MutationState::Appear:
	case MutationState::Idle:
	case MutationState::Hurt:
	case MutationState::Scream:
	{
		waver += 0.01;
		bgInfo.uObjectWaveStrength = 0.8;
		bgInfo.uObjectWaveOffset += 0.001;
		glm::vec2 BackgroundBehindPos = { 320, 180 };
		auto BackgroundBehindMat = glm::translate(glm::mat4(1.0), glm::vec3(BackgroundBehindPos, 0.0));
		BackgroundBehindMat = glm::scale(BackgroundBehindMat, glm::vec3(2.0, 2.0, 1.0));
		backgroundBehind->Render(BackgroundBehindMat, bgInfo);
		BackgroundBehindPos = { 960, 180 };
		BackgroundBehindMat = glm::translate(glm::mat4(1.0), glm::vec3(BackgroundBehindPos, 0.0));
		BackgroundBehindMat = glm::scale(BackgroundBehindMat, glm::vec3(2.0, 2.0, 1.0));
		backgroundBehind->Render(BackgroundBehindMat, bgInfo);
		RenderInfo info{};
		if(mutationState == MutationState::Hurt || mutationState == MutationState::Scream) info.color = { 1.0, .0, .0, 1.0 };
		glm::vec2 headBehindPos = { 660, 120 };
		auto headBehindMat = glm::translate(glm::mat4(1.0), glm::vec3(headBehindPos, 0.0));
		headBehindMat = glm::scale(headBehindMat, glm::vec3(2.0, 2.0, 1.0));
		headBehind->Render(headBehindMat, info);

		auto tentacleMat = glm::translate(glm::mat4(1.0), glm::vec3(tentaclePos, 0.0));
		tentacleMat = glm::scale(tentacleMat, glm::vec3(2.0, 3.0, 1.0));
		tentacle->Render(tentacleMat);

		sprite->Render(info);

		glm::vec2 bodyPos = { 740, 330 };
		auto bodyMat = glm::translate(glm::mat4(1.0), glm::vec3(bodyPos, 0.0));
		bodyMat = glm::scale(bodyMat, glm::vec3(2.0, 2.0, 1.0));
		body->Render(bodyMat, info);

		glm::vec2 BackgroundPos = { 640, 455 };
		auto BackgroundMat = glm::translate(glm::mat4(1.0), glm::vec3(BackgroundPos, 0.0));
		BackgroundMat = glm::scale(BackgroundMat, glm::vec3(1.0, 1.0, 1.0));
		background->Render(BackgroundMat);

		glm::vec2 illguyPos = { 975, 412 };
		auto illguyMat = glm::translate(glm::mat4(1.0), glm::vec3(illguyPos, 0.0));
		illguyMat = glm::scale(illguyMat, glm::vec3(2.0, 2.0, 1.0));
		illguy->Render(illguyMat);

		glm::vec2 FloorPos = { 640, 705 };
		auto FloorMat = glm::translate(glm::mat4(1.0), glm::vec3(FloorPos, 0.0));
		FloorMat = glm::scale(FloorMat, glm::vec3(1.0, 1.0, 1.0));
		floor->Render(FloorMat);

		glm::vec2 ForegroundPos = { 640, 645 };
		auto ForegroundMat = glm::translate(glm::mat4(1.0), glm::vec3(ForegroundPos, 0.0));
		ForegroundMat = glm::scale(ForegroundMat, glm::vec3(1.0, 1.0, 1.0));
		Foreground->Render(ForegroundMat);
	} break;
	}

}

void Mutation::TentacleAttack()
{
	tentacleState = TentacleState::Stab;
	tentacle->PlaySprite("tentacle_stab");
	Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_akirasyringe_appear_01");

}
