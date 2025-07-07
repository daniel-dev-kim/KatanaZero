#include "pch.h"
#include "Engine.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "Object.h"

void CameraComponent::Initialize() {
    RECT winRect = Engine::GetInstance().GetOriginalWindowSize();
    screenSize = {
        static_cast<float>(winRect.right - winRect.left),
        static_cast<float>(winRect.bottom - winRect.top)
    };

    position = { 0.0f, 0.0f };
    SetTarget(owner->GetComponent<TransformComponent>());
}

void CameraComponent::SetTarget(TransformComponent* t) {
    target = t;
    if (t) position = t->GetPosition();
}

void CameraComponent::SetWorldBounds(const glm::vec2& min, const glm::vec2& max) {
    worldMin = min;
    worldMax = max;
}

void CameraComponent::SetDeadZone(const glm::vec2& size, const glm::vec2& offset) {
    deadZoneSize = size;
    deadZoneOffset = offset;
}

void CameraComponent::StartShake(float duration, float magnitude) {
    shakeDuration = duration;
    shakeMagnitude = magnitude;
    Engine::GetInstance().HitStop();
}

void CameraComponent::Update(double dt) {
    if (!target) return;

    glm::vec2 targetPos = target->GetPosition(); // 플레이어 위치
    glm::vec2 deadZoneCenter = position + deadZoneOffset; // 
    glm::vec2 desiredPos = position;

    glm::vec2 dzHalf = deadZoneSize * 0.5f;
    float left = deadZoneCenter.x - dzHalf.x;
    float right = deadZoneCenter.x + dzHalf.x;
    float top = deadZoneCenter.y - dzHalf.y;
    float bottom = deadZoneCenter.y + dzHalf.y;

    if (targetPos.x < left)
        desiredPos.x = position.x + (targetPos.x - left); // 얼마나 벗어났는지만큼만 이동
    else if (targetPos.x > right)
        desiredPos.x = position.x + (targetPos.x - right);

    if (targetPos.y < top)
        desiredPos.y = position.y + (targetPos.y - top);
    else if (targetPos.y > bottom)
        desiredPos.y = position.y + (targetPos.y - bottom);

    // 부드러운 이동
    position += (desiredPos - position) * static_cast<float>(lerpSpeed * dt);

    // 맵 범위 제한
    glm::vec2 camHalf = screenSize * 0.5f;
    position = glm::clamp(position, worldMin + camHalf, worldMax - camHalf);

    // 진동 처리
    if (shakeDuration > 0.0f) {
        shakeDuration -= static_cast<float>(dt);
        float x = ((rand() % 2001 - 1000) / 1000.0f) * shakeMagnitude;
        float y = ((rand() % 2001 - 1000) / 1000.0f) * shakeMagnitude;
        shakeOffset = { x, y };
    }
    else {
        shakeOffset = { 0.0f, 0.0f };
    }
}

glm::vec2 CameraComponent::GetAppliedPos() const {
    return position - screenSize / 2.f + shakeOffset;
}

glm::mat4 CameraComponent::GetViewMatrix() const {
    return glm::translate(glm::mat4(1.0f), glm::vec3(
        -position.x + screenSize.x / 2.f - shakeOffset.x,
        position.y - screenSize.y / 2.f + shakeOffset.y,
        0.0f
    ));
}

glm::mat4 CameraComponent::GetProjectionMatrix() const {
    return glm::ortho(
        0.0f,
        screenSize.x,
        screenSize.y,
        0.0f,
        -1.0f,
        1.0f
    );
}