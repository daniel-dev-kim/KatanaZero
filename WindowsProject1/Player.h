#pragma once
#include "Object.h"
class Terrain;
class LineCollisionComponent;
class Item;
class Particle;
class Player : public Object
{
    std::string slowEffectKey = "player_slow_effect";
    std::string laserContactKey = "laser_contact";
    std::list<Afterimage> afterimages;
    std::list<Afterimage> dashAfterimages;
    glm::vec3 colorStart = glm::vec3(1.0, 0.4, 0.8); // ºÐÈ«
    glm::vec3 colorEnd = glm::vec3(0.8, 0.9, 1.0); // ÇÏ´Ã
    //float afterimageTimer = 0.0;
private:
    friend class Fanblade;
    friend class UI;
    friend class Background;
    const int totalBattery = 9;
    int currentBattery = totalBattery;
    double slowTimer = 0.;
    bool slowSkill = false;
    ItemType currItem = ItemType::None;
    double currentSlow = 1.0;
    double rollTimer = 0.4;
    bool slowSoundOn = false;
public:
	Player(ObjectManager*, ObjectType);			
    double memoryTimer = 60.;

    virtual void Update(double dt);				
    virtual void Render();
    bool willDie = false;
    bool die = false;
    bool CanMove = false;
    bool isInvincible = false;
    bool hidden = false;
    float hiddenTimer = 0.0;
    LineCollisionComponent* currentGround = nullptr;
    LineCollisionComponent* firstGround = nullptr;

private:
    Item* itemOwn = nullptr;
    bool isThroughableGround = false;

    bool didAirAttack = false;
    bool isLeftWallSlidable = false;
    bool isRightWallSlidable = false;
    bool isWallStop = false;

    class StatePlaySong : public State {
        double timer = 0;
        bool chosen = false;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateIdle : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateIdleToRun : public State {
        int dir;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateRun : public State {
        int dir;
        bool footStepSwitch;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateRunToIdle : public State {
        int dir;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateJump : public State {
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateFall : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateWallSlide : public State {
        double effectTimer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateFlip : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateHangUp : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StatePreCrouch: public State{
    public:
        void Enter(Object * obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateCrouch : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StatePostCrouch : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateRoll : public State {
        double effectTimer;
        int dir;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateAttack : public State {
        glm::vec2 dir;
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

    class StateHurtFlyBegin : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class StateHurtFly : public State {
        double timer;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class StateHurtGround : public State {
        double timer;
        double initVelocityX;
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };
    class StateHurtRecover : public State {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
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

    class StateDoorKick : public State
    {
    public:
        void Enter(Object* obj) override;
        void Update(Object* obj, double dt) override;
        void TestForExit(Object* obj) override;
    };

public:
    StatePlaySong statePlaySong;
    StateIdle stateIdle;    
    StateIdleToRun stateIdleToRun;
    StateRun stateRun;
    StateRunToIdle stateRunToIdle;
    StateJump stateJump;
    StateFall stateFall;
    StateWallSlide stateWallSlide;
    StateFlip stateFlip;
    StateHangUp stateHangUp;
    StatePreCrouch statePreCrouch;
    StateCrouch stateCrouch;
    StatePostCrouch statePostCrouch;
    StateRoll stateRoll;
    StateAttack stateAttack;
    StateHurtFlyBegin stateHurtFlyBegin;
    StateHurtFly stateHurtFly;
    StateHurtGround stateHurtGround;
    StateHurtRecover stateHurtRecover;
    StateLaserContact stateLaserContact;
    StateDoorKick stateDoorKick;
};