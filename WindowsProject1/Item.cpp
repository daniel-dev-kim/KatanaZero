#include "pch.h"
#include "Item.h"
#include "TextureManager.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"
#include "Engine.h"
#include "ObjectManager.h"
#include "Warning.h"
#include "Terrain.h"
#include "Particle.h"
#include "BloodEffect.h"
#include "HitEffect.h"
#include "Random.h"
#include "Enemy.h"
#include "CameraComponent.h"
#include "AudioManager.h"
#include "Smoke.h"

Item::Item(ObjectManager* owner, ObjectType objType) : Object(owner, objType)
{
	auto sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/UI/ui_pickup/pickupSpriteData.txt");
	sprite->PlaySprite("UI_Pickup_");
	status = ItemStatus::Unpicked;

	auto collision = AddComponent<AABBCollisionComponent>();
	collision->SetSize({ 24, 24 });
}

void Item::Update(double dt)
{
	if (owner->GetFrameIndex() == 0)
	{
		status = ItemStatus::Unpicked;
	}

	auto transform = GetComponent<TransformComponent>();

	if (status == ItemStatus::Thrown)
	{
		transform->Translate(velocity * (float)dt);
		transform->SetRotation((velocity.x > 0 ? --thrownItemRotate : ++thrownItemRotate) * dt * 200.);

		auto doors = owner->GetObjectList(ObjectType::Door);
		for (auto door : doors)
		{
			auto doorAABBCollision = door->GetComponent<AABBCollisionComponent>();
			if (doorAABBCollision && doorAABBCollision->CheckCollide(this))
			{
				doorAABBCollision->ResolveCollide(this);
				velocity = { 0, 0 };
				switch (type)
				{
				case ItemType::Smoke:
				{
					Engine::GetInstance().GetAudioManager()->StartSound("vial_smoke");
					SetDead();
					{
						//아이템 벽에 부딪히면 alert
						auto warning = std::make_shared<Warning>(owner, ObjectType::Neutral2, 1000.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
						owner->AddObject(ObjectType::Neutral2, warning);
					}
					glm::vec2 itemPos = transform->GetPosition();
					int cols = 7; // 가로 방향 연막 개수
					int rows = 5; // 세로 방향 연막 개수
					float cellWidth = 60.f;
					float cellHeight = 60.f;

					for (int row = 0; row < rows; ++row) {
						for (int col = 0; col < cols; ++col) {
							// 격자 기준 위치
							float x = -200.f + col * cellWidth + cellWidth / 2;  // -400/2 ~ +400/2
							float y = -150.f + row * cellHeight + cellHeight / 2; // -300/2 ~ +300/2

							// 살짝 오차를 더해 자연스럽게
							float jitterX = Engine::GetInstance().GetRandom()->get(-25.f, 25.f);
							float jitterY = Engine::GetInstance().GetRandom()->get(-25.f, 25.f);

							glm::vec2 offset = glm::vec2{ x + jitterX, y + jitterY };

							auto smoke = std::make_shared<Smoke>(owner, ObjectType::Effect, Engine::GetInstance().GetRandom()->get(-1., 0.));
							auto smokeTransform = smoke->GetComponent<TransformComponent>();
							smokeTransform->SetPosition(itemPos + offset);
							owner->AddObject(ObjectType::Effect, smoke);

						}
					}

				}break;
				case ItemType::Knife:
				{
					transform->SetRotation(0);
					{
						Engine::GetInstance().GetAudioManager()->StartSound("knife_wall", true);
						auto warning = std::make_shared<Warning>(owner, ObjectType::Effect, 500.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
						owner->AddObject(ObjectType::Effect, warning);
					}
					status = ItemStatus::Unpicked;
				}break;
				}
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
				targetAABBCollision->ResolveCollide(this);
				velocity = { 0, 0 };
				switch (type)
				{
				case ItemType::Smoke:
				{
					SetDead();
					Engine::GetInstance().GetAudioManager()->StartSound("vial_smoke");
					{
						//아이템 땅에 부딪히면 alert
						auto warning = std::make_shared<Warning>(owner, ObjectType::Neutral2, 1000.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition() + glm::vec2{0.f, 50.f});
						owner->AddObject(ObjectType::Neutral2, warning);
					}
					glm::vec2 itemPos = transform->GetPosition();
					int cols = 7; // 가로 방향 연막 개수
					int rows = 5; // 세로 방향 연막 개수
					float cellWidth = 60.f;
					float cellHeight = 60.f;


					for (int row = 0; row < rows; ++row) {
						for (int col = 0; col < cols; ++col) {
							// 격자 기준 위치
							float x = -200.f + col * cellWidth + cellWidth / 2;  // -400/2 ~ +400/2
							float y = -150.f + row * cellHeight + cellHeight / 2; // -300/2 ~ +300/2

							// 살짝 오차를 더해 자연스럽게
							float jitterX = Engine::GetInstance().GetRandom()->get(-25.f, 25.f);
							float jitterY = Engine::GetInstance().GetRandom()->get(-25.f, 25.f);

							glm::vec2 offset = glm::vec2{ x + jitterX, y + jitterY };

							auto smoke = std::make_shared<Smoke>(owner, ObjectType::Effect, Engine::GetInstance().GetRandom()->get(-1., 0.));
							auto smokeTransform = smoke->GetComponent<TransformComponent>();
							smokeTransform->SetPosition(itemPos + offset);
							owner->AddObject(ObjectType::Effect, smoke);

						}
					}


				}break;
				case ItemType::Knife:
				{
					transform->SetRotation(0);
					{
						Engine::GetInstance().GetAudioManager()->StartSound("knife_wall", true);
						auto warning = std::make_shared<Warning>(owner, ObjectType::Effect, 500.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
						owner->AddObject(ObjectType::Effect, warning);
					}
					status = ItemStatus::Unpicked;
				}break;
				}
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

		if (status == ItemStatus::Thrown && hitGround)
		{
			hitGround->ResolveCollide(this);
			
			velocity = { 0, 0 };
			switch (type)
			{
				case ItemType::Smoke:
				{
					SetDead();
					{
						//아이템 벽에 부딪히면 alert
						auto warning = std::make_shared<Warning>(owner, ObjectType::Neutral2, 2000.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
						owner->AddObject(ObjectType::Neutral2, warning);
					}

					glm::vec2 itemPos = transform->GetPosition();
					int cols = 7; // 가로 방향 연막 개수
					int rows = 5; // 세로 방향 연막 개수
					float cellWidth = 60.f;
					float cellHeight = 60.f;


					for (int row = 0; row < rows; ++row) {
						for (int col = 0; col < cols; ++col) {
							// 격자 기준 위치
							float x = -200.f + col * cellWidth + cellWidth / 2;  // -400/2 ~ +400/2
							float y = -150.f + row * cellHeight + cellHeight / 2; // -300/2 ~ +300/2

							// 살짝 오차를 더해 자연스럽게
							float jitterX = Engine::GetInstance().GetRandom()->get(-25.f, 25.f);
							float jitterY = Engine::GetInstance().GetRandom()->get(-25.f, 25.f);

							glm::vec2 offset = glm::vec2{ x + jitterX, y + jitterY };

							auto smoke = std::make_shared<Smoke>(owner, ObjectType::Effect, Engine::GetInstance().GetRandom()->get(-1., 0.));
							auto smokeTransform = smoke->GetComponent<TransformComponent>();
							smokeTransform->SetPosition(itemPos + offset);
							owner->AddObject(ObjectType::Effect, smoke);

						}
					}
				}break;
				case ItemType::Knife:
				{
					transform->SetRotation(0);
					{
						Engine::GetInstance().GetAudioManager()->StartSound("knife_wall", true);

						auto warning = std::make_shared<Warning>(owner, ObjectType::Neutral2, 500.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
						owner->AddObject(ObjectType::Neutral2, warning);
					}
					status = ItemStatus::Unpicked;
				} break;
			}
		}

		auto enemyList = owner->GetObjectList(ObjectType::Enemy);
		for (auto e : enemyList)
		{
			auto targetCollision = e->GetComponent<AABBCollisionComponent>();
			auto enemy = dynamic_cast<Enemy*>(e.get());

			if (targetCollision->CheckCollide(this) && !enemy->willDie)
			{
				switch (type)
				{
					case ItemType::Knife:
					{
						SetDead();
						Engine::GetInstance().playerCam->StartShake(SHAKE_DUR, SHAKE_MAG);
						Engine::GetInstance().GetAudioManager()->StartSound("enemy_death_knife", true);
						enemy->Hit(true);
						//아이템에 죽으면 
						auto warning = std::make_shared<Warning>(owner, ObjectType::Neutral2, 700.f, false);
						warning->GetComponent<TransformComponent>()->SetPosition(GetComponent<TransformComponent>()->GetPosition());
						owner->AddObject(ObjectType::Neutral2, warning);
						{ // HitEffect 광선
							auto dir = glm::normalize(velocity);
							auto hitEffect = std::make_shared<HitEffect>(owner, ObjectType::HitEffect);
							auto hitEffectTransform = hitEffect->GetComponent<TransformComponent>();
							hitEffectTransform->SetPosition(transform->GetPosition());
							hitEffectTransform->SetRotation(atan2(dir.y, dir.x));
							hitEffectTransform->SetScale(4, 2);
							owner->AddObject(ObjectType::HitEffect, hitEffect);
						}
						{
							auto enemyTransform = enemy->GetComponent<TransformComponent>();
							{ // 벽에 피
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
							auto dir = glm::normalize(velocity);
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
						} break;
					}
				}
				break;
			}
		}
	}


	//if (status == ItemStatus::Thrown)
	//{
	//	auto texture = GetComponent<TextureComponent>();
	//	auto transform = GetComponent<TransformComponent>();
	//	Afterimage afterIage{};
	//	afterIage.modelMat = transform->GetModelMatrix();
	//	afterIage.texture = Engine::GetInstance().GetTextureManager()->GetTexture(texture->GetKey());
	//	afterIage.lifetime = 0.2;
	//	afterIage.info.color = glm::vec4(1.0, 1.0, 1.0, 0.2);
	//	afterimages.push_back(afterIage);

	//}
	//else
	//{
	//	afterimages.clear();
	//}


	for (auto& afterimage : afterimages)
	{
		afterimage.lifetime -= dt;
		afterimage.info.color.a -= dt * 0.5;
	}

	afterimages.remove_if([](const auto& img) { return img.lifetime <= 0.0f; });
}

void Item::Render()
{
	if (status != ItemStatus::Picked)
	{
		auto texture = GetComponent<TextureComponent>();
		if (texture)
		{
			texture->Render(GetComponent<TransformComponent>()->GetModelMatrix());
		}
	}
	if (status == ItemStatus::Unpicked)
	{
		timer++;
		auto transform = GetComponent<TransformComponent>();
		double gap = 50 + 10. * sin(timer * 0.04);
		
		glm::vec2 originalScale = transform->GetScale();
		glm::vec2 originalTranslate = transform->GetPosition();
		
		transform->Translate({ 0., originalScale.y > 0. ? gap : -gap });
		transform->ScaleBy({ 2, 2 });
		
		auto sprite = GetComponent<SpriteManagerComponent>();
		if (sprite)
		{
			sprite->Render();
		}

		transform->SetScale(originalScale);
		transform->SetPosition(originalTranslate);
	}

	if (status == ItemStatus::Thrown)
	{
		for (auto& afterimage : afterimages)
		{
			afterimage.Render(Engine::GetInstance().GetShaderProgram(), Engine::GetInstance().playerCam);
		}
	}

}

void Item::SetStatus(ItemStatus s)
{
	thrownItemRotate = 0;
	status = s;
}

ItemStatus Item::GetStatus() const
{
	return status;
}

void Item::SetItemType(ItemType t)
{
	type = t;
}

ItemType Item::GetItemType() const
{
	return type;
}

