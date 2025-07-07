#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TransformComponent;
class BossScene;
class TextureComponent;
class AABBCollisionComponent;
class BossEye : public Object
{
public:
	enum class EyeState
	{
		Wait,
		Open,
		Stare,
		Die,
	};
	EyeState eyeState = EyeState::Wait;
	std::shared_ptr<TextureComponent> iris;
	std::shared_ptr<TextureComponent> eyeMask;
	AABBCollisionComponent* AABB;
	SpriteManagerComponent* sprite;
	TransformComponent* transform;
	BossScene* bossScene;
	glm::vec2 eyeEllipseSize = {32, 14};
	glm::vec2 irisPos;
	double timer = 0.;

public:
	BossEye(ObjectManager* owner, ObjectType type, BossScene* bossScene);
	virtual void Update(double);
	virtual void Render();
};