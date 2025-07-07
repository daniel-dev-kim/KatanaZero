#include "pch.h"
#include "TransformComponent.h"
#include "Engine.h"
glm::mat4 TransformComponent::GetModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f); // �������

    glm::vec2 newPos = { position.x, Engine::GetInstance().GetWindowSize().bottom - position.y };
    // 1. �̵�
    model = glm::translate(model, glm::vec3(newPos, 0.0f));

    // 2. ȸ�� (Z�� ����)
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, -1.0f));

    // 3. ������
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    return model;
}