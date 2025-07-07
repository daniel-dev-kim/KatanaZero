#pragma once
#include "Object.h"
class TextureComponent;
class TransformComponent;
class AABBCollisionComponent;
class Bullet : public Object
{
public:
	Bullet(ObjectManager*, ObjectType);
	void Update(double) final;
	void Render() final;
	void SetDead() final;
	AABBCollisionComponent* collision;
	TransformComponent* transform;
	TextureComponent* texture;
};