#include "pch.h"
#include "Player.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Engine.h"
#include "InputSystem.h"
#include "TextureManager.h"
#include "Random.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "SpriteManagerComponent.h"
#include "TextureComponent.h"
#include "AudioManager.h"
#include "ObjectManager.h"

#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"

#include "Terrain.h"
#include "Item.h"
#include "Particle.h"
#include "Melee.h"
#include "BloodEffect.h"
#include "Light.h"
#include "SongTitle.h"
#include "Door.h"
constexpr float PLAYER_SCALE = 2.;

constexpr float DOWN_VEL = 500.;

constexpr float JUMP_INIT_SPEED = 500.;
constexpr float JUMP_LIMIT_TIMER = .3;

constexpr float RUN_ACC = 3000.;
constexpr float RUN_SPEED_LIMIT = 400.;

constexpr float DASH_SPEED = 900.;
constexpr float DASH_DURATION = 0.05;

constexpr float ROLL_COOLDOWN = 0.4;
constexpr float THROWING_SPEED = 2000.;

constexpr float MIN_SLOW_FACTOR = 0.1;
constexpr float MAX_BATTERY = 9.0;

constexpr float AFTERIMAGE_SCALE = 1.1;
Player::Player(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	auto transform = AddComponent<TransformComponent>();
	transform->SetPosition(300, 250);
	transform->SetScale(PLAYER_SCALE, PLAYER_SCALE);
	Engine::GetInstance().playerCam = AddComponent<CameraComponent>();

	auto collision = AddComponent<AABBCollisionComponent>();
	collision->SetSize({ 40, 68 });

	auto playerCam = AddComponent<CameraComponent>();
	playerCam->Initialize();
	playerCam->SetDeadZone({ 200.f, 100.f }, { -200.f, 0.f });
	Engine::GetInstance().playerCam = playerCam;

	auto sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Object/obj_player/playerSpriteData.txt");
	sprite->PlaySprite("Player_Idle_");
	Engine::GetInstance().GetTextureManager()->LoadTexture("laser_contact", "Resource/Texture/Object/obj_player/player_laser_contact.png");
	Engine::GetInstance().GetTextureManager()->LoadTexture("player_slow_effect", "Resource/Texture/Object/obj_player/player_slow.png");
}

