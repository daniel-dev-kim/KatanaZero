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
    class GruntStateAlert : public StateAlert { // ��� ����
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateSuspicious : public StateSuspicious { // �ǹ� ����
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateChase : public StateChase { // �߰� ����
        bool chaseFirst = false;
        double attackCool = 0.0;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GruntStateAttack : public StateAttack { // ����
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