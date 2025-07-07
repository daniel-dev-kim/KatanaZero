#pragma once
#include "ObjectComponent.h"
class AABBCollisionComponent : public ObjectComponent
{
private:
    glm::vec2 offset = { 0.0f, 0.0f };      // Transform���� �󸶳� ������ ��ġ
    glm::vec2 size = { 1.f, 1.f };      // �浹 �ڽ� ũ��

public:
    virtual void Update(double dt) {};
    virtual void Render();
    bool CheckCollide(Object* other) ;
    bool CheckCollide(glm::vec2 other);
    void ResolveCollide(Object*);
    bool CheckWallSlidable(Object* other);

public:
    void SetSize(glm::vec2 s);
    glm::vec2 GetSize() const;
    void SetOffset(glm::vec2 o);
    glm::vec2 GetOffset() const;
};