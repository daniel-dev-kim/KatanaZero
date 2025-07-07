#pragma once
#include "Object.h"
class Player;
class SpriteManagerComponent;
class LineCollisionComponent;
class EnemyMark;
class Enemy : public Object
{
    friend class Warning;
    friend class Smoke;
    std::string laserContactKey = "laser_contact";
protected:
    Player* p;
    glm::vec2 alertedPosition;

    bool footStepSwitch = false;
    
    std::shared_ptr<EnemyMark> myMark;
    bool wallStop = false;
public:
	Enemy(ObjectManager*, ObjectType);
    virtual void Update(double);
    virtual void Render();
    bool willDie = false;
    bool inSmoke = false;
    float smokeTimer = 0.;
    LineCollisionComponent* currentGround = nullptr;

    virtual void MakeAlerted(glm::vec2) {};
protected:
    bool CanSeePlayer();
private:
    bool LineCasting(glm::vec2 start, glm::vec2 dir, float length);
protected:
    class StateIdle : public State {
    public:
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateWalk : public State {
    public:
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateAlert : public State { // 경계 상태
    public:
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateSuspicious : public State { // 의문 상태
    public:
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateChase : public State { // 발견 상태
    public:
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateAttack : public State { // 공격
    public:
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateTurn : public State
    {
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateHurtFly : public State
    {
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateHurtGround : public State
    {
        void Enter(Object* obj) override {}
        void Update(Object* obj, double dt) override {}
        void TestForExit(Object* obj) override {}
    };

    class StateLaserContact : public State
    {
        double timer;
        double sparkHeight;
    public:
        double GetTimer();
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
        void SetSparkHeight(float h);
    };


public:
    StateIdle* stateIdle;
    StateWalk* stateWalk;
    StateAlert* stateAlert;
    StateSuspicious* stateSuspicious;
    StateChase* stateChase;
    StateAttack* stateAttack;
    StateTurn* stateTurn;
    StateHurtFly* stateHurtfly;
    StateHurtGround* stateHurtground;

    StateLaserContact stateLaserContact;
};