#include "pch.h"
#include "BossArm.h"
#include "TextureComponent.h"
#include "SpriteManagerComponent.h"
#include "AABBCollisionComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "Boss.h"
#include "HitEffect.h"

#include "Engine.h"
#include "AudioManager.h"
#include "Random.h"
#include "InputSystem.h"

#include "ObjectManager.h"
#include "BossScene.h"
#include "Player.h"
BossArm::BossArm(ObjectManager* owner, ObjectType type, int armType, Boss* boss) : Object(owner, type), boss(boss), armType(armType)
{
	portal = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	portalCover = std::make_shared<SpriteManagerComponent>("Resource/Texture/Effect/Boss/Portal/portal.txt");
	hand = std::make_shared<SpriteManagerComponent>("Resource/Texture/Object/Boss/obj_hand/obj_hand.txt");
	wrist = std::make_shared<TextureComponent>("boss_wrist", "Resource/Texture/Object/Boss/obj_wrist/0.png");
	arms.push_back(std::make_shared<TextureComponent>("boss_arm_0", "Resource/Texture/Object/Boss/obj_arm/0.png"));
	arms.push_back(std::make_shared<TextureComponent>("boss_arm_1", "Resource/Texture/Object/Boss/obj_arm/1.png"));
	arms.push_back(std::make_shared<TextureComponent>("boss_arm_2", "Resource/Texture/Object/Boss/obj_arm/2.png"));
	arms.push_back(std::make_shared<TextureComponent>("boss_arm_3", "Resource/Texture/Object/Boss/obj_arm/3.png"));
	MakeCurve(armType);
	armStart = Engine::GetInstance().GetRandom()->get(0.0, 1.0);
	AABB = AddComponent<AABBCollisionComponent>();
	AABB->SetSize({ 80, 80 });
	transform = AddComponent<TransformComponent>();
	transform->SetPosition(portalPos);
}

