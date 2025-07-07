#pragma once
#include "Object.h"
class TransformComponent;
class Melee : public Object
{
	TransformComponent* attackerTransform;
	glm::vec2 offset;
public:
	Melee(ObjectManager*, ObjectType);
	virtual void Update(double);
	void SetAttackerTransform(TransformComponent* attacker);
};