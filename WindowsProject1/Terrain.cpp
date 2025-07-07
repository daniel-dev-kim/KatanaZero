#include "pch.h"
#include "Terrain.h"

#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"
#include "TransformComponent.h"

#include "Engine.h"
Terrain::Terrain(ObjectManager* owner, ObjectType objType) : Object(owner, objType), wallSidable(false)
{
}

void Terrain::Update(double dt)
{
}

void Terrain::AddAABB(glm::vec2 size, glm::vec2 offset)
{
    auto AABB = AddComponent<AABBCollisionComponent>();
    AABB->SetSize(size);
    AABB->SetOffset(offset);
}

void Terrain::AddLine(std::vector<glm::vec2> dots)
{
    auto Line = AddComponent<LineCollisionComponent>();
    *Line->GetDots() = dots;
}

void Terrain::SetWallSidable(bool w)
{
    wallSidable = w;
}

bool Terrain::IsWallSidable()
{
    return wallSidable;
}

void Terrain::SetWallThroughable(bool w)
{
    wallThroughable = w;
}

bool Terrain::IsWallThroughable()
{
    return wallThroughable;
}
