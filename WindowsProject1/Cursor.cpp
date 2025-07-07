#include "pch.h"
#include "Cursor.h"

#include "TransformComponent.h"
#include "TextureComponent.h"
#include "CameraComponent.h"

#include "Engine.h"
#include "InputSystem.h"
Cursor::Cursor(ObjectManager* owner, ObjectType objType) : Object(owner, objType)
{
	auto cursorTransform = AddComponent<TransformComponent>();
	cursorTexture = AddComponent<TextureComponent>("ui_cursor", "Resource/Texture/UI/ui_cursor/0.png");
	cursorTransform->SetScale(2., 2.);
}

void Cursor::Update(double dt)
{
	auto transform = GetComponent<TransformComponent>();
	transform->SetPosition(Engine::GetInstance().GetInputSystem()->GetMousePos() + Engine::GetInstance().playerCam->GetAppliedPos());
}

void Cursor::Render()
{
	cursorTexture->Render(GetComponent<TransformComponent>()->GetModelMatrix());
}
