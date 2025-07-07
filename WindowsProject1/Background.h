#pragma once
#include "Object.h"
class Player;
class Background : public Object
{
public:
    Player* p;
    double alpha = 1.0;
    bool off = false;
public:
    Background(ObjectManager*, ObjectType);
    ~Background() {}
    virtual void Update(double);
    virtual void Render();
    void Off();
    void On();
    RenderInfo info{};
};

