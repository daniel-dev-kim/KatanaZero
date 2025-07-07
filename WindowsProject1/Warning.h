#pragma once
#include "Object.h"
class TransformComponent;
class Warning : public Object
{
    TransformComponent* transform;
    float size;
    bool chase;
public:
    Warning(ObjectManager*, ObjectType, float, bool chase = false);
    ~Warning() {}
    virtual void Update(double dt);
    virtual void Render() {};
};