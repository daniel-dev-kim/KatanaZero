#include "pch.h"
#include "LaserBody.h"
#include "TextureComponent.h"
#include "TransformComponent.h"

LaserBody::LaserBody(ObjectManager* owner, ObjectType objType) : Object(owner, objType)
{
	on = std::make_shared<TextureComponent>("Laser_On", "Resource/Texture/Object/obj_laser/laserOn.png");
	off = std::make_shared<TextureComponent>("Laser_On", "Resource/Texture/Object/obj_laser/laserOff.png");
	transform = AddComponent<TransformComponent>();
	transform->SetScale(2, 2);
}

void LaserBody::Render()
{
	if (laserSwitch) on->Render(transform->GetModelMatrix());
	else off->Render(transform->GetModelMatrix());
}
