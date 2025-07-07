#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TextureComponent;
class Mutation;
class TransformComponent;
class AABBCollisionComponent;

class Injection : public Object
{
public:
	AABBCollisionComponent* AABB;
	TextureComponent* injection;
	TransformComponent* transform;
	enum class InjectionState
	{
		Wait,
		PortalOpen,
		Appear,
		Back,
		Shot,
	};

	InjectionState currentState;
	std::shared_ptr<SpriteManagerComponent> portal;
	std::shared_ptr<SpriteManagerComponent> portalCover;
	std::shared_ptr<TextureComponent> portalMask;

	double timer = 0.;
	float angle;
	glm::vec2 dir;
	glm::vec2 portalPos;
public:
	Injection(ObjectManager* owner, ObjectType type, glm::vec2 startPos, float angle);
	virtual void Update(double);
	virtual void Render();
};