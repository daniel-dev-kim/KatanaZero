#pragma once
#include "Object.h"
class TextureComponent;
class TransformComponent;
class ToStart : public Object
{
    enum class Status
    {
        Begin,
        StringMove,
        Blinking,
    };
    std::shared_ptr<TransformComponent> nameTransform1;
    std::shared_ptr<TransformComponent> nameTransform2;
    std::shared_ptr<TextureComponent> name;

    std::shared_ptr<TransformComponent> clickTransform;
    std::shared_ptr<TextureComponent> click1;
    std::shared_ptr<TextureComponent> click2;

    std::shared_ptr<TransformComponent> backTransform;
    std::shared_ptr<TextureComponent> back;

    Status currStatus;
    double backAlpha = 1.;

    double string1X = -480;
    double string2X = 480;

    double useClick1Timer = 0.0;
    bool useClick1 = true;
public:
    ToStart(ObjectManager*, ObjectType);
    ~ToStart() {}

    virtual void Update(double dt);
    virtual void Render();
};