void Player::Update(double dt)
{
	

	Engine::GetInstance().GetAudioManager()->SetVolume("player_wallslide", 0);
	if (currState != &stateFlip || currState != &stateRoll)
	{
		isInvincible = false;
	}
	if (owner->GetFrameIndex() == 0)
	{
		die = false;
		willDie = false;
		currentBattery = 9;
		memoryTimer = 60.;
		currItem = ItemType::None;
		itemOwn = nullptr;
		GetComponent<SpriteManagerComponent>()->Freeze(false);
		currentGround = firstGround;
	}
	memoryTimer -= dt;
	if (!willDie && memoryTimer <= 0.)
	{
		willDie = true;
		ChangeState(&stateHurtFly);
	}
	auto transform = GetComponent<TransformComponent>();
	auto inputSystem = Engine::GetInstance().GetInputSystem();
	if (inputSystem->IsKeyPressed(InputKey::ESC))
	{
		Engine::GetInstance().GetAudioManager()->PauseSound(Engine::GetInstance().GetSceneManager()->activeScene->bgmName);

		owner->pauseSwitch = true;
		Engine::GetInstance().GetAudioManager()->StartSound("static");

	}
	if (die && inputSystem->IsKeyPressed(InputKey::LButton))
	{
		ChangeState(&stateIdle);
	}
	if (CanMove && !willDie && currState != &statePlaySong)
	{
		rollTimer += dt;

		if (inputSystem->IsKeyPressed(InputKey::RButton))
		{
			if (!itemOwn)
			{
				auto items = owner->GetObjectList(ObjectType::Item);
				for (auto item : items)
				{
					if (glm::distance(item->GetComponent<TransformComponent>()->GetPosition(), transform->GetPosition()) < 100)
					{
						auto target = dynamic_cast<Item*>(item.get());
						{
							if (target->GetStatus() == ItemStatus::Unpicked)
							{
								Engine::GetInstance().GetAudioManager()->StartSound("player_pickup");

								itemOwn = target;
								itemOwn->GetComponent<TransformComponent>()->SetScale({ 1, 1 });
								itemOwn->GetComponent<TransformComponent>()->SetPosition({-100, -100});
								itemOwn->SetStatus(ItemStatus::Picked);
								currItem = itemOwn->GetItemType();
							}
						}
					}
				}
			}
			else
			{
				Engine::GetInstance().GetAudioManager()->StartSound("player_throw");

				itemOwn->SetStatus(ItemStatus::Thrown);
				itemOwn->GetComponent<TransformComponent>()->SetPosition(transform->GetPosition());
				glm::vec2 dir = glm::normalize(inputSystem->GetMousePos() + Engine::GetInstance().playerCam->GetAppliedPos()  - transform->GetPosition());
				if (dir.x < 0 && dir.y > 0)
				{
					itemOwn->GetComponent<TransformComponent>()->ScaleBy({ -1, -1 });
				}
				else if (dir.x < 0)
				{
					itemOwn->GetComponent<TransformComponent>()->ScaleBy({ -1, 1 });
				}
				else if (dir.y > 0)
				{
					itemOwn->GetComponent<TransformComponent>()->ScaleBy({ 1, -1 });
				}

				itemOwn->SetVelocity({ dir * THROWING_SPEED });
				currItem = ItemType::None;
				itemOwn = nullptr;
			}
		}

		if (currState != &stateAttack) // 공격 중 조작 불가
		{
			if (currentGround) // 지상 조작
			{
				if (currState != &stateRoll) // 구르기 중 조작 불가
				{
					if (inputSystem->IsKeyPressed(InputKey::A))
					{
						isRightWallSlidable = false;
						transform->SetScale({ -PLAYER_SCALE, PLAYER_SCALE });
						if (!isLeftWallSlidable)
						{
							if (currState == &statePreCrouch ||
								currState == &stateCrouch ||
								currState == &statePostCrouch)
							{
								if (rollTimer > ROLL_COOLDOWN)
								{
									ChangeState(&stateRoll);
									rollTimer = 0.;
								}
							}
							else
							{
								ChangeState(&stateIdleToRun);
							}
						}
					}

					if (inputSystem->IsKeyPressed(InputKey::D))
					{
						isLeftWallSlidable = false;
						transform->SetScale({ PLAYER_SCALE, PLAYER_SCALE });
						if (!isRightWallSlidable)
						{
							if (currState == &statePreCrouch ||
								currState == &stateCrouch ||
								currState == &statePostCrouch)
							{
								if (rollTimer > ROLL_COOLDOWN)
								{
									ChangeState(&stateRoll);
									rollTimer = 0.;
								}
							}
							else
							{
								ChangeState(&stateIdleToRun);
							}
						}
					}

					if (inputSystem->IsKeyPressed(InputKey::S))
					{
						if (currState == &stateIdle)
						{
							if (isThroughableGround)
							{
								currentGround = nullptr;
								ChangeState(&stateFall);
								transform->Translate(0, -21);
								isThroughableGround = false;
							}
							else
							{
								ChangeState(&statePreCrouch);
							}
						}
						else if (currState == &stateIdleToRun ||
							currState == &stateRun)
						{
							if (rollTimer > ROLL_COOLDOWN)
							{
								ChangeState(&stateRoll);
								rollTimer = 0.;
							}
						}
					}

					if (inputSystem->IsKeyPressed(InputKey::W))
					{
						ChangeState(&stateJump);
					}

					if (inputSystem->IsKeyPressed(InputKey::Space))
					{
						if (rollTimer > ROLL_COOLDOWN)
						{
							ChangeState(&stateRoll);
							rollTimer = 0.;
						}
					}
				}
			}
			else // 공중 및 벽타기 중 조작
			{
				if (currState != &stateWallSlide)
				{
					if (inputSystem->IsKeyDown(InputKey::D) && !inputSystem->IsKeyDown(InputKey::A) && isRightWallSlidable)
					{
						transform->SetScale({ PLAYER_SCALE, PLAYER_SCALE });
						ChangeState(&stateWallSlide);
					}
					if (inputSystem->IsKeyDown(InputKey::A) && !inputSystem->IsKeyDown(InputKey::D) && isLeftWallSlidable)
					{
						transform->SetScale({ -PLAYER_SCALE, PLAYER_SCALE });
						ChangeState(&stateWallSlide);
					}
				}
				else
				{
					if (inputSystem->IsKeyPressed(InputKey::A) && isRightWallSlidable)
					{
						transform->SetScale({ -PLAYER_SCALE, PLAYER_SCALE });
						ChangeState(&stateFall);
					}

					if (inputSystem->IsKeyPressed(InputKey::D) && isLeftWallSlidable)
					{
						transform->SetScale({ PLAYER_SCALE, PLAYER_SCALE });
						ChangeState(&stateFall);
					}

					if (inputSystem->IsKeyPressed(InputKey::W))
					{
						ChangeState(&stateFlip);
					}
				}
			}

			if (inputSystem->IsKeyPressed(InputKey::LButton))
			{
				ChangeState(&stateAttack);
			}
		}
		slowTimer += dt / currentSlow;
		if (currentBattery > 0 && inputSystem->IsKeyDown(InputKey::Shift))
		{
			if (!slowSoundOn)
			{
				slowSoundOn = true;
				Engine::GetInstance().GetAudioManager()->StartSound("Slowmo_Enter");
				Engine::GetInstance().GetAudioManager()->StopSound("Slowmo_Exit");
			}
			if (slowTimer > 0.5)
			{
				slowTimer = 0;
				currentBattery--;
			}
			if (currentSlow > MIN_SLOW_FACTOR)
			{
				currentSlow *= 0.95;
				if (abs(currentSlow - MIN_SLOW_FACTOR) < 0.05) currentSlow = MIN_SLOW_FACTOR;
			}
			else
			{
				slowSkill = true;
			}
			Engine::GetInstance().SetDeltaTimeFactor(currentSlow);
		}
		else
		{
			if (slowSoundOn)
			{
				slowSoundOn = false;
				Engine::GetInstance().GetAudioManager()->StopSound("Slowmo_Enter");
				Engine::GetInstance().GetAudioManager()->StartSound("Slowmo_Exit");
			}
			if (slowTimer > 1. && currentBattery < totalBattery)
			{
				slowTimer = 0;
				currentBattery++;
			}
			slowSkill = false;
			if (currentSlow < 1.0) currentSlow += 0.1;
			if (abs(currentSlow - 1.0) < 0.1) currentSlow = 1.0;
			Engine::GetInstance().SetDeltaTimeFactor(currentSlow);
		}
	}
	if(!die)
	{
		Object::Update(dt);

		auto terrains = GetOwner()->GetObjectList(ObjectType::Terrain);
		if (velocity.y < 0. && !currentGround)
		{
			double playerAnkle = transform->GetPosition().y - GetComponent<AABBCollisionComponent>()->GetSize().y / 2 + 20.;
			double highestUnderAnkle = 0.;
			for (auto terrain : terrains)
			{
				auto terrainLineCollision = terrain->GetComponent<LineCollisionComponent>();
				if (terrainLineCollision)
				{
					auto [result, y] = terrainLineCollision->CheckCollide(this);
					if (!result || y > playerAnkle) continue;
					if (y > highestUnderAnkle)
					{
						highestUnderAnkle = y;
						currentGround = terrainLineCollision;
						isThroughableGround = dynamic_cast<Terrain*>(terrain.get())->IsWallThroughable();
						didAirAttack = false;
					}
				}
			}
		}

		auto doors = GetOwner()->GetObjectList(ObjectType::Door);

		for (auto d : doors)
		{
			auto door = dynamic_cast<Door*>(d.get());
			if (!door->close || currState == &stateDoorKick) continue;
			auto doorAABBCollision = door->GetComponent<AABBCollisionComponent>();
			if (doorAABBCollision->CheckCollide(this))
			{
				doorAABBCollision->ResolveCollide(this);
				if(!willDie && CanMove) ChangeState(&stateDoorKick);
			}
		}
		

		transform->Translate({ velocity.x * dt, 0. });
		isLeftWallSlidable = false;
		isRightWallSlidable = false;
		isWallStop = false;
		for (auto terrain : terrains)
		{
			AABBCollisionComponent* terrainAABBCollision = terrain->GetComponent<AABBCollisionComponent>();

			if (terrainAABBCollision && terrainAABBCollision->CheckCollide(this))
			{
				isWallStop = true;
				terrainAABBCollision->ResolveCollide(this);

				if (dynamic_cast<Terrain*>(terrain.get())->IsWallSidable() && terrainAABBCollision->CheckWallSlidable(this))
				{
					if (terrain->GetComponent<TransformComponent>()->GetPosition().x > transform->GetPosition().x) // 왼쪽에서 충돌
					{
						isRightWallSlidable = true;
					}
					else
					{
						isLeftWallSlidable = true;
					}
				}
			}
		}
		transform->Translate({ 0., velocity.y * dt });

		if (currentGround)
		{
			// 지형 위에 여전히 있는지 체크
			if (!currentGround->CheckInside(this))
			{
				currentGround = nullptr; // 지형 위에서 벗어났다면 낙하 시작
				auto terrains = GetOwner()->GetObjectList(ObjectType::Terrain);
				double playerAnkle = transform->GetPosition().y - GetComponent<AABBCollisionComponent>()->GetSize().y / 2 + 20.;
				double highestUnderAnkle = 0.;
				for (auto terrain : terrains)
				{
					auto terrainLineCollision = terrain->GetComponent<LineCollisionComponent>();
					if (terrainLineCollision)
					{
						auto [result, y] = terrainLineCollision->CheckCollide(this);
						if (!result || y > playerAnkle) continue;
						if (y > highestUnderAnkle)
						{
							highestUnderAnkle = y;
							currentGround = terrainLineCollision;
							isThroughableGround = dynamic_cast<Terrain*>(terrain.get())->IsWallThroughable();
							didAirAttack = false;
						}
					}
				}
			}
			else
			{
				// 여전히 지형 위에 있다면 위치를 조정
				currentGround->ResolveCollide(this);
			}
		}

		//점프 올라갈 때, 달리기 최고속도 -> 알파 낮음, 수명 짧음, 진한 분홍
		//플립, 구르기, 공격 -> 진하게, 수명 긺, 밝은 색
		//afterimageTimer += dt;
		if (currentSlow == MIN_SLOW_FACTOR)
		{
			auto spriteManager = GetComponent<SpriteManagerComponent>();
			auto transform = GetComponent<TransformComponent>();
			Afterimage afterImage{};
			afterImage.modelMat = transform->GetModelMatrix();
			afterImage.texture = spriteManager->GetCurrentSprite()->GetCurrentData();
			afterImage.lifetime = 0.1;
			afterImage.info.color = glm::vec4(0.10196f, 1.0f, 1.0f, 0.1);
			afterimages.push_back(afterImage);
		}
		else if (currState == &stateFlip || currState == &stateRoll || currState == &stateAttack && glm::length(velocity) > 200)
		{
			auto spriteManager = GetComponent<SpriteManagerComponent>();
			auto transform = GetComponent<TransformComponent>();
			Afterimage afterImage{};
			afterImage.modelMat = transform->GetModelMatrix();
			afterImage.texture = spriteManager->GetCurrentSprite()->GetCurrentData();
			afterImage.lifetime = 0.2;
			afterImage.info.color = glm::vec4(glm::mix(colorStart, colorEnd, (dashAfterimages.size() + 1) / 12.), 0.2);
			dashAfterimages.push_back(afterImage);
		}
		else if (currState == &stateRun || currState == &stateJump)
		{
			auto spriteManager = GetComponent<SpriteManagerComponent>();
			auto transform = GetComponent<TransformComponent>();
			Afterimage afterImage{};
			afterImage.modelMat = transform->GetModelMatrix();
			afterImage.texture = spriteManager->GetCurrentSprite()->GetCurrentData();
			afterImage.lifetime = 0.2;
			afterImage.info.color = glm::vec4(1.0, 1.0, 1.0, 0.1);
			afterimages.push_back(afterImage);
		}

		for (auto& dashAfterimage : dashAfterimages)
		{
			dashAfterimage.lifetime -= dt;
			dashAfterimage.info.color.a -= dt;
		}

		for (auto& afterimage : afterimages)
		{
			afterimage.lifetime -= dt;
			afterimage.info.color.a -= dt * 0.5;
		}

		afterimages.remove_if([](const auto& img) { return img.lifetime <= 0.0f; });
		dashAfterimages.remove_if([](const auto& img) { return img.lifetime <= 0.0f; });
	}
	if (hidden)
	{
		hiddenTimer += dt;
		if (hiddenTimer > 0.1)
		{
			hiddenTimer = 0.;
			hidden = false;
		}
	}
}