void BossArm::Update(double dt)
{

	switch (currState)
	{
	case ArmState::Wait:
	{
		timer += dt;
		if (timer > 0.)
		{
			timer = 0.;
			portal->PlaySprite("portal_open");
			currState = ArmState::PortalOpen;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_arm_portal_01");
		}
	} break;
	case ArmState::PortalOpen:
	{
		
		portal->Update(dt);
		if (portal->IsDone())
		{
			portalCover->PlaySprite("portal_front");
			portal->PlaySprite("portal_back");
			currState = ArmState::OpenWait;
		}
		if(timer > armStart)
		{
			timer = 0.;
		}
	} break;
	case ArmState::OpenWait:
	{
		timer += dt;
		portal->Update(dt);
		portalCover->Update(dt);
		if (timer > armStart)
		{
			timer = 0.;
			hand->PlaySprite("arm_normal");
			currState = ArmState::HandCommingOut;
		}
	} break;
	case ArmState::HandCommingOut: 
	{
		timer += dt * 1.8;
		portal->Update(dt);
		portalCover->Update(dt);
		hand->Update(dt);
		handIndex = static_cast<int>(floor(timer * 100));
		transform->SetPosition(curvInfo[handIndex].first);
		if (timer > 2.0)
		{
			timer = 2.0;
			currState = ArmState::HandGoal;
		}
	} break;
	case ArmState::HandGoal: 
	{
		timer += dt * 1.8;
		portal->Update(dt);
		portalCover->Update(dt);
		hand->Update(dt);

		if (timer > 2.5)
		{
			timer = 2.0;
			currState = ArmState::HandGoingBack;
		}
	} break;
	case ArmState::HandGrab:
	{
		boss->bossScene->player->GetComponent<TransformComponent>()->SetScale(0, 0);

		portal->Update(dt);
		portalCover->Update(dt);
		hand->Update(dt);
	} break;
	case ArmState::HandExplode:
	{
		portal->Update(dt);
		portalCover->Update(dt);
		hand->Update(dt);
		if (hand->GetName() == "arm_player_die" && hand->IsDone())
		{
			timer = 0.;
			Engine::GetInstance().uGlitchEnabled = true;
			currState = ArmState::Glitch;
			Engine::GetInstance().GetAudioManager()->StartSound("sound_ui_screen_glitch_01");
		}
	} break;
	case ArmState::Glitch:
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
			boss->bossScene->retry = true;
			boss->bossScene->ChangeBossSceneState(BossScene::BossSceneState::ZigZag);
			Engine::GetInstance().uGlitchEnabled = false;
		}
	} break;
	case ArmState::HandHit: 
	{
		portal->Update(dt);
		portalCover->Update(dt);
		timer += dt * 1.8;
		if (timer > 1.5)
		{
			timer = timerWhenHit;
			currState = ArmState::HandGoingBack;
		}
	} break;
	case ArmState::HandGoingBack: 
	{
		timer -= dt * 1.8;
		portal->Update(dt);
		portalCover->Update(dt);
		hand->Update(dt);
		handIndex = static_cast<int>(floor(timer * 100));
		if(handIndex > 0) transform->SetPosition(curvInfo[handIndex].first);
		if (timer < 0.0)
		{
			timer = 0.0;
			portal->PlaySprite("portal_close");
			Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_arm_portal_02");
			currState = ArmState::PortalClose;
		}
	} break;
	case ArmState::PortalClose: 
	{
		portal->Update(dt);
		if (portal->IsDone())
		{
			SetDead();
		}
	} break;
	}

	if (currState == ArmState::HandCommingOut)
	{
		auto player = boss->bossScene->player;
		if (!player->hit && AABB->CheckCollide(player.get()))
		{
			player->GetComponent<TransformComponent>()->SetScale(0, 0);
			player->hit = true;
			player->willDie = true;
			player->CanMove = false;
			player->ChangeState(&player->stateIdle);
			player->SetVelocity({0, 0});
			currState = ArmState::HandGrab;
			hand->PlaySprite("arm_player_struggle");
			if (portalPos.x > 640.) // 오른쪽에서 나온 손
			{
				boss->killPlayerPos = transform->GetPosition() + glm::vec2(-70, 0); //왼쪽으로 이동
				boss->transform->SetScale(2, 2);
			}
			else
			{
				boss->killPlayerPos = transform->GetPosition() + glm::vec2(70, 0); //오른쪽으로 이동
				boss->transform->SetScale(-2, 2);
			}
			boss->ChangeState(Boss::BossState::KillPlayer);
			boss->grabArm = this;
		}
	}
	if (currState == ArmState::HandCommingOut)
	{
		auto attacks = owner->GetObjectList(ObjectType::PBullet);
		for (auto a : attacks)
		{
			if (!hit && AABB->CheckCollide(a.get()))
			{
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
				Engine::GetInstance().GetAudioManager()->StartSound("sound_boss_arm_hurt_0" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
				Engine::GetInstance().GetAudioManager()->StartSound("enemy_death_sword_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
				timerWhenHit = timer;
				timer = 0.;
				hit = true;
				boss->hp--;
				boss->ChangeState(Boss::BossState::ArmAttacked);
				currState = ArmState::HandHit;
				hand->PlaySprite("arm_hurt");
				{ // HitEffect 광선
					auto dir = Engine::GetInstance().GetInputSystem()->GetMousePos() + Engine::GetInstance().playerCam->GetAppliedPos() - a->GetComponent<TransformComponent>()->GetPosition();
					auto hitEffect = std::make_shared<HitEffect>(owner, ObjectType::HitEffect);
					auto hitEffectTransform = hitEffect->GetComponent<TransformComponent>();
					hitEffectTransform->SetPosition(transform->GetPosition());
					hitEffectTransform->SetRotation(atan2(dir.y, dir.x));
					hitEffectTransform->SetScale(4, 2);
					owner->AddObject(ObjectType::HitEffect, hitEffect);
				}
			}
		}
	}
}

void BossArm::Render()
{
	int seeright = 1;
	if (portalPos.x < 640)
	{
		seeright = -1;
	}
	switch (currState)
	{
	case ArmState::OpenWait:
	case ArmState::HandCommingOut:
	case ArmState::HandGoal:
	case ArmState::HandHit:
	case ArmState::HandExplode:
	case ArmState::HandGrab:
	case ArmState::HandGoingBack:
	{
		{
			glm::vec2 newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y };
			glm::mat4 portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 0.0));
			portalMat = glm::rotate(portalMat, 0.f, glm::vec3(0.f, 0.f, 1.f));
			portalMat = glm::scale(portalMat, glm::vec3(seeright * 2.0, 2.0, 1.0));
			portal->Render(portalMat);
		}

		{
			RenderInfo info{};
			info.useBounding = true;
			int filp = 1;
			if (hit) info.color = { 1.0f, 0.f, 0.f, 1.f };
			if (armType < 6)
			{
				info.worldRenderLowerBound.x = portalPos.x - 12;
			}
			else
			{
				filp = -1;
				info.worldRenderUpperBound.x = portalPos.x + 12;
			}

			int armIndex = 0;
			int startIndex = static_cast<int>(floor(timer * 100));
			for (int i = handIndex; i >= 0; i -= 15)
			{
				auto [handPos, handTangent] = curvInfo[i];
				handPos.y = Engine::GetInstance().GetWindowSize().bottom - handPos.y;
				if (currState == ArmState::HandHit) handPos += glm::vec2{Engine::GetInstance().GetRandom()->get(-10, 10), Engine::GetInstance().GetRandom()->get(-10, 10) };
				glm::mat4 handMat = glm::translate(glm::mat4(1.0), glm::vec3(handPos, 0.0));
				float angle = atan2(-handTangent.y, handTangent.x);
				handMat = glm::rotate(handMat, angle, glm::vec3(0.f, 0.f, 1.f));
				handMat = glm::scale(handMat, glm::vec3(2., filp * 2., 1.0));
				if (i == handIndex)
				{
					handMat = glm::translate(handMat, glm::vec3(0, -8, 0));
					hand->Render(handMat, info);
					i -= 8;
				}
				else if (i == handIndex - 23)
				{
					wrist->Render(handMat, info);
				}
				else
				{
					arms[armIndex % 4]->Render(handMat, info);
					armIndex++;
				}
			}
		}
		{
			glm::vec2 newPortalPos = { portalPos.x + seeright * 11., Engine::GetInstance().GetWindowSize().bottom - portalPos.y };
			glm::mat4 portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 0.0));
			portalMat = glm::rotate(portalMat, 0.f, glm::vec3(0.f, 0.f, 1.f));
			portalMat = glm::scale(portalMat, glm::vec3(seeright * 2.0, 2.0, 1.0));
			portalCover->Render(portalMat);
		}
	} break;
	case ArmState::PortalOpen:
	case ArmState::PortalClose:
	{
		glm::vec2 newPortalPos = { portalPos.x, Engine::GetInstance().GetWindowSize().bottom - portalPos.y };
		glm::mat4 portalMat = glm::translate(glm::mat4(1.0), glm::vec3(newPortalPos, 0.0));
		portalMat = glm::rotate(portalMat, 0.f, glm::vec3(0.f, 0.f, 1.f));
		portalMat = glm::scale(portalMat, glm::vec3(seeright * 2.0, 2.0, 1.0));
		portal->Render(portalMat);
	}
	break;
	}

	AABB->Render();
}

