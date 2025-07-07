#pragma once
#include "Enemy.h"
class Grunt : public Enemy
{
public:
    State* initState;
	Grunt(ObjectManager*, ObjectType);
    void Update(double dt) final;
private:
    class GruntStateIdle : public StateIdle {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateWalk : public StateWalk {
        int dir;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateAlert : public StateAlert { // 경계 상태
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateSuspicious : public StateSuspicious { // 의문 상태
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateChase : public StateChase { // 발견 상태
        bool chaseFirst = false;
        double attackCool = 0.0;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateAttack : public StateAttack { // 공격
        bool attack = false;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateTurn : public StateTurn { 
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class GruntStateHurtFly : public StateHurtFly
    {
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class GruntStateHurtGround : public StateHurtGround
    {
        bool bloodSwitch;
        double initVelocityX;
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };


    GruntStateIdle gruntStateIdle;
    GruntStateWalk gruntStateWalk;
    GruntStateAlert gruntStateAlert;
    GruntStateSuspicious gruntStateSuspicious;
    GruntStateChase gruntStateChase;
    GruntStateAttack gruntStateAttack;
    GruntStateTurn gruntStateTurn;
    GruntStateHurtFly gruntStateHurtFly;
    GruntStateHurtGround gruntStateHurtGround;
    //GruntState
};