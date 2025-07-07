#pragma once
#include "Enemy.h"
class SpriteManagerComponent;
class TextureComponent;
class Shotgun : public Enemy
{
    std::shared_ptr<Object> gunPart;
    SpriteManagerComponent* gunPartSprite;
    std::string gunsmoke;
    std::string gunspark;
public:
    State* initState;
    Shotgun(ObjectManager*, ObjectType);
    void Update(double dt) final;
    void Render() final;
    virtual void SetDead() final;
private:
    class ShotgunStateIdle : public StateIdle {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class ShotgunStateWalk : public StateWalk {
        int dir;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class ShotgunStateAlert : public StateAlert { // ��� ����
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class ShotgunStateSuspicious : public StateSuspicious { // �ǹ� ����
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class ShotgunStateChase : public StateChase { // �߰� ����
        bool chaseFirst = false;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class ShotgunStateAttack : public StateAttack { // ����
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class ShotgunStateTurn : public StateTurn {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class ShotgunStateHurtFly : public StateHurtFly
    {
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class ShotgunStateHurtGround : public StateHurtGround
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
    ShotgunStateIdle shotgunStateIdle;
    ShotgunStateWalk shotgunStateWalk;
    ShotgunStateAlert shotgunStateAlert;
    ShotgunStateSuspicious shotgunStateSuspicious;
    ShotgunStateChase shotgunStateChase;
    ShotgunStateAttack shotgunStateAttack;
    ShotgunStateTurn shotgunStateTurn;
    ShotgunStateHurtFly shotgunStateHurtFly;
    ShotgunStateHurtGround shotgunStateHurtGround;
    //ShotgunState
};