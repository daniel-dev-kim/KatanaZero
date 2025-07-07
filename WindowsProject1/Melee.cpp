#include "pch.h"
#include "Melee.h"
#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"
#include "SpriteManagerComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Enemy.h"
#include "HitEffect.h"
#include "Engine.h"
#include "Random.h"
#include "InputSystem.h"
#include "BloodEffect.h"
#include "Bullet.h"
#include "AudioManager.h"
#include "Warning.h"
Melee::Melee(ObjectManager* owner, ObjectType type) : Object(owner, type)
{	
	AddComponent<TransformComponent>();
	AddComponent<AABBCollisionComponent>();
}

void Melee::Update(double dt)
{
	Object::Update(dt);
	auto transform = GetComponent<TransformComponent>();
	if(objType == ObjectType::PBullet) transform->SetPosition(attackerTransform->GetPosition());

	if (objType == ObjectType::EBullet)
	{
		auto player = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());
		if (!player->willDie)
		{
			if (GetComponent<AABBCollisionComponent>()->CheckCollide(player))
			{
				auto dir = glm::normalize(player->GetComponent<TransformComponent>()->GetPosition() - attackerTransform->GetPosition());
				Engine::GetInstance().GetAudioManager()->StartSound("enemy_punch_hit");
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);

				player->willDie = true;
				player->currentGround = nullptr;
				player->SetVelocity({ dir.x * 600.f, abs(std::max(0.5f, dir.y) * 600) });
				player->ChangeState(&player->stateHurtFlyBegin);
			}
		}
	}
	else if (objType == ObjectType::PBullet)
	{
		auto bullets = owner->GetObjectList(ObjectType::EBullet);
		for (auto b : bullets)
		{
			auto bullet = dynamic_cast<Bullet*>(b.get());
			if (bullet && !bullet->IsDead() && GetComponent<AABBCollisionComponent>()->CheckCollide(bullet))
			{
				auto newBullet = std::make_shared<Bullet>(owner, ObjectType::PBullet);
				newBullet->SetVelocity(-bullet->GetVelocity());
				newBullet->transform->SetPosition(bullet->transform->GetPosition());
				newBullet->transform->SetScale(bullet->transform->GetScale());
				newBullet->transform->SetRotation(bullet->transform->GetRotation());
				owner->AddObject(ObjectType::PBullet, newBullet);

				auto parryingEffect = std::make_shared<Particle>(owner, ObjectType::Effect);
				parryingEffect->SetIsDeadBySpriteEnd(true);
				parryingEffect->GetComponent<TransformComponent>()->SetPosition(bullet->transform->GetPosition());
				parryingEffect->GetComponent<TransformComponent>()->SetScale(2, 2);
				auto sprite = parryingEffect->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_bulletReflect/bulletReflectSpriteData.txt");
				sprite->PlaySprite("Bullet_Reflect_");
				owner->AddObject(ObjectType::Effect, parryingEffect);

				Engine::GetInstance().GetAudioManager()->StartSound("slash_bullet");
				bullet->SetDead();
				Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
			}
		}


		auto enemies = owner->GetObjectList(ObjectType::Enemy);
		for (auto e : enemies)
		{
			auto enemy = dynamic_cast<Enemy*>(e.get());
			if (!enemy->willDie)
			{
				if (GetComponent<AABBCollisionComponent>()->CheckCollide(enemy))
				{
					Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);

					//Àû Á×À¸¸é 
					if (!enemy->inSmoke)
					{
						auto warning = std::make_shared<Warning>(owner, ObjectType::Neutral2, 700.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
						owner->AddObject(ObjectType::Neutral2, warning);
						Engine::GetInstance().GetAudioManager()->StartSound("enemy_death_sword_" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 2)));
					}

					enemy->Hit(true);
					{ // HitEffect ±¤¼±
						auto dir = Engine::GetInstance().GetInputSystem()->GetMousePos() + Engine::GetInstance().playerCam->GetAppliedPos() - attackerTransform->GetPosition();
						auto hitEffect = std::make_shared<HitEffect>(owner, ObjectType::HitEffect);
						auto hitEffectTransform = hitEffect->GetComponent<TransformComponent>();
						hitEffectTransform->SetPosition(transform->GetPosition());
						hitEffectTransform->SetRotation(atan2(dir.y, dir.x));
						hitEffectTransform->SetScale(4, 2);
						owner->AddObject(ObjectType::HitEffect, hitEffect);
					}
					{
						auto enemyTransform = enemy->GetComponent <TransformComponent>();
						{ // ÇÇ
							auto blood = std::make_shared<BloodEffect>(owner, ObjectType::BackgroundEffect);
							int textureNumber = abs(enemy->GetVelocity().y) < 100. ? Engine::GetInstance().GetRandom()->get(0, 1) : Engine::GetInstance().GetRandom()->get(2, 4);
							auto texture = blood->AddComponent<TextureComponent>("wall_blood_" + std::to_string(textureNumber), "Resource/Texture/WallBlood/" + std::to_string(textureNumber) + ".png");
							auto AABB = blood->AddComponent<AABBCollisionComponent>();
							AABB->SetSize(texture->GetSize() / 2.f);
							glm::vec2 dir = -glm::normalize(enemy->GetVelocity());
							auto bloodTransform = blood->GetComponent<TransformComponent>();
							bloodTransform->SetRotation(atan2(dir.y, dir.x));
							bloodTransform->SetScale(2.f, 2.f);
							bloodTransform->SetPosition(enemyTransform->GetPosition());
							RenderInfo info{};
							info.color = { 0.5, 0.5, 0.5, 0.8 };
							blood->SetRenderInfo(info);
							owner->AddObject(ObjectType::BackgroundEffect, blood);
						}
						auto dir = glm::normalize(enemyTransform->GetPosition() - attackerTransform->GetPosition());
						enemy->SetVelocity(dir * 800.f);
						enemyTransform->Translate(dir * 50.f);

						auto terrains = owner->GetObjectList(ObjectType::Terrain);
						double enemyAnkle = transform->GetPosition().y - GetComponent<AABBCollisionComponent>()->GetSize().y / 2 + 20.;
						double highestUnderAnkle = 0.;
						for (auto terrain : terrains)
						{
							auto terrainLineCollision = terrain->GetComponent<LineCollisionComponent>();
							if (terrainLineCollision)
							{
								auto [result, y] = terrainLineCollision->CheckCollide(this);
								if (!result || y > enemyAnkle) continue;
								if (y > highestUnderAnkle)
								{
									highestUnderAnkle = y;
									enemy->currentGround = terrainLineCollision;

								}
							}
						}

						enemy->currentGround->ResolveCollide(enemy);

						for (auto terrain : terrains)
						{
							auto terrainAABB = terrain->GetComponent<AABBCollisionComponent>();
							if (terrainAABB == nullptr) continue;

							if (terrainAABB->CheckCollide(enemy))
							{
								terrainAABB->ResolveCollide(enemy);
							}
						}

						auto doors = owner->GetObjectList(ObjectType::Door);
						for (auto door : doors)
						{
							auto doorAABB = door->GetComponent<AABBCollisionComponent>();
							if (doorAABB == nullptr) continue;
							if (doorAABB->CheckCollide(enemy))
							{	
								doorAABB->ResolveCollide(enemy);
							}
						}


						enemyTransform->SetScale(dir.x < 0 ? 2 : -2, 2);
						RenderInfo info{};
						info.color = { 1.2, 0.9, 0.5, 1.0 };
						{
							auto jetEffect = std::make_shared<Particle>(owner, ObjectType::Effect);
							jetEffect->SetRenderInfo(info);
							auto sprite = jetEffect->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_JetEffect/JetEffectSpriteData.txt");
							sprite->PlaySprite("Jet_Effect_");
							jetEffect->SetIsDeadBySpriteEnd(true);
							auto jetEffectTransform = jetEffect->GetComponent<TransformComponent>();
							jetEffectTransform->SetPosition(enemyTransform->GetPosition() - dir * 20.f);
							jetEffectTransform->SetRotation(atan2(dir.y, dir.x));
							jetEffectTransform->SetScale(1, 1);
							owner->AddObject(ObjectType::Effect, jetEffect);
						}
						{
							auto cutEffect = std::make_shared<Particle>(owner, ObjectType::Effect);
							cutEffect->SetRenderInfo(info);
							auto sprite = cutEffect->AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_CutEffect/CutEffectSpriteData.txt");
							sprite->PlaySprite("Cut_Effect_");
							cutEffect->SetIsDeadBySpriteEnd(true);
							auto cutEffectTransform = cutEffect->GetComponent<TransformComponent>();
							cutEffectTransform->SetPosition(enemyTransform->GetPosition());
							cutEffectTransform->SetRotation(atan2(dir.y, dir.x));
							cutEffectTransform->SetScale(1, 1);
							owner->AddObject(ObjectType::Effect, cutEffect);
						}
					}
					break;
				}
			}
		}
	}
	

	auto sprite = GetComponent<SpriteManagerComponent>();
	if (sprite && sprite->IsDone())
	{
		isDead = true;
	}
}

void Melee::SetAttackerTransform(TransformComponent* attacker)
{
	attackerTransform = attacker;
}

