#pragma once
#include "Particle.h"
class ObjectManager;
class BloodEffect : public Particle
{
	int level = 3;
public:
	BloodEffect(ObjectManager*, ObjectType objType);
	void Update(double dt) final;
};