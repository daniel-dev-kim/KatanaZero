#pragma once
#include "Object.h"
class TextureComponent;
class TransformComponent;
class GO : public Object
{
    TransformComponent* transform;
    TextureComponent* texture;
    float xOffset;
    float originalXOffset;
public:
    GO(ObjectManager*, ObjectType);
    ~GO() {}

    virtual void Update(double dt);
    virtual void Render();
};

