#pragma once
#include "Enemy.h"
class TextureComponent;
class Gangster : public Enemy
{
    std::shared_ptr<Object> gunPart;
    TextureComponent* gunPartTexture;
    std::string gunsmoke;
    std::string gunspark;

public:
    State* initState;
    Gangster(ObjectManager*, ObjectType);
    void Update(double dt) final;
    void Render() final;
private:
    class GangsterStateIdle : public StateIdle {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GangsterStateWalk : public StateWalk {
        int dir;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GangsterStateAlert : public StateAlert { // 경계 상태
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GangsterStateSuspicious : public StateSuspicious { // 의문 상태
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GangsterStateChase : public StateChase { // 발견 상태
        bool chaseFirst = false;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GangsterStateAttack : public StateAttack { // 공격
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GangsterStateTurn : public StateTurn {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class GangsterStateHurtFly : public StateHurtFly
    {
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class GangsterStateHurtGround : public StateHurtGround
    {
        bool bloodSwitch;
        double initVelocityX;
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    glm::vec2 armPosOffset = { 9, 10 };

    GangsterStateIdle gangsterStateIdle;
    GangsterStateWalk gangsterStateWalk;
    GangsterStateAlert gangsterStateAlert;
    GangsterStateSuspicious gangsterStateSuspicious;
    GangsterStateChase gangsterStateChase;
    GangsterStateAttack gangsterStateAttack;
    GangsterStateTurn gangsterStateTurn;
    GangsterStateHurtFly gangsterStateHurtFly;
    GangsterStateHurtGround gangsterStateHurtGround;
    //GangsterState
};