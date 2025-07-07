#pragma once
#include "ObjectComponent.h"
class TransformComponent;
class CameraComponent : public ObjectComponent {
private:
    TransformComponent* target = nullptr;

    glm::vec2 position = { 0.0f, 0.0f };      // 카메라 위치
    glm::vec2 velocity = { 0.0f, 0.0f };      // 이동 속도

    glm::vec2 deadZoneSize = { 100.0f, 80.0f };
    glm::vec2 deadZoneOffset = { 0.0f, 0.0f };

    glm::vec2 worldMin = { 0.0f, 0.0f };
    glm::vec2 worldMax = { 2000.0f, 1200.0f };

    float lerpSpeed = 50.0f;
    glm::vec2 screenSize = { 1280.0f, 720.0f };

    float shakeDuration = 0.0f;
    float shakeMagnitude = 0.0f;
    glm::vec2 shakeOffset = { 0.0f, 0.0f };
public:
    void Initialize() override;
    void Update(double dt) override;

    void SetTarget(TransformComponent* t);
    void SetWorldBounds(const glm::vec2& min, const glm::vec2& max);
    void SetDeadZone(const glm::vec2& size, const glm::vec2& offset);

    void StartShake(float duration, float magnitude);

    glm::vec2 GetAppliedPos() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
};