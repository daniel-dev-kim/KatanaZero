#pragma once
#include "Object.h"
class TextureComponent;
class TransformComponent;
class ToLeave : public Object
{
    std::shared_ptr<TransformComponent> clickTransform;
    std::shared_ptr<TextureComponent> click1;
    std::shared_ptr<TextureComponent> click2;

    double useClick1Timer = 0.0;
    bool useClick1 = true;
public:
    ToLeave(ObjectManager*, ObjectType);
    ~ToLeave() {}

    virtual void Update(double dt);
    virtual void Render();
};

