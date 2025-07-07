#pragma once
#include "Object.h"
#include "Weapon.h"
class ObjectManager;
class HammerEnemy : public Object
{

public:
	std::shared_ptr<Weapon> m_pWeapon;

	HammerEnemy(ObjectManager*, ObjectType);

	virtual void Update(double dt);
	virtual void Render(HDC);
	virtual void SetDead();
	virtual int GetWeaponType()override;

	class HammerEnemyStart :public State
	{
	public:
		void Enter(Object* object) override;
		void Update(Object* object, double dt) override;
		void TestForExit(Object* object) override;
		double timer;
	};

	class HammerEnemyIdle :public State
	{
	public:
		void Enter(Object* object) override;
		void Update(Object* object, double dt) override;
		void TestForExit(Object* object) override;
		double timer;
	};

	class HammerEnemyMove :public State
	{
	public:
		void Enter(Object* object) override;
		void Update(Object* object, double dt) override;
		void TestForExit(Object* object) override;
		double timer;
		math::vec2 startPos;
		float speed;
	};

	class HammerEnemyAttack :public State
	{
	public:
		void Enter(Object* object) override;
		void Update(Object* object, double dt) override;
		void TestForExit(Object* object) override;
		double timer;
	};

	class HammerEnemyAttackMove :public State
	{
	public:
		void Enter(Object* object) override;
		void Update(Object* object, double dt) override;
		void TestForExit(Object* object) override;
		double timer;
		float speed;


		math::vec2  startPos;	//움직임 시작좌표
		math::vec2	dstPos;		//목적지의 좌표
		math::vec2	endPos;		//실제 이동거리
	};

	class HammerEnemyDead :public State
	{
	public:
		void Enter(Object* object) override;
		void Update(Object* object, double dt) override;
		void TestForExit(Object* object) override;
		double timer;
	};



	void		checkPlayerPos();
	void		SetLookDir(int _dir) { lookDir = _dir; }
	int			GetLookDir() { return lookDir; }
	float		GetPGap() { return playerGap; }



private:
	HammerEnemyStart		hammerenemyStart;
	HammerEnemyIdle			hammerenemyIdle;
	HammerEnemyMove			hammerenemyMove;
	HammerEnemyAttack		hammerenemyAttack;
	HammerEnemyAttackMove	hammerenemyAttackMove;
	HammerEnemyDead			hammerenemyDead;

	math::vec2	playerPos;
	math::vec2	centerPos;
	math::vec2	rightendPos;
	math::vec2	leftendPos;

	//1 = right, -1 = left
	int lookDir = 1;
	int lastDir = 1;

	float playerGap;
	float defaultDistance = 200.f;



};