std::vector<std::pair<glm::vec2, glm::vec2>> SampleHermiteMultiUniformly(
	const std::vector<glm::vec2>& points,
	const std::vector<glm::vec2>& tangents,
	int sampleCount,
	int highResSample = 500)
{
	struct SamplePoint {
		float arcLength;
		float t;           // local t
		int curveIndex;    // index of curve segment
	};

	std::vector<SamplePoint> samples;
	float totalLength = 0.0f;

	// 점과 탄젠트 개수 확인
	if (points.size() < 2 || tangents.size() != points.size())
		throw std::runtime_error("Invalid input: Need at least two points and same number of tangents.");

	// 각 구간마다 고해상도 샘플링
	for (size_t seg = 0; seg < points.size() - 1; ++seg)
	{
		glm::vec2 p0 = points[seg];
		glm::vec2 p1 = points[seg + 1];
		glm::vec2 t0 = tangents[seg];
		glm::vec2 t1 = tangents[seg + 1];

		glm::vec2 prev = HermiteCurve(p0, p1, t0, t1, 0.0f);

		for (int i = 1; i <= highResSample; ++i)
		{
			float t = static_cast<float>(i) / highResSample;
			glm::vec2 curr = HermiteCurve(p0, p1, t0, t1, t);
			totalLength += glm::length(curr - prev);
			samples.push_back({ totalLength, t, static_cast<int>(seg) });
			prev = curr;
		}
	}

	// 등간격 거리 기반으로 t 역매핑
	std::vector<std::pair<glm::vec2, glm::vec2>> result;
	float segmentSpacing = totalLength / (sampleCount - 1);

	for (int i = 0; i < sampleCount; ++i)
	{
		float targetLength = segmentSpacing * i;

		for (int j = 1; j < samples.size(); ++j)
		{
			if (samples[j].arcLength >= targetLength)
			{
				const auto& s0 = samples[j - 1];
				const auto& s1 = samples[j];

				float ratio = (targetLength - s0.arcLength) / (s1.arcLength - s0.arcLength);
				float tMapped = s0.t + (s1.t - s0.t) * ratio;
				int seg = s0.curveIndex;

				glm::vec2 p0 = points[seg];
				glm::vec2 p1 = points[seg + 1];
				glm::vec2 t0 = tangents[seg];
				glm::vec2 t1 = tangents[seg + 1];

				glm::vec2 pos = HermiteCurve(p0, p1, t0, t1, tMapped);
				glm::vec2 dir = HermiteDerivative(p0, p1, t0, t1, tMapped);
				result.emplace_back(pos, dir);
				break;
			}
		}
	}

	return result;
}

