#pragma once
#include "Object.h"
class ObjectManager;
class SpriteManagerComponent;
class TransformComponent;
class BossDead : public Object
{
public:
	SpriteManagerComponent* sprite;
	TransformComponent* transform;
	BossDead(ObjectManager*, ObjectType objType);
};