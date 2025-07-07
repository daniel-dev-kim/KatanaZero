#include "pch.h"
#include "GO.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
GO::GO(ObjectManager*, ObjectType) : Object(owner, objType)
{
	texture = AddComponent<TextureComponent>("GO", "Resource/Texture/UI/GO/GO.png");
	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
	xOffset = 0.f;
}

void GO::Update(double dt)
{
	transform->Translate(dt * 50.f, 0);
	xOffset += dt * 50.f;
	if (xOffset > 50.f)
	{
		transform->Translate(-xOffset, 0);
		xOffset = 0.;
	}
}

void GO::Render()
{
	if (Engine::GetInstance().GetSceneManager()->activeScene->showGO)
	{
		texture->Render(transform->GetModelMatrix());
	}
	
}
