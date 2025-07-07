#include "pch.h"
#include "Transition.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"

Transition::Transition(ObjectManager*, ObjectType objType, bool isFadeIn) : Particle(owner, objType)
{
	sprite = AddComponent<SpriteManagerComponent>("Resource/Texture/Effect/spr_transition/TransitionEffectSpriteData.txt");
	if(isFadeIn) sprite->PlaySprite("Fade_In_");
	else sprite->PlaySprite("Fade_Out_");
	GetComponent<TransformComponent>()->SetScale(2, 2);
}

void Transition::Update(double dt)
{
	Particle::Update(dt);
}