void Player::Render()
{
	if (currState == &stateLaserContact)
	{
		if (stateLaserContact.GetTimer() > 0.)
		{
			RenderInfo info{};
			info.texture2 = Engine::GetInstance().GetTextureManager()->GetTexture(laserContactKey);
			info.blendFactor = 0.5;
			info.renderRatio.y = stateLaserContact.GetTimer();

			auto sprite = GetComponent<SpriteManagerComponent>();
			stateLaserContact.SetSparkHeight(sprite->GetCurrentSprite()->GetCurrentData()->height * (info.renderRatio.y - 0.5));
			sprite->Render(info);
		}
	}
	else
	{
		if (currentSlow == MIN_SLOW_FACTOR)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			TextureData* slowEffect = Engine::GetInstance().GetTextureManager()->GetTexture(slowEffectKey);
			if (!Engine::GetInstance().uPersistentTrails)
			{
				for (auto& dashAfterimage : dashAfterimages)
				{
					dashAfterimage.info.texture2 = slowEffect;
					dashAfterimage.Render(Engine::GetInstance().GetShaderProgram(), Engine::GetInstance().playerCam);
				}
				for (auto& afterimage : afterimages)
				{
					afterimage.info.texture2 = slowEffect;
					afterimage.Render(Engine::GetInstance().GetShaderProgram(), Engine::GetInstance().playerCam);
				}
			}

			auto sprite = GetComponent<SpriteManagerComponent>();
			RenderInfo info{};
			info.texture2 = slowEffect;
			info.blendFactor = 0.6;
			
			sprite->Render(info);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if(Engine::GetInstance().GetRandom()->get(0, 9) < 3)
			{
				{
					auto spark = std::make_shared<Particle>(owner, ObjectType::EffectOverUI);
					auto spartTexture = spark->AddComponent<TextureComponent>("neutron");
					auto sparkTransform = spark->GetComponent<TransformComponent>();
					spark->SetGravityFactor(0.5);

					spark->SetTimer(Engine::GetInstance().GetRandom()->get(0.2, 0.35));
					RenderInfo info{};
					info.isFixed = true;
					info.color.a = 0.7;
					spark->SetRenderInfo(info);
					spark->SetIgnoreSlow(true);
					spark->SetIsGettingDisappearByTime(true);
					double angle = Engine::GetInstance().GetRandom()->get(-7. * PI / 4., -PI / 4.);
					sparkTransform->SetPosition(15, 706);
					//int size = Engine::GetInstance().GetRandom()->get(1., 1.);
					sparkTransform->SetScale(1., 1.);
					sparkTransform->SetRotation(angle);
					float speed = Engine::GetInstance().GetRandom()->get(200., 300.);
					spark->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
					owner->AddObject(ObjectType::EffectOverUI, spark);
				}
				{
					auto spark = std::make_shared<Particle>(owner, ObjectType::EffectOverUI);
					auto spartTexture = spark->AddComponent<TextureComponent>("neutron");
					auto sparkTransform = spark->GetComponent<TransformComponent>();
					spark->SetGravityFactor(0.5);

					spark->SetTimer(Engine::GetInstance().GetRandom()->get(0.2, 0.35));
					RenderInfo info{};
					info.isFixed = true;
					info.color.a = 0.7;
					spark->SetRenderInfo(info);
					spark->SetIgnoreSlow(true);
					spark->SetIsGettingDisappearByTime(true);
					double angle = Engine::GetInstance().GetRandom()->get(-5. * PI / 4., 3 * PI / 4.);
					sparkTransform->SetPosition(165, 706);
					//int size = Engine::GetInstance().GetRandom()->get(1., 1.);
					sparkTransform->SetScale(0.5, 0.5);
					sparkTransform->SetRotation(angle);
					float speed = Engine::GetInstance().GetRandom()->get(200., 300.);
					spark->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
					owner->AddObject(ObjectType::EffectOverUI, spark);
				}
			}
			
		}
		else
		{
			RenderInfo info{};
			
			auto lights = owner->GetObjectList(ObjectType::Light);
			Light* targetLight = nullptr;
			float targetDist = 10000.f;
			for (auto light : lights)
			{
				float dist = glm::distance(GetComponent<TransformComponent>()->GetPosition(), light->GetComponent<TransformComponent>()->GetPosition());
				if (dist < 200 && dist < targetDist)
				{
					targetDist = dist;
					targetLight = dynamic_cast<Light*>(light.get());
				}
			}
			if (targetLight)
			{
				info.useLight = true;
				info.lightColor = targetLight->GetColor();
				info.lightPos = { targetLight->GetComponent<TransformComponent>()->GetPosition().x, Engine::GetInstance().GetOriginalWindowSize().bottom - targetLight->GetComponent<TransformComponent>()->GetPosition().y };
				info.lightPower = 1000;
			}
			if (!Engine::GetInstance().uPersistentTrails)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				for (auto& dashAfterimage : dashAfterimages)
				{
					dashAfterimage.Render(Engine::GetInstance().GetShaderProgram(), Engine::GetInstance().playerCam);
				}
				
				for (auto& afterimage : afterimages)
				{
					if (targetLight)
					{
						afterimage.info.useLight = true;
						afterimage.info.lightColor = targetLight->GetColor();
						info.lightPos = { targetLight->GetComponent<TransformComponent>()->GetPosition().x, Engine::GetInstance().GetOriginalWindowSize().bottom - targetLight->GetComponent<TransformComponent>()->GetPosition().y };
						afterimage.info.lightPower = 1000;
					}
					afterimage.Render(Engine::GetInstance().GetShaderProgram(), Engine::GetInstance().playerCam);
				}
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			auto sprite = GetComponent<SpriteManagerComponent>();
			sprite->Render(info);
		}
	}
	
	if (auto AABB = GetComponent<AABBCollisionComponent>())
	{
		AABB->Render();
	}
}

