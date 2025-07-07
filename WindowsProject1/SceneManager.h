#pragma once
class Scene;

class SceneManager
{
public:
    void Initialize();
    void Update(double dt);
    void Render();
    
    void AddScene(const std::string& name, std::shared_ptr<Scene> scene);
    void SetActiveScene(const std::string& name);
public:
	std::shared_ptr<Scene> activeScene;
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
};