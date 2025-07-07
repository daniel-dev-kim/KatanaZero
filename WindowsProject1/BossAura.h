#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TransformComponent;
class Boss;
class BossAura : public Object
{
	SpriteManagerComponent* sprite;
	TransformComponent* transform;
	Boss* boss;
	double alpha = 0.0;
public:
	BossAura(ObjectManager* owner, ObjectType type, Boss* boss);
	virtual void Update(double);
	virtual void Render();
};