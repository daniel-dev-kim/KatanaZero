#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TextureComponent;
class BossIntro : public Object
{
	std::list<Afterimage> afterimages;
	double afterimageTimer;
	friend class Boss;
	enum class HandIntroState
	{
		Wait,
		PortalOpen,
		HandCommingOut,
		HandCracked,
		HandGoingBack,
		PortalClose
	};
	HandIntroState currState;
	std::shared_ptr<SpriteManagerComponent> portal;
	std::shared_ptr<SpriteManagerComponent> portalCover;
	std::shared_ptr<TextureComponent> portalMask;
	std::shared_ptr<SpriteManagerComponent> hand;
	double timer = 0.;
	glm::vec2 handPos;
	glm::vec2 portalPos;
	float handLength = 136.f;

	int glitch = 0;
	float seed = 0.0f;
	bool glitchSwitch = false;
public:
	BossIntro(ObjectManager* owner, ObjectType type, glm::vec2);
	virtual void Update(double);
	virtual void Render();
};