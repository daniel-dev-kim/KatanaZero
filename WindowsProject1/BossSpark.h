#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TransformComponent;
class Boss;
class BossSpark : public Object
{
	SpriteManagerComponent* sprite;
	TransformComponent* transform;
	Boss* boss;
public:
	BossSpark(ObjectManager* owner, ObjectType type, Boss* boss);
	virtual void Update(double);
};