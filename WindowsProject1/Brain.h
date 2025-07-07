#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TextureComponent;
class Mutation;
class TransformComponent;
class AABBCollisionComponent;
class Player;
class Brain : public Object
{
	Player* player;
	AABBCollisionComponent* AABB;
	SpriteManagerComponent* sprite;
	TransformComponent* transform;
	enum class BrainState
	{
		PortalOpen,
		Appear,
		Idle,
		Attack,
		Die,
	};
	BrainState currentState;
	std::shared_ptr<SpriteManagerComponent> portal;
	std::shared_ptr<SpriteManagerComponent> portalCover;
	std::shared_ptr<TextureComponent> portalMask;

	double timer = 0.;
	double moveperiod;
	glm::vec2 targetPos;
	glm::vec2 portalPos;
public:
	Brain(ObjectManager* owner, ObjectType type, glm::vec2 startPos, Player*);
	virtual void Update(double);
	virtual void Render();
};