#pragma once
#include "Object.h"
class TextureComponent;
class Pause : public Object
{
    std::shared_ptr<TextureComponent> blue;
    std::shared_ptr<TextureComponent> pink;
    std::shared_ptr<TextureComponent> white;
    std::shared_ptr<TextureComponent> pauseChoose;
    std::shared_ptr<TextureComponent> menuKatana;
    std::shared_ptr<TextureComponent> pauseString;
    std::shared_ptr<TextureComponent> keepString;
    std::shared_ptr<TextureComponent> menuString;
    float pinkColorChange = 0.;
    float timer = 0.;
    float selectTimer = 0.;
    unsigned int selected = 0;
    bool drawKatana = true;
    int confirmed = -1;
public:
    Pause(ObjectManager*, ObjectType);

    virtual void Update(double dt);
    virtual void Render();
};