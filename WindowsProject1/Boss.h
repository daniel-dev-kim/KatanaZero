#pragma once
#include "Object.h"
class SpriteManagerComponent;
class AABBCollisionComponent;
class TransformComponent;
class BossSpark;
class BossAura;
class BossScene;
class BossArm;
class Boss : public Object
{
	std::list<Afterimage> afterimages;
public:
	enum class BossState
	{
		Idle,
		Glasses,
		Float_Begin,
		Float_Up_Slow,
		Float_Up_Fast,
		Float_End,
		ReadyToFight,
		ShowCaseHands,
		Fight,
		KillPlayer,
		ArmAttacked,
		Disappear,
		Count
	};
	BossArm* grabArm;
	BossState currState;
	SpriteManagerComponent* sprite;
	AABBCollisionComponent* AABB;
	TransformComponent* transform;
	BossScene* bossScene;
	bool secondThunder = false;
	glm::vec2 floatStart;
	glm::vec2 floatEnd = {660, 550};
	float floatTimer = 0.;
	float radiusX = 3.0f;
	float radiusY = 0.0f;
	int randomPattern = 0;
	std::shared_ptr<BossSpark> spark;
	std::shared_ptr<BossAura> aura;
	double floatBeginTimer = 0.;
	int hp = 12;
	glm::vec2 killPlayerPos;
	double afterimageTimer = 0.;
	bool killPlayerSwitch = false;
public:
	Boss(ObjectManager* owner, ObjectType type, BossScene* bossScene);
	
	virtual void Update(double dt);
	virtual void Render();
	void ChangeState(BossState newState);
};