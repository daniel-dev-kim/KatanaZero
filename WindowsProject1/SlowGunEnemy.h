#pragma once
#include "Object.h"
#include "Weapon.h"

class SlowGunEnemy :
    public Object
{
private:
    std::shared_ptr<Weapon> pWeapon;
public:
    SlowGunEnemy(ObjectManager*, ObjectType);
    ~SlowGunEnemy() {}

    void Update(double dt);
    void Render(HDC hdc);
    void SetDead();

    virtual int GetWeaponType()override;

    class SlowGunEnemyIdle :public State
    {
    public:
        void Enter(Object* object) override;
        void Update(Object* object, double dt) override;
        void TestForExit(Object* object) override;
        double timer;
        double distance;
        static constexpr double DETECTION_RANGE = 400.;
        static constexpr double ATTACK_RANGE = 250.;
        static constexpr double ATTACK_COOLDOWN = 1.;
    };
    class SlowGunEnemyMove :public State
    {
    public:
        void Enter(Object* object) override;
        void Update(Object* object, double dt) override;
        void TestForExit(Object* object) override;
        double timer;
        float direction;
        float speed;
    };
    class SlowGunEnemyAttack : public State
    {
    public:
        void Enter(Object* object) override;
        void Update(Object* object, double dt) override;
        void TestForExit(Object* object) override;
        double timer;
        float direction;
        math::vec2 targetPos;
    };
private:
    SlowGunEnemyIdle slowgunEnemyIdle;
    SlowGunEnemyMove slowgunEnemyMove;
    SlowGunEnemyAttack slowgunEnemyAttack;
};

