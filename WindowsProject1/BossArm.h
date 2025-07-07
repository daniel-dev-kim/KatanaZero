#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TextureComponent;
class TransformComponent;
class AABBCollisionComponent;
class Boss;
class BossArm : public Object
{
	friend class Boss;
	enum class ArmState
	{
		Wait,
		PortalOpen,
		OpenWait,
		HandCommingOut,
		HandHit,
		HandGrab,
		HandExplode,
		Glitch,
		HandGoal,
		HandGoingBack,
		PortalClose
	};
	TransformComponent* transform;
	AABBCollisionComponent* AABB;
	ArmState currState = ArmState::Wait;
	std::shared_ptr<SpriteManagerComponent> portal;
	std::shared_ptr<SpriteManagerComponent> portalCover;
	std::shared_ptr<SpriteManagerComponent> hand;
	std::shared_ptr<TextureComponent> wrist;
	std::vector<std::shared_ptr<TextureComponent>> arms;
	double timer = 0.;
	double timerWhenHit = 0.0;
	double armStart = 0.0;
	Boss* boss = nullptr;
	int armType = 0;
	int handIndex = 0;
	glm::vec2 portalPos;
	std::vector<std::pair<glm::vec2, glm::vec2>> curvInfo;
	bool hit = false;
public:
	BossArm(ObjectManager* owner, ObjectType type, int armType, Boss*);
	virtual void Update(double);
	virtual void Render();

	void MakeCurve(int armType);

};


