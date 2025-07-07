#include "pch.h"
#include "Filter.h"
#include "TextureManager.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
Filter::Filter(ObjectManager* owner, ObjectType type) : Object(owner, type)
{
	transform = AddComponent<TransformComponent>();
	texture = AddComponent<TextureComponent>("default");
	transform->SetScale({ 1600, 900 });
	transform->Translate({ 640, 360 });
	info.color = { 0.0, 0.0, 0.0, 0.0 };
}

void Filter::Render()
{
	texture->Render(transform->GetModelMatrix(), info);
}
