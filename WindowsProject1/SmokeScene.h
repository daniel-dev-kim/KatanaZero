#pragma once
#include "Scene.h"
class Player;
class YouCanDoThis;
class Transition;
class SmokeScene : public Scene
{
    std::shared_ptr<Player> player;
    bool transitionSoundSwitch = false;
    double startTimer = 0;
    std::shared_ptr<YouCanDoThis> youCanDoThis;
    std::shared_ptr<Transition> lastTransition;

public:
    void Load() override;
    void Unload() override;
    void Render() override;
    void Update(double dt) override;
    void ChangeSceneState(SceneState newState) override;
};

