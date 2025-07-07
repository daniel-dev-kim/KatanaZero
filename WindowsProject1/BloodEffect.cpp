#include "pch.h"
#include "BloodEffect.h"
#include "Engine.h"
#include "Random.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "ObjectManager.h"
#include "AABBCollisionComponent.h"
BloodEffect::BloodEffect(ObjectManager* owner, ObjectType objType) : Particle(owner, objType)
{
	deadTime = 0.1 * level;
	gravityFactor = 0.3;
	info.color = glm::vec4(glm::vec3(Engine::GetInstance().GetRandom()->get(0.6f, 1.0f)), 1.f);
}

void BloodEffect::Update(double dt)
{
	if (isGettingDisappearByTime)
	{
		info.color.a -= dt;
	}
	if (objType != ObjectType::BackgroundEffect)
	{
		auto transform = GetComponent<TransformComponent>();
		Object::Update(dt);
		timer += dt;
		velocity.y -= GRAVITY * gravityFactor * dt;
		transform->Translate(velocity * (float)dt);

		if (timer > deadTime)
		{
			if (level > 2)
			{
				for (int i = 0; i < 3; i++)
				{
					auto blood = std::make_shared<BloodEffect>(owner, (ObjectType)((int)objType - 1));
					blood->level = level - 1;
					blood->SetIsGettingDisappearByTime(true);
					int textureNumber = Engine::GetInstance().GetRandom()->get(0, 2);
					blood->AddComponent<TextureComponent>("blood_" + std::to_string(textureNumber), "Resource/Texture/Blood/" + std::to_string(textureNumber) + ".png");
					blood->velocity = glm::rotate(velocity, (float)(-PI / 6.f + PI / 6.f * i));
					auto bloodTransform = blood->GetComponent<TransformComponent>();
					bloodTransform->SetRotation(atan2(blood->velocity.y, blood->velocity.x));
					bloodTransform->SetPosition(transform->position);
					bloodTransform->SetScale(transform->GetScale() * 0.3f);
					owner->AddObject((ObjectType)((int)objType - 1), blood);

				}

			}
			
			if (level != 3 && Engine::GetInstance().GetRandom()->get(0, 3) == 0)
			{
				auto blood = std::make_shared<BloodEffect>(owner, ObjectType::BackgroundEffect);
				blood->level = 0;
				auto texture = blood->AddComponent<TextureComponent>(GetComponent<TextureComponent>()->GetKey());
				auto AABB = blood->AddComponent<AABBCollisionComponent>();
				AABB->SetSize(texture->GetSize() / 2.f);
				auto bloodTransform = blood->GetComponent<TransformComponent>();
				bloodTransform->SetPosition(transform->GetPosition());
				bloodTransform->SetRotation(transform->GetRotation());
				bloodTransform->SetScale(transform->GetScale());
				blood->info.color = { 0.5, 0.5, 0.5, 0.8 };
				owner->AddObject(ObjectType::BackgroundEffect, blood);


			}
			SetDead();
		}
	}
	else
	{
		auto terrains = owner->GetObjectList(ObjectType::Terrain);
		for (auto terrain : terrains)
		{
			auto AABB = terrain->GetComponent<AABBCollisionComponent>();
			if (AABB && AABB->CheckCollide(this))
			{
				SetDead();
			}
		}
	}
}
