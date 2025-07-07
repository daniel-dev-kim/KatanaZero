#include "pch.h"
#include "Bullet.h"
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
#include "TextureManager.h"
#include "AudioManager.h"
#include "Warning.h"
#include "Terrain.h"
Bullet::Bullet(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	texture = AddComponent<TextureComponent>("Bullet", "Resource/Texture/Object/obj_bullet/bullet.png");
	transform = AddComponent<TransformComponent>();
	collision = AddComponent<AABBCollisionComponent>();
}


void Bullet::Update(double dt)
{
	auto doors = owner->GetObjectList(ObjectType::Door);
	for (auto door : doors)
	{
		auto doorAABBCollision = door->GetComponent<AABBCollisionComponent>();
		if (doorAABBCollision && doorAABBCollision->CheckCollide(this))
		{
			SetDead();
			break;
		}
	}
	auto terrainList = owner->GetObjectList(ObjectType::Terrain);
	LineCollisionComponent* hitGround = nullptr;
	for (auto terrain : terrainList)
	{
		auto targetAABBCollision = terrain->GetComponent<AABBCollisionComponent>();
		if (targetAABBCollision && targetAABBCollision->CheckCollide(this))
		{
			SetDead();
			break;
		}

		auto targetLineCollision = terrain->GetComponent<LineCollisionComponent>();

		if (targetLineCollision && !dynamic_cast<Terrain*>(terrain.get())->IsWallThroughable())
		{
			double posY = transform->GetPosition().y;
			double highestUnderItemPos = 0.;
			auto [result, y] = targetLineCollision->CheckCollide(this);
			if (!result || y > posY) continue;
			if (y > highestUnderItemPos)
			{
				highestUnderItemPos = y;
				hitGround = targetLineCollision;
			}
		}
	}

	if (hitGround)
	{
		hitGround->ResolveCollide(this);
		transform->SetRotation(0);
		velocity = { 0, 0 };
		{
			SetDead();
		}
	}

	Object::Update(dt);
	transform->Translate(velocity * (float)dt);
	if (objType == ObjectType::EBullet)
	{
		auto player = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());
		if (!player->willDie)
		{
			if (GetComponent<AABBCollisionComponent>()->CheckCollide(player))
			{
				player->willDie = true;
				player->currentGround = nullptr;
				player->SetVelocity({velocity.x, 200.f});
				player->ChangeState(&player->stateHurtFlyBegin);
				SetDead();
			}
		}
	}
	else if (objType == ObjectType::PBullet)
	{
		auto enemies = owner->GetObjectList(ObjectType::Enemy);
		for (auto e : enemies)
		{
			auto enemy = dynamic_cast<Enemy*>(e.get());
			if (!enemy->willDie)
			{
				if (GetComponent<AABBCollisionComponent>()->CheckCollide(enemy))
				{
					SetDead();
					Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
					Engine::GetInstance().GetAudioManager()->StartSound("enemy_death_bullet", true);
					//Æ¨°ÜÁø ÃÑ¾Ë¿¡ ¸Â¾ÆÁ×À¸¸é alert
					auto warning = std::make_shared<Warning>(owner, ObjectType::Neutral2, 700.f, false);
					warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
					owner->AddObject(ObjectType::Neutral2, warning);
					enemy->Hit(true);
					{ // HitEffect ±¤¼±
						auto hitEffect = std::make_shared<HitEffect>(owner, ObjectType::HitEffect);
						auto hitEffectTransform = hitEffect->GetComponent<TransformComponent>();
						hitEffectTransform->SetPosition(transform->GetPosition());
						hitEffectTransform->SetRotation(atan2(velocity.y, velocity.x));
						hitEffectTransform->SetScale(4, 2);
						owner->AddObject(ObjectType::HitEffect, hitEffect);
					}
					{
						auto enemyTransform = enemy->GetComponent <TransformComponent>();
						enemy->SetVelocity(glm::normalize(velocity) * 800.f);
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
						enemyTransform->Translate(glm::normalize(velocity) * 50.f);

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

						enemyTransform->SetScale(velocity.x < 0 ? 2 : -2, 2);
						RenderInfo info{};
						info.color = { 1.2, 0.9, 0.5, 1.0 };
						glm::vec2 dir = -glm::normalize(enemy->GetVelocity());
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
}

void Bullet::Render()
{
	Object::Render();
}

void Bullet::SetDead()
{
	Object::SetDead();
	Engine::GetInstance().GetAudioManager()->StartSound("sound_bullethit" + std::to_string(Engine::GetInstance().GetRandom()->get(1, 3)));
}
