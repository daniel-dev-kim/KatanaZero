#include "pch.h"
#include "TransformComponent.h"
#include "Engine.h"
glm::mat4 TransformComponent::GetModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f); // 단위행렬

    glm::vec2 newPos = { position.x, Engine::GetInstance().GetWindowSize().bottom - position.y };
    // 1. 이동
    model = glm::translate(model, glm::vec3(newPos, 0.0f));

    // 2. 회전 (Z축 기준)
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, -1.0f));

    // 3. 스케일
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    return model;
}