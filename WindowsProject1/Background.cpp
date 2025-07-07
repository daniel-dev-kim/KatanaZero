#include "pch.h"
#include "Background.h"

#include "Engine.h"
#include "TextureManager.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "Player.h"
#include "ObjectManager.h"
Background::Background(ObjectManager* owner, ObjectType obyType) : Object(owner, objType)
{
	p = dynamic_cast<Player*>(owner->GetFrontObject(ObjectType::Player).get());

}

void Background::Update(double dt)
{
	if (off)
	{
		alpha -= dt * 2;
		alpha = std::clamp(alpha, 0.0, 1.0);
	}	
}

void Background::Render()
{
	if(!Engine::GetInstance().uPersistentTrails)
	{
		info.color = glm::vec4(glm::vec3(p->currentSlow), 0.8f * alpha);
		GetComponent<TextureComponent>()->Render(GetComponent<TransformComponent>()->GetModelMatrix(), info);
	}

}

void Background::Off()
{
	off = true;
}

void Background::On()
{
	alpha = 1.;
	off = false;
}
