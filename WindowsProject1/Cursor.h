#pragma once
#include "Object.h"
class TextureComponent;
class Cursor : public Object
{
    TextureComponent* cursorTexture;
public:
    Cursor(ObjectManager*, ObjectType);
    ~Cursor() {}

    virtual void Update(double dt);
    virtual void Render();
};

