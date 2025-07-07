#pragma once
#include "Scene.h"
class Player;
class YouCanDoThis;
class FactoryScene : public Scene
{
    std::shared_ptr<Player> player;
    bool transitionSoundSwitch = false;
    double clickToStartTimer = 0.;
    std::shared_ptr<YouCanDoThis> youCanDoThis;
public:
    void Load()override;
    void Unload() override;
    void Render() override;
    void Update(double dt) override;
    void ChangeSceneState(SceneState newState) override;
};

