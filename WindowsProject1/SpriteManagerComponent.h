#pragma once
#include "ObjectComponent.h"
class CameraComponent;
class ShaderProgram;
struct RenderInfo;
struct TextureData;
class Sprite 
{
	ShaderProgram* shader;
	CameraComponent* camera;
	int totalFrame;
	int currentFrame;
	double speed;
	double timer;
	std::string spriteName;
	bool isRepeat;
public:
	void Reset();
	void Update(double dt);
	void Render(glm::mat4, RenderInfo info = RenderInfo{});
	void SetSpriteFrame(int f);
	TextureData* GetCurrentData();
	int GetCurrentFrame();
	bool isDone();
	Sprite() {};
	Sprite(const std::string& name, int totalFrame, double speed, bool isRepeat);
};

class SpriteManagerComponent : public ObjectComponent
{
	std::unordered_map<std::string, Sprite> sprites;
	std::string playedSpriteName;
	bool freeze = false;
public:
	SpriteManagerComponent(const std::string& filePath);
	void LoadSprites(const std::string& filePath);
	virtual void Initialize() override {};
	virtual void Update(double dt);
	bool IsDone();
	void Render(RenderInfo info = RenderInfo{});
	void Render(glm::mat4 modelMat, RenderInfo info = RenderInfo{});

	void PlaySprite(const std::string& name);
	void Freeze(bool f);
	Sprite* GetCurrentSprite();
	std::string GetName() const;
};