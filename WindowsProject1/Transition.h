#pragma once
#include "Particle.h"
class SpriteManagerComponent;
class Transition : public Particle
{
	SpriteManagerComponent* sprite;
	
public:
	Transition(ObjectManager*, ObjectType objType, bool);

	void Update(double) final;
};