void Player::StatePlaySong::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Play_Song_");
	auto player = dynamic_cast<Player*>(obj);
	player->velocity = { 0 ,-1 };
	Engine::GetInstance().GetAudioManager()->StartSound("player_cassetrewind", false);
	player->GetComponent<AABBCollisionComponent>()->SetSize({40, 72});
	timer = 0.;
	chosen = false;
}

void Player::StatePlaySong::Update(Object* obj, double dt)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (!chosen && spriteManager->GetCurrentSprite()->GetCurrentFrame() == 27)
	{
		Engine::GetInstance().GetAudioManager()->StopSound("player_cassetrewind");
		Engine::GetInstance().GetAudioManager()->StartSound("player_cassetplay", false);
		chosen = true;
		obj->GetOwner()->AddObject(ObjectType::EffectOverUI, std::make_shared<SongTitle>(obj->GetOwner(), ObjectType::EffectOverUI));
	}
	if (spriteManager->IsDone())
	{
		timer += dt;
	}
}

void Player::StatePlaySong::TestForExit(Object* obj)
{
	if (timer > 1.0)
	{
		auto player = dynamic_cast<Player*>(obj);
		player->GetComponent<AABBCollisionComponent>()->SetSize({ 40, 68 });
		Engine::GetInstance().GetAudioManager()->StartSound(Engine::GetInstance().GetSceneManager()->activeScene->bgmName, false);
		player->ChangeState(&player->stateIdle);
	}

}


void Player::StateIdle::Enter(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Idle_");
	auto player = dynamic_cast<Player*>(obj);
	player->velocity = { 0 ,0 };
}

void Player::StateIdle::Update(Object*, double) {}

void Player::StateIdle::TestForExit(Object* obj) 
{
	auto player = dynamic_cast<Player*>(obj);

	if (!player->currentGround)
	{
		player->ChangeState(&player->stateFall);
	}
}
//
//void Player::StateIdleToWalk::Enter(Object* obj) {}
//
//void Player::StateIdleToWalk::Update(Object* obj, double dt) {}
//
//void Player::StateIdleToWalk::TestForExit(Object* obj) {}
//
//void Player::StateWalk::Enter(Object* obj) {}
//
//void Player::StateWalk::Update(Object* obj, double dt) {}
//
//void Player::StateWalk::TestForExit(Object* obj) {}

void Player::StateIdleToRun::Enter(Object* obj) 
{	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Idle_To_Run_");
	Engine::GetInstance().GetAudioManager()->StartSound("player_prerun");
	dir = obj->GetComponent<TransformComponent>()->GetScale().x;
	for (int i = 0; i < 5; i++)
	{
		auto dustCloud = std::make_shared<Particle>(obj->GetOwner(), ObjectType::BackgroundEffect);
		dustCloud->SetIsDeadBySpriteEnd(true);
		RenderInfo info{};
		info.color.a = 0.7;
		dustCloud->SetRenderInfo(info);
		auto sprite = dustCloud->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/clouds/cloudSptData.txt");
		sprite->PlaySprite("Dust_Cloud_");
		auto transform = dustCloud->GetComponent<TransformComponent>();
		transform->SetPosition(obj->GetComponent<TransformComponent>()->GetPosition() - glm::vec2{ 0.f, 36.f });
		if (dir < 0) //왼쪽으로 달림
		{
			double angle = Engine::GetInstance().GetRandom()->get(0., PI/3.);
			transform->SetScale(2, 2);
			transform->SetRotation(angle);
			float speed = Engine::GetInstance().GetRandom()->get(100., 200.);
			dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } * speed);
			//double start = Engine::GetInstance().GetRandom()->get(0., 0.15);
			//dustCloud->SetStartTimer(start);

		}
		else if (dir > 0) // 오른쪽으로 달림
		{
			double angle = Engine::GetInstance().GetRandom()->get(PI * 2. / 3., PI);
			transform->SetScale(2, -2);
			transform->SetRotation(angle);
			float speed = Engine::GetInstance().GetRandom()->get(100., 200.);
			dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
			//double start = Engine::GetInstance().GetRandom()->get(0., 0.15);
			//dustCloud->SetStartTimer(start);
		}
		obj->GetOwner()->AddObject(ObjectType::BackgroundEffect, dustCloud);
	}
}

void Player::StateIdleToRun::Update(Object* obj, double dt) 
{
	auto player = dynamic_cast<Player*>(obj);

	if (dir > 0)
	{
		if (player->velocity.x < RUN_SPEED_LIMIT) player->velocity.x += RUN_ACC * dt;
	}
	else
	{
		if (player->velocity.x > -RUN_SPEED_LIMIT) player->velocity.x -= RUN_ACC * dt;
	}
	
}

void Player::StateIdleToRun::TestForExit(Object* obj) 
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		player->ChangeState(&player->stateRun);
	}
	if (dir < 0 && Engine::GetInstance().GetInputSystem()->IsKeyReleased(InputKey::A) || dir > 0 && Engine::GetInstance().GetInputSystem()->IsKeyReleased(InputKey::D))
	{
		player->ChangeState(&player->stateRunToIdle);
	}
	if (!player->currentGround)
	{
		player->ChangeState(&player->stateFall);
	}
}

void Player::StateRun::Enter(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Run_");
	dir = obj->GetComponent<TransformComponent>()->GetScale().x;
	footStepSwitch = false;
}

void Player::StateRun::Update(Object* obj, double dt) 
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	int footstep = spriteManager->GetCurrentSprite()->GetCurrentFrame();
	if (footStepSwitch && footstep != 3 && footstep != 8)
	{
		footStepSwitch = false;
	}
	if (!footStepSwitch && (footstep == 3 || footstep == 8))
	{
		footStepSwitch = true;
		Engine::GetInstance().GetAudioManager()->StartSound("player_running_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 4)));
	}

	if (dir > 0)
	{
		if(player->velocity.x < RUN_SPEED_LIMIT) player->velocity.x += RUN_ACC * dt;
	}
	else
	{
		if (player->velocity.x > -RUN_SPEED_LIMIT) player->velocity.x -= RUN_ACC * dt;
	}
	
}

