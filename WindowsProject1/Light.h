#pragma once
#include "Object.h"
class TransformComponent;
class TextureComponent;
class Light : public Object
{
public:
    TransformComponent* transform;
    TextureComponent* texture;
    RenderInfo info{};
    bool move;

    float radiusX = 50.0f;
    float radiusY = 30.0f;
    float timer = 0.0f;

    glm::vec2 gap = {0, 0};
public:
    Light(ObjectManager*, ObjectType, glm::vec3, bool move = false);
    ~Light() {}

    virtual void Update(double);
    virtual void Render();

    glm::vec4 GetColor();
};