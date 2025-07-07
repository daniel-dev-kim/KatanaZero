#pragma once
#include "Scene.h"
class Player;
class TitleScene : public Scene
{
    std::shared_ptr<Object> bg;
    std::shared_ptr<Object> fence;
    std::shared_ptr<Object> ZERO;
    std::shared_ptr<Object> ZER;
    std::shared_ptr<Object> O;
    std::shared_ptr<Object> KATANA;
    std::shared_ptr<Object> grass;
    std::shared_ptr<Object> plant;

    std::shared_ptr<Object> textBack;
    std::shared_ptr<Object> chooseBack;

    std::shared_ptr<Object> newGame;
    std::shared_ptr<Object> continueGame;
    std::shared_ptr<Object> setting;
    std::shared_ptr<Object> language;
    std::shared_ptr<Object> exit;

    std::shared_ptr<Object> black;

    const glm::vec2 bgStartPosition = { 640, 0 };
    const glm::vec2 fenceStartPosition = { 640, 0 };
    const glm::vec2 ZEROStartPosition = { 640, -330 };
    const glm::vec2 ZERStartPosition = { 580, -330 };
    const glm::vec2 OStartPosition = { 792, -330 };
    const glm::vec2 KATANAStartPosition = { 640, -220 };
    const glm::vec2 grassStartPosition = { 640, -660 };
    const glm::vec2 blackStartPosition = { 640, -1440 };
    const glm::vec2 plantStartPosition = { 640, -465 };
     
    const glm::vec2 textBackStartPosition = { 640, -400 };
    const glm::vec2 chooseBackStartPosition = { 640, -10000 };
     
    const glm::vec2 newGameStartPosition = { 640, -330 };
    const glm::vec2 continueGameStartPosition = { 640, -365 };
    const glm::vec2 settingStartPosition = { 640, -400 };
    const glm::vec2 languageStartPosition = { 640, -435 };
    const glm::vec2 exitStartPosition = { 640, -470 };

    int choose = 0;
    enum class TitleState
    {
        Down,
        MenuUp,
        Idle,
        Choose,
        Up
    };
    
    double downTimer = 0.0;
    double pivot = 180.0;
    double pivot2 = 0.0;

    double menuTimer = 0.0;
    double pivot3 = 0.0;
    TitleState state;

    

    bool Off = false;
    double neonTimer = 0.;
    double neonCooldown = 0.;
    double flickTimer = 0.;
    int flickCount = 0;
    double upTimer = 0.;
public:
    void Load()override;
    void Unload() override;
    void Render() override;
    void Update(double dt) override;    
    virtual void ChangeSceneState(SceneState newState) {};

};

