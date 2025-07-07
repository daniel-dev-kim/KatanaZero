#pragma once
#include "Object.h"

class SpriteManagerComponent;
class TransformComponent;
class AABBCollisionComponent;
class BossFire : public Object
{
	AABBCollisionComponent* AABB;
	SpriteManagerComponent* sprite;
	TransformComponent* transform;
public:
	BossFire(ObjectManager* owner, ObjectType type);
	virtual void Update(double);
	void Crash();
	bool reflected = false;
};