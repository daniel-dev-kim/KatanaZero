#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TextureComponent;
class Mutation;
class TransformComponent;
class AABBCollisionComponent;
class Head : public Object
{
	AABBCollisionComponent* AABB;
	SpriteManagerComponent* sprite;
	TransformComponent* transform;
	enum class HeadState
	{
		PortalOpen,
		Appear,
		Idle,
		Attack,
		Hurt,
		Disappear,
		PortalClose
	};
	HeadState currentState;
	std::shared_ptr<SpriteManagerComponent> portal;
	std::shared_ptr<SpriteManagerComponent> portalCover;
	std::shared_ptr<TextureComponent> portalMask;

	double timer = 0.;
	glm::vec2 portalPos;
	Mutation* mutation;
public:
	Head(ObjectManager* owner, ObjectType type, Mutation*, glm::vec2);
	virtual void Update(double);
	virtual void Render();
};