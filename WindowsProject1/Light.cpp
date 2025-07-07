#include "pch.h"
#include "Light.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "SpriteManagerComponent.h"

#include "Engine.h"
#include "Random.h"

#include "ObjectManager.h"
Light::Light(ObjectManager*, ObjectType, glm::vec3 color, bool move) : Object(owner, objType), move(move)
{
	texture = AddComponent<TextureComponent>("LightSphere", "Resource/Texture/Object/obj_lightsphere/spr_lightsphere.png");
	transform = AddComponent<TransformComponent>();
	info.color = glm::vec4(color, 0.15f);

    radiusX = Engine::GetInstance().GetRandom()->get(30, 50);
    radiusY = Engine::GetInstance().GetRandom()->get(30, 50);
    timer = Engine::GetInstance().GetRandom()->get(0., PI);
}

void Light::Update(double dt)
{
	Object::Update(dt);
    if (move)
    {
        timer += dt;
        float angle = timer * 2.5f;;
        gap = { radiusX * cos(angle), radiusY * sin(angle) };
    }
}

void Light::Render()
{
 
    auto gapMat = glm::translate(glm::mat4(1.0), glm::vec3(gap, 0.0));
    auto sprite = GetComponent<SpriteManagerComponent>();
    if (sprite)
    {
        auto temp = info.color;
        info.color.r = 1.0;
        info.color.g = 1.0;
        info.color.b = 1.0;
        sprite->Render(transform->GetModelMatrix(), info);
        info.color = temp;
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        texture->Render(gapMat * transform->GetModelMatrix(), info);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
   

}

glm::vec4 Light::GetColor()
{
	return info.color;
}