void Player::StateRun::TestForExit(Object* obj) 
{
	auto player = dynamic_cast<Player*>(obj);
	if (dir < 0 && Engine::GetInstance().GetInputSystem()->IsKeyReleased(InputKey::A) || 
		dir > 0 && Engine::GetInstance().GetInputSystem()->IsKeyReleased(InputKey::D) || 
		player->isLeftWallSlidable ||
		player->isRightWallSlidable || player->isWallStop)
	{
		player->ChangeState(&player->stateRunToIdle);
	}
	if (!player->currentGround)
	{
		player->ChangeState(&player->stateFall);
	}
}

void Player::StateRunToIdle::Enter(Object* obj) 
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Run_To_Idle_");
	dir = obj->GetComponent<TransformComponent>()->GetScale().x;
}

void Player::StateRunToIdle::Update(Object* obj, double dt) 
{
	auto player = dynamic_cast<Player*>(obj);
	if (dir > 0)
	{
		player->velocity.x -= RUN_ACC * dt;
		if (player->velocity.x < 10) player->velocity.x = 0.;
	}
	else if (dir < 0)
	{
		player->velocity.x += RUN_ACC * dt;
		if (player->velocity.x > -10) player->velocity.x = 0.;
	} 
}

void Player::StateRunToIdle::TestForExit(Object* obj) 
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		player->velocity.x = 0.;
		player->ChangeState(&player->stateIdle);
	}
	if (!player->currentGround)
	{
		player->ChangeState(&player->stateFall);
	}
}

void Player::StateJump::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	Engine::GetInstance().GetAudioManager()->StartSound("player_jump");
	spriteManager->PlaySprite("Player_Jump_");
	auto player = dynamic_cast<Player*>(obj);
	player->velocity.y = JUMP_INIT_SPEED;
	auto normal = player->currentGround->GetNormalFromDiagonal(player);
	if (player->velocity.x * normal.x < 0.)
	{
		player->velocity.x = 0;
	}
	timer = 0.;
	player->currentGround = nullptr;
	{
		auto jumpCloud = std::make_shared<Particle>(player->owner, ObjectType::BackgroundEffect);
		jumpCloud->SetIsDeadBySpriteEnd(true);
		//RenderInfo info{};
		//info.color.a = 0.5;
		//jumpCloud->SetRenderInfo(info);
		auto sprite = jumpCloud->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/clouds/cloudSptData.txt");
		sprite->PlaySprite("Jump_Cloud_");
		auto transform = jumpCloud->GetComponent<TransformComponent>();
		transform->SetPosition(player->GetComponent<TransformComponent>()->GetPosition() + glm::vec2{0.f, 16.f});
		transform->SetScale(2, 2);
		player->owner->AddObject(ObjectType::BackgroundEffect, jumpCloud);
	}
}

void Player::StateJump::Update(Object* obj, double dt)
{
	timer += dt;
	auto player = dynamic_cast<Player*>(obj);
	auto transform = player->GetComponent<TransformComponent>();
	if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::D))
	{
		transform->SetScale(PLAYER_SCALE, PLAYER_SCALE);
		if (player->velocity.x < RUN_SPEED_LIMIT)player->velocity.x += RUN_ACC * dt;
	}
	else if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::A))
	{
		transform->SetScale(-PLAYER_SCALE, PLAYER_SCALE);
		if (player->velocity.x > -RUN_SPEED_LIMIT) player->velocity.x -= RUN_ACC * dt;
	}
	player->velocity.y -= GRAVITY * (0.25 + timer) * dt;
}

void Player::StateJump::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	if (timer > JUMP_LIMIT_TIMER || Engine::GetInstance().GetInputSystem()->IsKeyReleased(InputKey::W))
	{
		player->ChangeState(&player->stateFall);
	}
}

void Player::StateFall::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Fall_");
}

void Player::StateFall::Update(Object* obj, double dt)
{
	auto player = dynamic_cast<Player*>(obj);
	auto transform = player->GetComponent<TransformComponent>();

	if (!Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::A) && Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::D))
	{
		transform->SetScale(PLAYER_SCALE, PLAYER_SCALE);
		if (player->velocity.x < RUN_SPEED_LIMIT) player->velocity.x += RUN_ACC * dt;
	}
	else if (!Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::D) && Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::A))
	{
		transform->SetScale(-PLAYER_SCALE, PLAYER_SCALE);
		if (player->velocity.x > -RUN_SPEED_LIMIT) player->velocity.x -= RUN_ACC * dt;
	}
	player->velocity.y -= GRAVITY * dt;
	if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::S))  player->velocity.y = -500;

}

void Player::StateFall::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);	
	if (player->currentGround)
	{
		player->ChangeState(&player->stateHangUp);
	}
}

void Player::StateWallSlide::Enter(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Wall_Slide_");
	effectTimer = 0.;
	Engine::GetInstance().GetAudioManager()->StartSound("player_wallslide", false);
}

void Player::StateWallSlide::Update(Object* obj, double dt)
{
	auto player = dynamic_cast<Player*>(obj);
	if(player->velocity.y < -100) player->velocity.y = -100;
	if(Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::S))  player->velocity.y = -200;
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (player->velocity.y < 0) player->didAirAttack = false;
	player->velocity.y -= GRAVITY * 0.35 * dt;
	
	Engine::GetInstance().GetAudioManager()->SetVolume("player_wallslide", std::clamp(abs(player->velocity.y) / 100., 0.0, 0.2));

	effectTimer += dt;
	if (effectTimer > 0.06)
	{
		effectTimer = 0.;
		for (int i = 0; i < 3; i++)
		{
			auto dustCloud = std::make_shared<Particle>(obj->GetOwner(), ObjectType::BackgroundEffect);
			dustCloud->SetIsDeadBySpriteEnd(true);
			RenderInfo info{};
			info.color.a = 0.7;
			dustCloud->SetRenderInfo(info);
			dustCloud->SetIsGettingDisappearByTime(true);
			auto sprite = dustCloud->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/clouds/cloudSptData.txt");
			sprite->PlaySprite("Dust_Cloud_");
			auto transform = dustCloud->GetComponent<TransformComponent>();
			transform->SetPosition(obj->GetComponent<TransformComponent>()->GetPosition() - glm::vec2{ 0.f, 36.f });
			if (player->isLeftWallSlidable) //왼쪽 벽
			{
				transform->Translate(-18, 0);
				if (player->velocity.y < 0) // 내려감
				{
					double angle = Engine::GetInstance().GetRandom()->get(PI / 4., PI / 2.);
					transform->SetScale(2, 2);
					transform->SetRotation(angle);
					float speed = Engine::GetInstance().GetRandom()->get(50., 100.);
					dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } * speed);

				}
				else if (player->velocity.y > 0) // 올라감
				{
					double angle = Engine::GetInstance().GetRandom()->get(-PI / 2., -PI / 4.);
					transform->SetScale(2, 2);
					transform->SetRotation(angle);
					float speed = Engine::GetInstance().GetRandom()->get(50., 100.);
					dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
				}
			}
			else
			{
				transform->Translate(18, 0);
				if (player->velocity.y < 0) // 내려감
				{
					double angle = Engine::GetInstance().GetRandom()->get(PI / 2., PI * 3 / 4.);
					transform->SetScale(2, -2);
					transform->SetRotation(angle);
					float speed = Engine::GetInstance().GetRandom()->get(50., 100.);
					dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);

				}
				else if (player->velocity.y > 0) // 올라감
				{
					double angle = Engine::GetInstance().GetRandom()->get(-PI * 3 / 4. , -PI / 2.);
					transform->SetScale(2, -2);
					transform->SetRotation(angle);
					float speed = Engine::GetInstance().GetRandom()->get(50., 100.);
					dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
				}
			}


				double angle = Engine::GetInstance().GetRandom()->get(PI * 2. / 3., PI);
				transform->SetScale(2, -2);
				transform->SetRotation(angle);
				float speed = Engine::GetInstance().GetRandom()->get(100., 200.);
				dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
				//double start = Engine::GetInstance().GetRandom()->get(0., 0.15);
				//dustCloud->SetStartTimer(start);
			
			obj->GetOwner()->AddObject(ObjectType::BackgroundEffect, dustCloud);
		}
	}
}

