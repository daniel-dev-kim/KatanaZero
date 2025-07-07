#include "pch.h"
#include "HitEffect.h"

#include "TextureComponent.h"
#include "TransformComponent.h"

#include "Engine.h"
#include "Random.h"
HitEffect::HitEffect(ObjectManager*, ObjectType objType) : Particle(owner, objType)
{
	texture1 = AddComponent<TextureComponent>("HitEffect_Blue", "Resource/Texture/Effect/Hit_effect_blue.png");
	texture2 = AddComponent<TextureComponent>("HitEffect_Pink", "Resource/Texture/Effect/Hit_effect_pink.png");
	transform = GetComponent<TransformComponent>();
	deadTime = 0.1;
	toggleCount = Engine::GetInstance().GetRandom()->get(4, 10);
}

void HitEffect::Update(double dt)
{
	Particle::Update(dt / Engine::GetInstance().dtFactor);
}

void HitEffect::Render()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	RenderInfo info{};
	info.timer = timer * 10;
	info.useGlowEffect = true;
	i++;
	if (i % toggleCount == 0)
	{
		toggle = !toggle;
	}
	if(toggle) texture1->Render(transform->GetModelMatrix(), info);
	else texture2->Render(transform->GetModelMatrix(), info);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
