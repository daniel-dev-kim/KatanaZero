#pragma once
#include "Object.h"
class TransformComponent;
class SpriteManagerComponent;
class AABBCollisionComponent;
class Door : public Object
{
    AABBCollisionComponent* AABB;
    TransformComponent* transform;
    SpriteManagerComponent* sprite;
public:
    Door(ObjectManager*, ObjectType);
    ~Door() {}
    bool close = true;

    virtual void Update(double);
};