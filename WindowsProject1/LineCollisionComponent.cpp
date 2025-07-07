#include "pch.h"
#include "LineCollisionComponent.h"
#include "AABBCollisionComponent.h"

#include "Object.h"
#include "TransformComponent.h"

#include "Engine.h"
#include "ShaderProgram.h"
#include "CameraComponent.h"
void LineCollisionComponent::Render()
{
	if (!Engine::GetInstance().debug) return;
	ShaderProgram* shader = Engine::GetInstance().GetDebugShaderProgram(); // 별도로 만들어 둠
	CameraComponent* camera = Engine::GetInstance().playerCam;	
	if (dots.size() < 2) return;

	std::vector<glm::vec2> tempDots = dots;
	for (auto& tempDot : tempDots)
	{
		tempDot.y = Engine::GetInstance().GetWindowSize().bottom - tempDot.y;
	}
	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, dots.size() * sizeof(glm::vec2), tempDots.data(), GL_DYNAMIC_DRAW);

	shader->Use();
	shader->SetUniform("uModel", glm::mat4(1.0f));
	shader->SetUniform("uView", camera->GetViewMatrix());
	shader->SetUniform("uProjection", camera->GetProjectionMatrix());
	shader->SetUniform("uColor", glm::vec4(1.0, 1.0, 1.0, 1.0)); // 마젠타
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(dots.size()));
}

std::pair<bool, float> LineCollisionComponent::CheckCollide(Object* other)
{ 
	auto otherTransform = other->GetComponent<TransformComponent>();
	float otherPosX = otherTransform->GetPosition().x;

	const int size = dots.size() - 1;
	int target = -1;
	for (int i = 0; i < size; i++)
	{
		if (otherPosX >= dots[i].x && otherPosX < dots[i + 1].x)
		{
			target = i;
			break;
		}
	}

	if (target == -1) return { false, 0. };

	float x1 = dots[target].x;
	float y1 = dots[target].y;

	float x2 = dots[target + 1].x;
	float y2 = dots[target + 1].y;
	float result = ((y2 - y1) / (x2 - x1)) * (otherPosX - x1) + y1;
	float halfHeight = other->GetComponent<AABBCollisionComponent>()->GetSize().y / 2.;
	float foot = otherTransform->GetPosition().y - halfHeight;
	return { result >= foot, result };
}

void LineCollisionComponent::ResolveCollide(Object* other)
{
	auto otherTransform = other->GetComponent<TransformComponent>();
	float otherPosX = otherTransform->GetPosition().x;

	const int size = dots.size() - 1;
	int target = -1;
	for (int i = 0; i < size; i++)
	{
		if (otherPosX >= dots[i].x && otherPosX < dots[i + 1].x)
		{
			target = i;
			break;
		}
	}

	if (target == -1) return;

	float x1 = dots[target].x;
	float y1 = dots[target].y;

	float x2 = dots[target + 1].x;
	float y2 = dots[target + 1].y;
	float result = ((y2 - y1) / (x2 - x1)) * (otherPosX - x1) + y1;;
	float halfHeight = other->GetComponent<AABBCollisionComponent>()->GetSize().y / 2.;
	otherTransform->SetPosition({ otherPosX , result + halfHeight});
}

bool LineCollisionComponent::CheckInside(Object* other)
{
	auto otherTransform = other->GetComponent<TransformComponent>();
	float otherPosX = otherTransform->GetPosition().x;
	return otherPosX >= dots.front().x && otherPosX <= dots.back().x;
}

glm::vec2 LineCollisionComponent::GetNormalFromDiagonal(Object* other)
{
	auto otherTransform = other->GetComponent<TransformComponent>();
	float otherPosX = otherTransform->GetPosition().x;

	const int size = dots.size() - 1;
	int target = -1;
	for (int i = 0; i < size; i++)
	{
		if (otherPosX >= dots[i].x && otherPosX < dots[i + 1].x)
		{
			target = i;
			break;
		}
	}

	if (target == -1 || target + 1 >= dots.size())
		return glm::vec2(0.0f, 0.0f); // 기본 위 방향

	float x1 = dots[target].x;
	float y1 = dots[target].y;

	float x2 = dots[target + 1].x;
	float y2 = dots[target + 1].y;

	if (y2 == y1)
		return glm::vec2(0.0f, 0.0f); // 기본 위 방향

	// 선분 벡터 계산
	glm::vec2 segment = glm::vec2(x2 - x1, y2 - y1);

	// 법선 벡터 계산 (좌측 법선: (-dy, dx), 우측 법선: (dy, -dx))
	glm::vec2 normal = glm::vec2(-(segment.y), segment.x);

	// 단위 벡터로 정규화
	normal = glm::normalize(normal);

	return normal;
}

std::vector<glm::vec2>* LineCollisionComponent::GetDots()
{
	return &dots;
}