void Player::StateWallSlide::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto transform = player->GetComponent<TransformComponent>();
	bool isLeftWall = transform->GetScale().x < 0;
	if (player->currentGround)
	{
		player->ChangeState(&player->stateIdle);
	}
	if (isLeftWall && !player->isLeftWallSlidable || 
		!isLeftWall && !player->isRightWallSlidable)
	{
		player->velocity.x = 0;
		player->ChangeState(&player->stateFall);
	}
}

void Player::StateFlip::Enter(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	player->isInvincible = true;
	Engine::GetInstance().GetAudioManager()->StartSound("player_roll");
	Engine::GetInstance().GetAudioManager()->StartSound("player_jump");
	auto transform = player->GetComponent<TransformComponent>();
	int dir = transform->GetScale().x;		
	
	if (dir < 0.) //왼쪽 벽에 붙음
	{
		player->isLeftWallSlidable = false;
		player->velocity = { JUMP_INIT_SPEED * 1.5, DASH_SPEED };
	}
	else if (dir > 0.)
	{
		player->isRightWallSlidable = false;
		player->velocity = { -JUMP_INIT_SPEED * 1.5, DASH_SPEED };
	}
	transform->ScaleBy(-1, 1);

	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Flip_");
}

void Player::StateFlip::Update(Object* obj, double dt)
{
	auto player = dynamic_cast<Player*>(obj);
	player->velocity.y -= GRAVITY * dt;
	player->isInvincible = true;
}

void Player::StateFlip::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		player->isInvincible = false;

		if (player->velocity.x > RUN_SPEED_LIMIT) player->velocity.x = RUN_SPEED_LIMIT;
		else if (player->velocity.x < -RUN_SPEED_LIMIT) player->velocity.x = -RUN_SPEED_LIMIT;
		player->ChangeState(&player->stateFall);
	}

	if (player->isLeftWallSlidable || player->isRightWallSlidable)
	{
		player->isInvincible = false;
		player->ChangeState(&player->stateWallSlide);
	}
}

void Player::StateHangUp::Enter(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Hang_Up_");
	Engine::GetInstance().GetAudioManager()->StartSound("player_land");
	player->velocity.y = 0.;
	if (!Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::A) && !Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::D))
	{
		player->velocity.x = 0.;
	}
	{
		auto landCloud = std::make_shared<Particle>(player->owner, ObjectType::BackgroundEffect);
		landCloud->SetIsDeadBySpriteEnd(true);
		//RenderInfo info{};
		//info.color.a = 0.5;
		//landCloud->SetRenderInfo(info);
		auto sprite = landCloud->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/clouds/cloudSptData.txt");
		sprite->PlaySprite("Land_Cloud_");
		auto transform = landCloud->GetComponent<TransformComponent>();
		transform->SetPosition(player->GetComponent<TransformComponent>()->GetPosition() + glm::vec2{0, -22});
		transform->SetScale(2, 2);
		player->owner->AddObject(ObjectType::BackgroundEffect, landCloud);
	}

}

void Player::StateHangUp::Update(Object* obj, double dt)
{
}

void Player::StateHangUp::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		auto transform = player->GetComponent<TransformComponent>();
		if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::D))
		{
			transform->SetScale(PLAYER_SCALE, PLAYER_SCALE);
			player->ChangeState(&player->stateIdleToRun);
		}
		else if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::A))
		{
			transform->SetScale(-PLAYER_SCALE, PLAYER_SCALE);
			player->ChangeState(&player->stateIdleToRun);
		}
		else
		{
			player->velocity.x = 0;
			player->ChangeState(&player->stateIdle);
		}
	}
}


void Player::StatePreCrouch::Enter(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Pre_Crouch_");
	player->velocity = { 0, 0 };
}

void Player::StatePreCrouch::Update(Object* obj, double dt)
{
}

void Player::StatePreCrouch::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		player->ChangeState(&player->stateCrouch);
	}
}


void Player::StateCrouch::Enter(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Crouch_");
}

void Player::StateCrouch::Update(Object* obj, double dt)
{}

void Player::StateCrouch::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);

	if (Engine::GetInstance().GetInputSystem()->IsKeyReleased(InputKey::S))
	{
		player->ChangeState(&player->statePostCrouch);
	}	
	if (!player->currentGround)
	{
		player->ChangeState(&player->stateFall);
	}
}

void Player::StatePostCrouch::Enter(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Post_Crouch_");
}

void Player::StatePostCrouch::Update(Object* obj, double dt)
{
}

void Player::StatePostCrouch::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		player->ChangeState(&player->stateIdle);
	}
}

void Player::StateRoll::Enter(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	dir = player->GetComponent<TransformComponent>()->GetScale().x;
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	Engine::GetInstance().GetAudioManager()->StartSound("player_roll");
	Engine::GetInstance().GetAudioManager()->StartSound("player_roll_real");

	spriteManager->PlaySprite("Player_Roll_");
	if(dir > 0) player->velocity.x = 750;
	else if (dir < 0) player->velocity.x = -750;
	player->isInvincible = true;
	effectTimer = 0.;
}

