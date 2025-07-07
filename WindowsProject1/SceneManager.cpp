#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "BossScene.h"
#include "FactoryScene.h"
#include "SmokeScene.h"
#include "TitleScene.h"
#include "EndingScene.h"
void SceneManager::Initialize()
{
	AddScene("TitleScene", std::make_shared<TitleScene>());
	AddScene("FactoryScene", std::make_shared<FactoryScene>());
	AddScene("SmokeScene", std::make_shared<SmokeScene>());
	AddScene("BossScene", std::make_shared<BossScene>());
	AddScene("EndingScene", std::make_shared<EndingScene>());

	activeScene = scenes["SmokeScene"];
	activeScene->Load();
}

void SceneManager::Update(double dt)
{
	activeScene->Update(dt);
}

void SceneManager::Render()
{
	activeScene->Render();
}

void SceneManager::AddScene(const std::string& name, std::shared_ptr<Scene> scene)
{
	scenes[name] = scene;
}

void SceneManager::SetActiveScene(const std::string& name) {
	activeScene->Unload();
    auto it = scenes.find(name);
    if (it != scenes.end()) {
        activeScene = it->second;
		activeScene->Load();
    }
}
