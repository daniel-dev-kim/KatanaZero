#pragma once
#include "Object.h"
class TextureComponent;
class TransformComponent;
class Boss;
class BossThunder : public Object
{
	TextureComponent* texture;
	TransformComponent* transform;
	double alpha;
public:
	BossThunder(ObjectManager* owner, ObjectType type);
	virtual void Update(double);
	virtual void Render();
};