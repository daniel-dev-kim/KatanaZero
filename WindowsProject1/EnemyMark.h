#pragma once
#include "Object.h"
class SpriteManagerComponent;
class Enemy;
class EnemyMark : public Object
{
public:
    enum class MarkType
    {
        None,
        Alert,
        Suspicious,
        Chase,
        Count
    };
	SpriteManagerComponent* marks;
    MarkType currentMark = MarkType::None;

public:
	EnemyMark(ObjectManager* owner, ObjectType type);
    virtual void Update(double dt);
    virtual void Render();
};