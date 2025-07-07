#pragma once
#include "Object.h"
class TextureComponent;
class TransformComponent;
class SongTitle : public Object
{
    enum class Status
    {
        BackBegin,
        String1Begin,
        MiddleString2Begin,
        Blinking,
        DisappearInside,
        DisappearOutside,
    };
    std::shared_ptr<TransformComponent> string1Transform;
    std::shared_ptr<TextureComponent> string1;

    std::shared_ptr<TransformComponent> string2Transform;
    std::shared_ptr<TextureComponent> string2;
    
    std::shared_ptr<TransformComponent> backTransform;
    std::shared_ptr<TextureComponent> back;

    std::shared_ptr<TransformComponent> middleTransform;
    std::shared_ptr<TextureComponent> middle;

    Status currStatus;
    double backHeight = 1.;
    double backWidth = 0.;
    double middleHeight = 1.;
    double middleWidth = 0.;

    double string1Alpha = 0.;
    double string2Alpha = 0.;

public:
    SongTitle(ObjectManager*, ObjectType);
    ~SongTitle() {}

    virtual void Update(double dt);
    virtual void Render();
};

