#pragma once
#include "Object.h"
class TransformComponent;
class LaserBody;
class LaserBeam : public Object
{
	LaserBody* body;
	bool laserSwitch = true;
	double contactTimer = 0.;
public:

	LaserBeam(ObjectManager* owner, ObjectType objType);
	virtual void Update(double dt);
	virtual void Render();
};