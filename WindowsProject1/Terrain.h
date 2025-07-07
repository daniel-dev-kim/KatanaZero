#pragma once
#include "Object.h"

class Terrain : public Object
{
    bool wallSidable = false;
    bool wallThroughable = false;
public:
    Terrain(ObjectManager*, ObjectType);
    ~Terrain(){}

    virtual void Update(double dt);

    void AddAABB(glm::vec2 size, glm::vec2 offset = {0, 0});
    void AddLine(std::vector<glm::vec2> dots);
    
    void SetWallSidable(bool);
    bool IsWallSidable();

    void SetWallThroughable(bool);
    bool IsWallThroughable();
};

