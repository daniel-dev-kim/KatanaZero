#pragma once
#include "Object.h"
class TextureComponent;
class Mutation;
class Player;
class TransformComponent;
class AABBCollisionComponent;
class Stab : public Object
{
public:
	AABBCollisionComponent* AABB;
	TransformComponent* transform;
	TextureComponent* stab;
	enum class StabState
	{
		Hide,
		Aim,
		Attack,
		Back,
	};
	StabState currentState;

	Player* player;
	Mutation* mutation;
	int attackCount = 4;
public:
	Stab(ObjectManager* owner, ObjectType type, Mutation* mutation, Player* player);
	virtual void Update(double);
	virtual void Render();
	void StartAttack();
};