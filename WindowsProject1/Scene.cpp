#include "pch.h"
#include "ObjectManager.h"
#include "Scene.h"
Scene::~Scene() = default;

void Scene::Load()
{
	objectManager = std::make_shared<ObjectManager>();
	objectManager->Initialize();
}