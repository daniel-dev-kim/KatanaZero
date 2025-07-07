#include "pch.h"
#include "AABBCollisionComponent.h"
#include "Object.h"
#include "TransformComponent.h"

#include "Engine.h"
#include "ShaderProgram.h"
#include "TextureManager.h"
#include "CameraComponent.h"
#include "TextureComponent.h"

void AABBCollisionComponent::Render()
{
    if (!Engine::GetInstance().debug) return;
    ShaderProgram* shader = Engine::GetInstance().GetDebugShaderProgram(); // º°µµ·Î ¸¸µé¾î µÒ
    CameraComponent* camera = Engine::GetInstance().playerCam;

    glm::vec2 pos = owner->GetComponent<TransformComponent>()->GetPosition() + offset;
    pos.y = Engine::GetInstance().GetWindowSize().bottom - pos.y;
    glm::vec2 half = size * 0.5f;

    glm::vec2 topLeft = pos + glm::vec2(-half.x, -half.y);
    glm::vec2 topRight = pos + glm::vec2(half.x, -half.y);
    glm::vec2 bottomRight = pos + glm::vec2(half.x, half.y);
    glm::vec2 bottomLeft = pos + glm::vec2(-half.x, half.y);

    std::vector<glm::vec2> vertices = {
        topLeft, topRight, bottomRight, bottomLeft
    };

    // VBO Àü¼Û
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);

    shader->Use();
    shader->SetUniform("uModel", glm::mat4(1.0f));
    shader->SetUniform("uView", camera->GetViewMatrix());
    shader->SetUniform("uProjection", camera->GetProjectionMatrix());
    shader->SetUniform("uColor", glm::vec4(1.0, 0.0, 1.0, 1.0)); // ¸¶Á¨Å¸
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

bool AABBCollisionComponent::CheckCollide(Object* other)
{
    auto transform = owner->GetComponent<TransformComponent>();
    auto otherTransform = other->GetComponent<TransformComponent>();
    auto otherCollision = other->GetComponent<AABBCollisionComponent>();

    glm::vec2 aCenter = transform->GetPosition() + offset;
    glm::vec2 bCenter = otherTransform->GetPosition();

    glm::vec2 aHalf = this->size * 0.5f;
    glm::vec2 bHalf = otherCollision->GetSize() * 0.5f;

    float dx = std::abs(aCenter.x - bCenter.x);
    float dy = std::abs(aCenter.y - bCenter.y);

    return dx < (aHalf.x + bHalf.x) && dy < (aHalf.y + bHalf.y);
}

bool AABBCollisionComponent::CheckCollide(glm::vec2 point)
{
    auto transform = owner->GetComponent<TransformComponent>();
    glm::vec2 center = transform->GetPosition() + offset;
    glm::vec2 halfSize = size * 0.5f;

    glm::vec2 min = center - halfSize;
    glm::vec2 max = center + halfSize;

    return point.x >= min.x && point.x <= max.x &&
        point.y >= min.y && point.y <= max.y;
}

void AABBCollisionComponent::ResolveCollide(Object* other)
{
    auto transform = owner->GetComponent<TransformComponent>();
    auto otherTransform = other->GetComponent<TransformComponent>();
    auto otherCollision = other->GetComponent<AABBCollisionComponent>();

    glm::vec2 aCenter = transform->GetPosition() + offset;
    glm::vec2 bCenter = otherTransform->GetPosition();

    glm::vec2 aHalf = this->size * 0.5f;
    glm::vec2 bHalf = otherCollision->GetSize() * 0.5f;

    glm::vec2 delta = bCenter - aCenter;
    glm::vec2 overlap = aHalf + bHalf - abs(delta); // µ¨Å¸°¡ ¾ç¼ö

    if (overlap.x > 0 && overlap.y > 0)
    {
        if (overlap.x < overlap.y)
        {
            //¿·¿¡¼­ ºÎµúÈù ÄÉÀÌ½º
            otherTransform->Translate(delta.x > 0 ? overlap.x : -overlap.x, 0);
        }
        else if(other->GetVelocity().y >= 0.)
        {
            otherTransform->Translate(0, -overlap.y);
        }
    }
}

bool AABBCollisionComponent::CheckWallSlidable(Object* other)
{
    auto transform = owner->GetComponent<TransformComponent>();
    auto otherTransform = other->GetComponent<TransformComponent>();
    auto otherCollision = other->GetComponent<AABBCollisionComponent>();

    float terrainHalfHeight = this->size.y / 2.0f;
    float otherHalfHeight = otherCollision->GetSize().y / 2.0f;

    glm::vec2 terrainPos = transform->GetPosition() + offset;
    glm::vec2 otherPos = otherTransform->GetPosition();

    float terrainTop = terrainPos.y + terrainHalfHeight;
    float terrainBottom = terrainPos.y - terrainHalfHeight;
    float playerTop = otherPos.y + otherHalfHeight;
    float playerBottom = otherPos.y - otherHalfHeight;

    float overlapTop = glm::min(terrainTop, playerTop);
    float overlapBottom = glm::max(terrainBottom, playerBottom);
    float overlapHeight = glm::max(0.0f, overlapTop - overlapBottom);

    float playerHeight = otherHalfHeight * 2.0f;

    // ÇÃ·¹ÀÌ¾î ¸öÀÇ 70% ÀÌ»óÀÌ °ãÄ¡¸é true
    return (overlapHeight / playerHeight) >= 0.7f;
}

void AABBCollisionComponent::SetSize(glm::vec2 s)
{
    size = s;
}

glm::vec2 AABBCollisionComponent::GetSize() const
{
    return size;
}

void AABBCollisionComponent::SetOffset(glm::vec2 o)
{
    offset = o;
}

glm::vec2 AABBCollisionComponent::GetOffset() const
{
    return offset;
}
