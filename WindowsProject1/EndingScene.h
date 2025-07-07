#pragma once
#include "Scene.h"
class Player;
class Background;
class BossDead;
class EndingScene : public Scene
{
    enum class EndingState
    {
        Sit,
        Stand,
        Walk,
        BringSwordBack,
        Wait,
        WalkBack,
        Wait2,
        Dance
    };
    EndingState currState;
    std::shared_ptr<Player> player;
    std::shared_ptr<BossDead> bossDead;
    std::shared_ptr<Background> ending;
    bool footstep = false;
    double timer;
public:
    void Load()override;
    void Unload() override;
    void Render() override;
    void Update(double dt) override;
    void ChangeSceneState(SceneState newState) override {};
};

