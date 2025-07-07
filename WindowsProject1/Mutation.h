#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TransformComponent;
class BossScene;
class TextureComponent;
class AABBCollisionComponent;
class Mutation : public Object
{
	RenderInfo bgInfo{};

public:
	enum class MutationState
	{
		Mutate,
		Mutate_Hold,
		Glitch1,
		Mutate_End,
		Glitch2,
		Appear,
		Idle,
		Hurt,
		Scream
	};
	MutationState mutationState = MutationState::Mutate;

	enum class TentacleState
	{
		Idle,
		Stab,
		Attack,
		Stab_End,
	};
	TentacleState tentacleState = TentacleState::Idle;
	std::shared_ptr<SpriteManagerComponent> body;
	std::shared_ptr<SpriteManagerComponent> tentacle;
	std::shared_ptr<SpriteManagerComponent> illguy;
	std::shared_ptr<TextureComponent> headBehind;

	std::shared_ptr<TextureComponent> backgroundBehind;
	std::shared_ptr<TextureComponent> background;
	std::shared_ptr<TextureComponent> floor;
	std::shared_ptr<TextureComponent> Foreground;

	SpriteManagerComponent* sprite;

	TransformComponent* transform;

	glm::vec2 portalPos;
	BossScene* bossScene;
	bool songSwitch = false;
	double timer = 0.;
	int hp = 3;
	bool scream = false;
public:
	Mutation(ObjectManager* owner, ObjectType type, BossScene* bossScene);
	virtual void Update(double);
	virtual void Render();
	void TentacleAttack();
private:
	const glm::vec2 headPos = { 500, 400 };
	const glm::vec2 tentaclePos = { 280, 200 };
	const glm::vec2 illguyPos = { 975, 412 };
	const glm::vec2 FloorPos = { 640, 705 };
	const glm::vec2 ForegroundPos = { 640, 645 };
	const glm::vec2 BackgroundPos = { 640, 455 };
	const glm::vec2 bodyPos = { 740, 330 };
};

