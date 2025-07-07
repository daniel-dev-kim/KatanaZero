#pragma once
#include "Object.h"
class SpriteManagerComponent;
class AABBCollisionComponent;
class TransformComponent;
class BossScene;
class BossDoppleGanger : public Object
{
	std::list<Afterimage> afterimages;
public:
	enum class BossState
	{
		Glasses,
		Float,
		Appear,
		Charge,
		Fire,
		Disappear,
		FakeDie,
		Die
	};
	bool didAttack = false;
	BossState currState;
	SpriteManagerComponent* sprite;
	AABBCollisionComponent* AABB;
	TransformComponent* transform;
	BossScene* bossScene;
	bool secondThunder = false;
	bool darkCapeSound = false;
	bool alive = true;
	double afterimageTimer = 0.;

	bool moveSwitch = false;
	double moveTimer = 0.;
	glm::vec2 goal;
	glm::vec2 dir;

	double floatTimer = 0.;
public:
	BossDoppleGanger(ObjectManager* owner, ObjectType type, BossScene* bossScene);

	virtual void Update(double dt);
	virtual void Render();
	void ChangeState(BossState newState);
	void SetGoal(glm::vec2);
};