void BossArm::MakeCurve(int armType)
{
	switch (armType)
	{
	case 0:
	{
		portalPos = { 150, 500 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{20, 0},
			{760 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{1200 + Engine::GetInstance().GetRandom()->get(-50, 50), 450 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{1100, 0},
			{400, 0},
			{400, 300},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 1:
	{
		portalPos = { 150, 500 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{20, 0},
			{650 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{1200 + Engine::GetInstance().GetRandom()->get(-50, 50), 200 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{800, 0},
			{400, -300},
			{400, 300},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 2:
	{
		portalPos = { 150, 375 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{20, 0},
			{720 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{1100 + Engine::GetInstance().GetRandom()->get(-50, 50), 350 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{1100, 0},
			{400, 0},
			{400, 0},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 3:
	{
		portalPos = { 150, 375 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{20, 0},
			{720 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{1100 + Engine::GetInstance().GetRandom()->get(-50, 50), 350 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{1100, 0},
			{400, 0},
			{400, 0},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 4:
	{
		portalPos = { 150, 250 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{20, 0},
			{1100 + Engine::GetInstance().GetRandom()->get(-50, 50), 500 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{1500, -100},
			{100, 1300},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 5:
	{
		portalPos = { 150, 250 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{20, 0},
			{1100 + Engine::GetInstance().GetRandom()->get(-50, 50), 650 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{1500, 0},
			{1000, 1000},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 6:
	{
		portalPos = { 1130, 500 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{-20, 0},
			{700 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{100 + Engine::GetInstance().GetRandom()->get(-50, 50), 150 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{-1100, 0},
			{-400, -200},
			{-400, 0},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 7:
	{
		portalPos = { 1130, 500 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{-20, 0},
			{700 + Engine::GetInstance().GetRandom()->get(-50, 50), 300 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{100 + Engine::GetInstance().GetRandom()->get(-50, 50), 550 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{-900, 0},
			{-400, 0},
			{-200, 200},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 8:
	{
		portalPos = { 1130, 375 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{-20, 0},
			{550 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{150 + Engine::GetInstance().GetRandom()->get(-50, 50), 500 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{-1100, 0},
			{-400, 0},
			{-400, 400},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 9:
	{
		portalPos = { 1130, 375 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{-20, 0},
			{550 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
			{150 + Engine::GetInstance().GetRandom()->get(-50, 50), 500 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{-1100, 0},
			{-400, 0},
			{-400, 400},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 10:
	{
		portalPos = { 1130, 250 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{-20, 0},
			{100 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},
		};

		std::vector<glm::vec2> tangents = {
			{-1500, 100},
			{-1000, -100},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	case 11:
	{
		portalPos = { 1130, 250 };

		std::vector<glm::vec2> points = {
			portalPos + glm::vec2{-20, 0},
			{100 + Engine::GetInstance().GetRandom()->get(-50, 50), 250 + Engine::GetInstance().GetRandom()->get(-50, 50)},			
			{100 + Engine::GetInstance().GetRandom()->get(-50, 50), 550 + Engine::GetInstance().GetRandom()->get(-50, 50)},

		};

		std::vector<glm::vec2> tangents = {
			{-1500, 100},
			{-1000, -100},
			{-100, 700},
		};

		curvInfo = SampleHermiteMultiUniformly(points, tangents, 202);
	} break;
	}
}