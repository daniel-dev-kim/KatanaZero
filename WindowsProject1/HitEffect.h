#pragma once
#include "Particle.h"
class TextureComponent;
class TextureData;
class TransformComponent;
class HitEffect : public Particle
{
	TextureComponent* texture1;
	TextureComponent* texture2;

	TransformComponent* transform;
	int i = 0;
	int toggleCount;
	bool toggle = true;
public:
	HitEffect(ObjectManager*, ObjectType objType);
	void Update(double) final;
	void Render() final;

};