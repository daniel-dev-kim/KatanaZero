#pragma once
#include "Object.h"

class TextureComponent;
class Player;
class UI : public Object
{
public:
    Player* p;
public:
    UI(ObjectManager*, ObjectType);
    ~UI() {}

    virtual void Update(double dt);
    virtual void Render();
private:
    using TC = std::shared_ptr<TextureComponent>;
    
    std::pair<TC, TC> mainHUD;

    std::vector<std::pair<TC, TC>> batteryCells;
    TC ShiftDown;
    TC ShiftUp;

    std::pair<TC, TC> timerTotal;
    std::pair<TC, TC> timerRemain;

    std::vector<std::pair<TC, TC>> inven;

    TC mouseLeft;
    TC mouseRight;

    TC gameoverFrame;
    std::vector<TC> gameover;

    int gameoverMessage = 0;
    double yGap = -80;
};

