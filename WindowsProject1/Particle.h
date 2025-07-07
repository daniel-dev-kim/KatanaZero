#pragma once
#include "Object.h"
class ObjectManager;
class TransformComponent;
class Particle : public Object
{
protected:
	double deadTime = 0.;
	double timer = 0.;
	bool isDeadBySpriteEnd = false;
	bool isGettingDisappearByTime = false;
	float gravityFactor = 0.;
	bool ignoreSlow = false;
	RenderInfo info;
	bool hasOwner = false;
	TransformComponent* ownerTransform;
	glm::vec2 ownerOffset;
public:	
	Particle(ObjectManager* owner, ObjectType objType);

	void SetVelocity(glm::vec2 v);
	void SetTimer(double timer);
	void SetStartTimer(double timer);
	void SetIsDeadBySpriteEnd(bool);
	void SetIsGettingDisappearByTime(bool);
	void SetGravityFactor(float);
	void SetIgnoreSlow(bool);
	void Update(double) override;
	void Render() override;
	void SetRenderInfo(RenderInfo i);
	void SetHasOwner(bool, TransformComponent* , glm::vec2);
};