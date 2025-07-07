#pragma once
#include "Object.h"
class TransformComponent;
class TextureComponent;
class Filter : public Object
{
    TransformComponent* transform;
    TextureComponent* texture;
public:
    Filter(ObjectManager*, ObjectType);
    ~Filter() {}
    virtual void Render();
    RenderInfo info{};
};
