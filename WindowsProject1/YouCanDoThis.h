#pragma once
#include "Object.h"
class TextureComponent;
class YouCanDoThis : public Object
{
public:
    double timer = 0.0;
    bool OMReset = false;
    TextureComponent* texture;
    bool isDone = false;
public:
    YouCanDoThis(ObjectManager*, ObjectType);
    ~YouCanDoThis() {}
    virtual void Update(double);
    virtual void Render();

};

