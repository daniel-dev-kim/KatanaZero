#pragma once
#include "ObjectComponent.h"
class CameraComponent;
class ShaderProgram;
class TextureComponent : public ObjectComponent
{
public:
	TextureComponent(const std::string&, const std::string&);
	TextureComponent(const std::string&);
	void Initialize() override {}
	void Update(double) override {}
	void Render(glm::mat4 modelMat, RenderInfo info = RenderInfo{});
	glm::vec2 GetSize();
	std::string GetKey();
private:
	std::string key;
	glm::vec2 size;
	ShaderProgram* shader;
	CameraComponent* camera;
};