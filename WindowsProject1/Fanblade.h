#pragma once
#include "Object.h"
class SpriteManagerComponent;
class TextureComponent;
class Player;
class Fanblade : public Object
{
    Player* p;
    SpriteManagerComponent* sprite;
    TextureComponent* texture;
public:
    bool isThroughable = false;
    int timer;
    Fanblade(ObjectManager*, ObjectType);
    ~Fanblade() {}

    virtual void Update(double dt);
    virtual void Render();
};