void Player::StateRoll::Update(Object* obj, double dt)
{
	effectTimer += dt;
	if (effectTimer > 0.03)
	{
		effectTimer = 0.;
		for (int i = 0; i < 3; i++)
		{
			auto dustCloud = std::make_shared<Particle>(obj->GetOwner(), ObjectType::BackgroundEffect);
			dustCloud->SetIsDeadBySpriteEnd(true);
			RenderInfo info{};
			info.color.a = 0.7;
			dustCloud->SetRenderInfo(info);
			dustCloud->SetIsGettingDisappearByTime(true);
			auto sprite = dustCloud->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/clouds/cloudSptData.txt");
			sprite->PlaySprite("Dust_Cloud_");
			auto transform = dustCloud->GetComponent<TransformComponent>();
			transform->SetPosition(obj->GetComponent<TransformComponent>()->GetPosition() - glm::vec2{ 0.f, 36.f });
			if (dir < 0) //왼쪽으로 달림
			{
				double angle = Engine::GetInstance().GetRandom()->get(0., PI / 3.);
				transform->SetScale(2, 2);
				transform->SetRotation(angle);
				float speed = Engine::GetInstance().GetRandom()->get(100., 200.);
				dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
				//double start = Engine::GetInstance().GetRandom()->get(0., 0.15);
				//dustCloud->SetStartTimer(start);

			}
			else if (dir > 0) // 오른쪽으로 달림
			{
				double angle = Engine::GetInstance().GetRandom()->get(PI * 2. / 3., PI);
				transform->SetScale(2, -2);
				transform->SetRotation(angle);
				float speed = Engine::GetInstance().GetRandom()->get(100., 200.);
				dustCloud->SetVelocity(glm::vec2{ cos(angle), sin(angle) } *speed);
				//double start = Engine::GetInstance().GetRandom()->get(0., 0.15);
				//dustCloud->SetStartTimer(start);
			}
			obj->GetOwner()->AddObject(ObjectType::BackgroundEffect, dustCloud);
		}
	}
	auto player = dynamic_cast<Player*>(obj);
	player->isInvincible = true;

}

void Player::StateRoll::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto transform = player->GetComponent<TransformComponent>();
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		player->isInvincible = false;;

		player->velocity.x = 0.;
		if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::S))
		{
			player->ChangeState(&player->stateCrouch);
		}
		else if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::D))
		{
			transform->SetScale(PLAYER_SCALE, PLAYER_SCALE);
			player->velocity.x = RUN_SPEED_LIMIT * 0.5;
			player->ChangeState(&player->stateIdleToRun);
		}
		else if (Engine::GetInstance().GetInputSystem()->IsKeyDown(InputKey::A))
		{
			transform->SetScale(-PLAYER_SCALE, PLAYER_SCALE);
			player->velocity.x = -RUN_SPEED_LIMIT * 0.5;
			player->ChangeState(&player->stateIdleToRun);
		}
		else
		{
			player->ChangeState(&player->stateIdle);
		}
		
	}
}

void Player::StateAttack::Enter(Object* obj)
{
	
	Engine::GetInstance().GetAudioManager()->StartSound("player_slash_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));

	auto player = dynamic_cast<Player*>(obj);
	player->isLeftWallSlidable = false;
	player->isRightWallSlidable = false;
	auto transform = player->GetComponent<TransformComponent>();

	auto spriteManager = player->GetComponent<SpriteManagerComponent>();


	dir = glm::normalize(Engine::GetInstance().GetInputSystem()->GetMousePos() + Engine::GetInstance().playerCam->GetAppliedPos() - transform->GetPosition());

	//std::cout << "mouse.x: " << Engine::GetInstance().GetInputSystem()->GetMousePos().x << ", mouse.y: " << Engine::GetInstance().GetInputSystem()->GetMousePos().y << std::endl;
	//std::cout << "target.x " << transform->GetPosition().x << ", target.y: " << transform->GetPosition().y << std::endl;
	if (dir.x < 0)
	{
		transform->SetScale(-PLAYER_SCALE, PLAYER_SCALE);
	}
	else
	{
		transform->SetScale(PLAYER_SCALE, PLAYER_SCALE);
	}
	spriteManager->PlaySprite("Player_Attack_");

	player->velocity = glm::vec2{ 0.8, 1.2 } * dir * DASH_SPEED;
	if (player->velocity.y > 0.)
	{
		if (!player->didAirAttack)
		{
			player->didAirAttack = true;
			player->currentGround = nullptr;
		}
		else
		{
			player->velocity.y = -10;
		}
	}
	timer = 0.;
	{
		auto attackEffect = std::make_shared<Melee>(player->owner, ObjectType::PBullet);
		attackEffect->SetAttackerTransform(transform);
		auto attackEffectTransform = attackEffect->GetComponent<TransformComponent>();
		attackEffectTransform->SetPosition(transform->GetPosition());
		attackEffectTransform->SetRotation(atan2(dir.y, dir.x));
		attackEffectTransform->SetScale(2.2, dir.x < 0. ? -2.2 : 2.2);
		auto attackEffectSprite = attackEffect->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_player_slash/slashSpriteData.txt");
		attackEffectSprite->PlaySprite("Player_Slash_");
		player->owner->AddObject(ObjectType::PBullet, attackEffect);
		auto attackEffectAABB = attackEffect->GetComponent<AABBCollisionComponent>();
		attackEffectAABB->SetSize({100, 100});
		attackEffectAABB->SetOffset(dir * 60.f);
	}




}

void Player::StateAttack::Update(Object* obj, double dt)
{
	auto player = dynamic_cast<Player*>(obj);
	timer += dt;

	if (timer > DASH_DURATION)
	{
		if(abs(player->velocity.x) > abs(dir.x) * DASH_SPEED * 0.4) player->velocity.x *= 0.9;
		if(abs(player->velocity.y) > dir.y * DASH_SPEED * 0.4) player->velocity.y *= 0.9;
	}
}

void Player::StateAttack::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	auto spriteManager = player->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		//player->velocity.y = 0;
		if (player->currentGround)
		{
			player->ChangeState(&player->stateIdle);

		}
		else
		{
			player->ChangeState(&player->stateFall);
		}
	}
}

void Player::StateHurtFlyBegin::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Hurt_Fly_Begin_");

	auto player = dynamic_cast<Player*>(obj);
	if (player->willDie)
	{
		Engine::GetInstance().GetAudioManager()->StartSound("playerdie");
	}

	player->CanMove = false;
	player->isInvincible = false;
	player->currentSlow = 1.0;
	Engine::GetInstance().SetDeltaTimeFactor(1.0);
}

void Player::StateHurtFlyBegin::Update(Object* obj, double dt)
{
}

void Player::StateHurtFlyBegin::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		auto player = dynamic_cast<Player*>(obj);
		player->ChangeState(&player->stateHurtFly);
	}
}

void Player::StateHurtFly::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Hurt_Fly_Loop_");
	timer = 0.;
	auto player = dynamic_cast<Player*>(obj);

	auto blood = std::make_shared<BloodEffect>(player->owner, ObjectType::BackgroundEffect);
	int textureNumber = abs(player->velocity.y) < 100. ? Engine::GetInstance().GetRandom()->get(0, 1) : Engine::GetInstance().GetRandom()->get(2, 4);
	blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
	auto texture = blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
	auto AABB = blood->AddComponent<AABBCollisionComponent>();
	AABB->SetSize(texture->GetSize() / 2.f);
	glm::vec2 dir = -glm::normalize(player->velocity);
	auto bloodTransform = blood->GetComponent<TransformComponent>();
	bloodTransform->SetRotation(atan2(dir.y, dir.x));
	bloodTransform->SetScale(2.f, 2.f);
	bloodTransform->SetPosition(player->GetComponent<TransformComponent>()->GetPosition());
	RenderInfo info{};
	info.color = { 0.5, 0.5, 0.5, 0.9 };
	blood->SetRenderInfo(info);
	player->owner->AddObject(ObjectType::BackgroundEffect, blood);
}

