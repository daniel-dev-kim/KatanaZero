#pragma once
#include "Object.h"
class ObjectManager;
class SpriteManagerComponent;
class AABBCollisionComponent;
class TransformComponent;
class Smoke : public Object
{
	enum class SmokeState
	{
		Wait,
		Appear,
		Loop,
		Dissolve
	};
	SpriteManagerComponent* sprite;
	AABBCollisionComponent* AABB;
	TransformComponent* transform;
	SmokeState currState;

	glm::vec3 color;
	double timer;
	double dieTime;
public:
	Smoke(ObjectManager*, ObjectType objType, double);
	void Update(double dt) final;
	void Render() final;
};