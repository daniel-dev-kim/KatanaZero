#pragma once
#include "ObjectComponent.h"

class LineCollisionComponent : public ObjectComponent
{
private:
    std::vector<glm::vec2> dots;
public:    
    virtual void Initialize() {};
    virtual void Update(double dt) {};
    virtual void Render();
    std::pair<bool, float> CheckCollide(Object* other);
    void ResolveCollide(Object*);
    bool CheckInside(Object* other);
    glm::vec2 GetNormalFromDiagonal(Object* other);
    std::vector<glm::vec2>* GetDots();
};