void Player::StateHurtFly::Update(Object* obj, double dt)
{
	auto player = dynamic_cast<Player*>(obj);
	player->velocity.y -= GRAVITY * dt;
	if (player->willDie)
	{
		timer += dt;
		if (timer > 0.1)
		{
			timer = 0.;
			for (int i = 0; i < 3; i++)
			{
				auto blood = std::make_shared<BloodEffect>(player->owner, ObjectType::Blood3);
				int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
				blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
				blood->SetIsGettingDisappearByTime(true);
				glm::vec2 dir = -glm::normalize(player->velocity);
				blood->SetVelocity(glm::rotate(dir * 200.f, (float)(-PI / 6.f + PI / 6.f * i)));
				auto bloodTransform = blood->GetComponent<TransformComponent>();
				bloodTransform->SetRotation(atan2(blood->GetVelocity().y, blood->GetVelocity().x));
				bloodTransform->SetScale(3.f, 3.f);
				bloodTransform->SetPosition(player->GetComponent<TransformComponent>()->GetPosition());
				player->owner->AddObject(ObjectType::Blood3, blood);
			}

			auto blood = std::make_shared<BloodEffect>(player->owner, ObjectType::BackgroundEffect);
			int textureNumber = abs(player->velocity.y) < 100. ? Engine::GetInstance().GetRandom()->get(0, 1) : Engine::GetInstance().GetRandom()->get(2, 4);
			blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
			auto texture = blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
			auto AABB = blood->AddComponent<AABBCollisionComponent>();
			AABB->SetSize(texture->GetSize() / 2.f);
			glm::vec2 dir = -glm::normalize(player->velocity);
			auto bloodTransform = blood->GetComponent<TransformComponent>();
			bloodTransform->SetRotation(atan2(dir.y, dir.x));
			bloodTransform->SetScale(2.f, 2.f);
			bloodTransform->SetPosition(player->GetComponent<TransformComponent>()->GetPosition());
			RenderInfo info{};
			info.color = { 0.5, 0.5, 0.5, 0.9 };
			blood->SetRenderInfo(info);
			player->owner->AddObject(ObjectType::BackgroundEffect, blood);
		}
	}
}

void Player::StateHurtFly::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);
	if (player->currentGround)
	{
		player->ChangeState(&player->stateHurtGround);
	}
}

void Player::StateHurtGround::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Hurt_Ground_");
	Engine::GetInstance().GetAudioManager()->StartSound("player_land");
	timer = 0;
}

void Player::StateHurtGround::Update(Object* obj, double dt)
{
	timer+=dt;
	auto player = dynamic_cast<Player*>(obj);
	player->velocity.x *= 0.9;
}

void Player::StateHurtGround::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (timer > 1.0)
	{
		auto player = dynamic_cast<Player*>(obj);
		player->velocity.x = 0.;
		if (player->willDie)
		{
			player->die = true;
			
		}
		else
		{
			player->ChangeState(&player->stateHurtRecover);
		}
	}
}

void Player::StateHurtRecover::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Hurt_Recover_");
	Engine::GetInstance().GetAudioManager()->StartSound("player_knockdown_recover");
}	

void Player::StateHurtRecover::Update(Object* obj, double dt)
{
}

void Player::StateHurtRecover::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{

		auto player = dynamic_cast<Player*>(obj);
		player->CanMove = true;
		player->ChangeState(&player->stateIdle);
	}
}

double Player::StateLaserContact::GetTimer()
{
	return timer;
}

void Player::StateLaserContact::Enter(Object* obj)
{

	Engine::GetInstance().GetAudioManager()->StartSound("actor_disintegrate_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->Freeze(true);
	auto player = dynamic_cast<Player*>(obj);
	player->willDie = true;
	player->currentSlow = 1.0;
	Engine::GetInstance().SetDeltaTimeFactor(1.0);
	timer = 1.5;
	player->velocity = { 0, 0 };
}

void Player::StateLaserContact::Update(Object* obj, double dt)
{
	timer -= dt * 2.;
	if (timer > 0.)
	{
		auto sprite = obj->GetComponent<SpriteManagerComponent>();
		auto player = dynamic_cast<Player*>(obj);

		auto spark = std::make_shared<Particle>(player->owner, ObjectType::BackgroundEffect);
		auto spartTexture = spark->AddComponent<TextureComponent>("electron");
		auto sparkTransform = spark->GetComponent<TransformComponent>();
		spark->SetGravityFactor(3.0);
		spark->SetTimer(0.2);
		spark->SetIsGettingDisappearByTime(true);

		double angle = Engine::GetInstance().GetRandom()->get(-PI - (PI / 4.), PI / 4.);
		sparkTransform->SetPosition(player->GetComponent<TransformComponent>()->GetPosition() + glm::vec2{ 0.f, sparkHeight } + glm::vec2{ cos(angle), sin(angle) } *10.f);
		int size = Engine::GetInstance().GetRandom()->get(1., 2.);
		sparkTransform->SetScale(size, size);
		sparkTransform->SetRotation(angle);
		float speed = Engine::GetInstance().GetRandom()->get(300., 2000.);
		spark->SetVelocity(glm::vec2{ cos(angle), sin(angle) } * speed);
		player->owner->AddObject(ObjectType::BackgroundEffect, spark);
	}
	
}

void Player::StateLaserContact::TestForExit(Object* obj)
{
	auto player = dynamic_cast<Player*>(obj);

	if (timer < 0.)
	{
		player->die = true;
		auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
		spriteManager->Freeze(false);
	}
	
}

void Player::StateLaserContact::SetSparkHeight(float h)
{
	sparkHeight = h;
}

void Player::StateDoorKick::Enter(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	spriteManager->PlaySprite("Player_Door_Break_Full_");
	auto player = dynamic_cast<Player*>(obj);
	player->CanMove = false;
	player->velocity = { 0, 0 };
}

void Player::StateDoorKick::Update(Object* obj, double dt)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->GetCurrentSprite()->GetCurrentFrame() > 5)
	{
		auto player = dynamic_cast<Player*>(obj);
		auto transform = player->GetComponent<TransformComponent>();
		auto doorOpenRect = std::make_shared<Particle>(player->owner, ObjectType::PBullet);
		auto doorOpenRectTransform = doorOpenRect->GetComponent<TransformComponent>();
		doorOpenRect->SetTimer(0.1);
		auto dir = transform->GetScale().x;
		doorOpenRectTransform->SetPosition(transform->GetPosition() + glm::vec2{25 * dir, 0});
		auto doorOpenRectAABB = doorOpenRect->AddComponent<AABBCollisionComponent>();
		doorOpenRectAABB->SetSize({ 100, 100 });

		player->owner->AddObject(ObjectType::PBullet, doorOpenRect);


		player->CanMove = true;
	}
}

void Player::StateDoorKick::TestForExit(Object* obj)
{
	auto spriteManager = obj->GetComponent<SpriteManagerComponent>();
	if (spriteManager->IsDone())
	{
		auto player = dynamic_cast<Player*>(obj);
		player->ChangeState(&player->stateIdle);
	}
}