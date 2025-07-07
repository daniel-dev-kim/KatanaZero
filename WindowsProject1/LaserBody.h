#pragma once
#include "Object.h"

class TransformComponent;
class TextureComponent;
class LaserBody : public Object
{
	friend class LaserBeam;
	TransformComponent* transform;
	std::shared_ptr<TextureComponent> off;
	std::shared_ptr<TextureComponent> on;
	bool laserSwitch = true;
public:

	LaserBody(ObjectManager* owner, ObjectType objType);

	virtual void Render();
};