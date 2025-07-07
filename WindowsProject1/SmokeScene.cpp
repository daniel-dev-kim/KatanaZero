#include "pch.h"
#include "SmokeScene.h"
#include "ObjectManager.h"

#include "Engine.h"
#include "InputSystem.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "SceneManager.h"
//Objects
#include "Background.h"
#include "Player.h"
#include "Terrain.h"
#include "Cursor.h"
#include "Item.h"
#include "UI.h"
#include "Fanblade.h"
#include "Grunt.h"
#include "LaserBody.h"
#include "LaserBeam.h"
#include "Transition.h"
#include "GO.h"
#include "Pause.h"
//Components
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "AABBCollisionComponent.h"
#include "LineCollisionComponent.h"
#include "Light.h"
#include "SpriteManagerComponent.h"
#include "Gangster.h"
#include "YouCanDoThis.h"
#include "Door.h"
#include "Shotgun.h"
#include "ToLeave.h"
void SmokeScene::Load()
{
	Scene::Load();
	Engine::GetInstance().GetInputSystem()->SetKeyWork(true);

	// ÇÃ·¹ÀÌ¾î
	{
		player = std::make_shared<Player>(objectManager.get(), ObjectType::Player);
		player->GetComponent<TransformComponent>()->SetPosition(100, 130);
		objectManager->AddObject(ObjectType::Player, player);
		player->ChangeState(&player->stateIdle);
	}
	{
		objectManager->pause = std::make_shared<Pause>(objectManager.get(), ObjectType::UI);
	}
	{//ÆäÀÌµåÀÎ
		for (int i = 0; i < 20; i++)
		{
			auto fadeIn = std::make_shared<Transition>(objectManager.get(), ObjectType::EffectOverUI, true);
			fadeIn->GetComponent<TransformComponent>()->SetPosition(32 + 64 * (19 - i), Engine::GetInstance().GetWindowSize().bottom / 2.);
			fadeIn->SetStartTimer(i * 1 / 60.);
			RenderInfo info{};
			info.isFixed = true;
			fadeIn->SetRenderInfo(info);
			fadeIn->SetIsDeadBySpriteEnd(true);
			objectManager->AddObject(ObjectType::EffectOverUI, fadeIn);
		}
	}

	{// ¹®

		{
			auto door = std::make_shared<Door>(objectManager.get(), ObjectType::Door);
			door->GetComponent<TransformComponent>()->SetPosition(805, 416);//+37, -64
			objectManager->AddObject(ObjectType::Door, door);
		}
		{
			auto door = std::make_shared<Door>(objectManager.get(), ObjectType::Door);
			door->GetComponent<TransformComponent>()->SetPosition(1381, 416);
			objectManager->AddObject(ObjectType::Door, door);
		}

		{
			auto door = std::make_shared<Door>(objectManager.get(), ObjectType::Door);
			door->GetComponent<TransformComponent>()->SetPosition(1893, 480);
			objectManager->AddObject(ObjectType::Door, door);
		}

		{
			auto door = std::make_shared<Door>(objectManager.get(), ObjectType::Door);
			door->GetComponent<TransformComponent>()->SetPosition(1893, 480);
			objectManager->AddObject(ObjectType::Door, door);
		}

		{
			auto door = std::make_shared<Door>(objectManager.get(), ObjectType::Door);
			door->GetComponent<TransformComponent>()->SetPosition(3297, 480);
			objectManager->AddObject(ObjectType::Door, door);
		}
	}
	//UI
	{
		//Ä¿¼­
		{
			auto cursor = std::make_shared<Cursor>(objectManager.get(), ObjectType::UI);
			objectManager->AddObject(ObjectType::UI, cursor);
		}

		{
			auto ui = std::make_shared<UI>(objectManager.get(), ObjectType::UI);
			objectManager->AddObject(ObjectType::UI, ui);
		}

		{//GO
			auto go = std::make_shared<GO>(objectManager.get(), ObjectType::UI);
			auto transform = go->GetComponent<TransformComponent>();
			transform->SetPosition({ 3450, 500 });
			objectManager->AddObject(ObjectType::UI, go);
		}
	}
	// ¾ÆÀÌÅÛ
	{
		{
			auto butcherknife = std::make_shared<Item>(objectManager.get(), ObjectType::Item);
			butcherknife->SetItemType(ItemType::Knife);
			butcherknife->AddComponent<TextureComponent>("obj_butcher_knife", "Resource/Texture/Object/obj_butcher_knife/0.png");
			auto transform = butcherknife->AddComponent<TransformComponent>();
			transform->SetPosition(280, 112);
			objectManager->AddObject(ObjectType::Item, butcherknife);
		}

		{
			auto butcherknife = std::make_shared<Item>(objectManager.get(), ObjectType::Item);
			butcherknife->SetItemType(ItemType::Knife);
			butcherknife->AddComponent<TextureComponent>("obj_butcher_knife", "Resource/Texture/Object/obj_butcher_knife/0.png");
			auto transform = butcherknife->AddComponent<TransformComponent>();
			transform->SetPosition(630, 368);
			objectManager->AddObject(ObjectType::Item, butcherknife);
		}

		{
			auto smoke = std::make_shared<Item>(objectManager.get(), ObjectType::Item);
			smoke->SetItemType(ItemType::Smoke);
			smoke->AddComponent<TextureComponent>("obj_smoke_vial", "Resource/Texture/Object/obj_smoke_vial/0.png");
			auto transform = smoke->AddComponent<TransformComponent>();
			transform->SetPosition(2050, 754);
			objectManager->AddObject(ObjectType::Item, smoke);
		}

	}

	{ // Àû
		{
			auto gangster = std::make_shared<Gangster>(objectManager.get(), ObjectType::Enemy);
			gangster->GetComponent<TransformComponent>()->SetPosition(540, 390); // ,+38
			objectManager->AddObject(ObjectType::Enemy, gangster);
			gangster->initState = gangster->stateWalk;
			gangster->ChangeState(gangster->stateWalk);
		}
		{
			auto gangster = std::make_shared<Gangster>(objectManager.get(), ObjectType::Enemy);
			gangster->GetComponent<TransformComponent>()->SetPosition(340, 390);
			gangster->GetComponent<TransformComponent>()->ScaleBy(-1, 1);
			objectManager->AddObject(ObjectType::Enemy, gangster);
			gangster->initState = gangster->stateWalk;
			gangster->ChangeState(gangster->stateWalk);

		}

		{
			auto grunt = std::make_shared<Grunt>(objectManager.get(), ObjectType::Enemy);
			grunt->GetComponent<TransformComponent>()->SetPosition(930, 390);
			objectManager->AddObject(ObjectType::Enemy, grunt);
			grunt->initState = grunt->stateWalk;
			grunt->ChangeState(grunt->stateWalk);
		}

		{
			auto gangster = std::make_shared<Gangster>(objectManager.get(), ObjectType::Enemy);
			gangster->GetComponent<TransformComponent>()->SetPosition(1120, 390);
			gangster->GetComponent<TransformComponent>()->ScaleBy(-1, 1);
			objectManager->AddObject(ObjectType::Enemy, gangster);
			gangster->initState = gangster->stateWalk;
			gangster->ChangeState(gangster->stateWalk);
		}

		{
			auto shotgun = std::make_shared<Shotgun>(objectManager.get(), ObjectType::Enemy);
			shotgun->GetComponent<TransformComponent>(	)->SetPosition(1664, 422);
			shotgun->GetComponent<TransformComponent>()->ScaleBy(-1, 1);
			objectManager->AddObject(ObjectType::Enemy, shotgun);
		}

		{
			auto shotgun = std::make_shared<Shotgun>(objectManager.get(), ObjectType::Enemy);
			shotgun->GetComponent<TransformComponent>()->SetPosition(2272, 454);
			shotgun->GetComponent<TransformComponent>()->ScaleBy(-1, 1);
			objectManager->AddObject(ObjectType::Enemy, shotgun);
			shotgun->initState = shotgun->stateWalk;
			shotgun->ChangeState(shotgun->stateWalk);
		}

		{
			auto gangster = std::make_shared<Gangster>(objectManager.get(), ObjectType::Enemy);
			gangster->GetComponent<TransformComponent>()->SetPosition(2368, 454);
			gangster->GetComponent<TransformComponent>()->ScaleBy(1, 1);
			objectManager->AddObject(ObjectType::Enemy, gangster);
			gangster->initState = gangster->stateWalk;
			gangster->ChangeState(gangster->stateWalk);
		}

		{
			auto gangster = std::make_shared<Gangster>(objectManager.get(), ObjectType::Enemy);
			gangster->GetComponent<TransformComponent>()->SetPosition(2624, 454);
			gangster->GetComponent<TransformComponent>()->ScaleBy(1, 1);
			objectManager->AddObject(ObjectType::Enemy, gangster);
			gangster->initState = gangster->stateWalk;
			gangster->ChangeState(gangster->stateWalk);
		}

		{
			auto gangster = std::make_shared<Gangster>(objectManager.get(), ObjectType::Enemy);
			gangster->GetComponent<TransformComponent>()->SetPosition(2976, 454);
			gangster->GetComponent<TransformComponent>()->ScaleBy(-1, 1);
			objectManager->AddObject(ObjectType::Enemy, gangster);
			gangster->initState = gangster->stateWalk;
			gangster->ChangeState(gangster->stateWalk);
		}
	}

	glm::vec3 lightColor = glm::vec3(1.0, 0.5, 0.0);
	{ // ºû
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 128, 384 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 96, 224 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 256, 128 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 320, 480 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 416, 192 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 448, 384 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 640, 480 });
			objectManager->AddObject(ObjectType::Light, light);
		}

		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 864, 416 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1056, 512 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1216, 416 });
			objectManager->AddObject(ObjectType::Light, light);
		}

		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1440, 391 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1504, 512 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1632, 623 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1696, 810 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1760, 446 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1984, 480 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 1984, 758 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 2112, 576 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 2208, 824 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 2464, 752 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 2592, 576 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 2752, 828 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 2848, 448 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 2976, 750 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 3040, 576 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 3168, 800 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 3360, 544 });
			objectManager->AddObject(ObjectType::Light, light);
		}
		{
			auto light = std::make_shared<Light>(objectManager.get(), ObjectType::Light, lightColor, true);
			auto transform = light->GetComponent<TransformComponent>();
			transform->SetPosition({ 3488, 544 });
			objectManager->AddObject(ObjectType::Light, light);
		}
	}



	//¹è°æ
	{
		auto bg = std::make_shared<Background>(objectManager.get(), ObjectType::Background);
		auto texture = bg->AddComponent<TextureComponent>("bg_Smoke", "Resource/Texture/Background/bg_stage/stage2bg.png");
		auto transform = bg->AddComponent<TransformComponent>();
		sceneSize = texture->GetSize();
		transform->SetPosition(sceneSize / 2.f);
		Engine::GetInstance().playerCam->SetWorldBounds({ 0.f, 0.f }, sceneSize);
		objectManager->AddObject(ObjectType::Background, bg);
	}

	{ //ÀÔ¸· º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ -116, 192 }); // Center 
		t->AddAABB({ 232, 192 }, { 0, 0 }); // Size
		t->SetWallSidable(false);
		objectManager->AddObject(ObjectType::Terrain, t);
	}

	{ //0¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ -84, 640 }); // Center 
		t->AddAABB({ 232, 704 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}

	{ //1¹ø º® 
		{
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 208, 47 }); // Center 
			t->AddAABB({ 416, 96 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}


		{ //Ã¹ ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {0, 96}, {416, 96}, {672, 352} });
			objectManager->AddObject(ObjectType::Terrain, t);
			auto player = dynamic_cast<Player*>(objectManager->GetFrontObject(ObjectType::Player).get());
			player->currentGround = t->GetComponent<LineCollisionComponent>();
			player->firstGround = t->GetComponent<LineCollisionComponent>();
		}
		{ //Ã¹ ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {416, 90}, {672, 346} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ //Ã¹ ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {416, 84}, {672, 340} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}
	
	}

	{ //2¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 384, 768 }); // Center 
		t->AddAABB({ 704, 448 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}
	{ //3¹ø º® 
		{
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 160, 319 }); // Center 
			t->AddAABB({ 224, 64 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}
		{
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 302, 288 }); // Center 
			t->AddAABB({ 64, 1 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}


		{ //µÎ¹øÂ° ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {32, 352}, {288, 352}});
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ //µÎ¹øÂ° ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({{288, 352}, {352, 288} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ //µÎ¹øÂ° ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->SetWallThroughable(true);
			t->AddLine({ {287, 352}, {673, 352} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ //µÎ¹øÂ° ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {672, 352}, {1484, 352}, {1516, 384}, {1772, 384},  {1804, 416}, {4096, 416} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}
	}

	{ //11¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 1104, 175 }); // Center 
		t->AddAABB({ 800, 352 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}

	{ //12¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 768, 736 }); // Center 
		t->AddAABB({ 64, 512 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}

	{ //13¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 1056, 800 }); // Center 
		t->AddAABB({ 512, 384 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}

	{ //14¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 1344, 736 }); // Center 
		t->AddAABB({ 64, 512 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}
	{ //15¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 1488, 768 }); // Center 
		t->AddAABB({ 224, 448 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}
	{ //16¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 1632, 191 }); // Center 
		t->AddAABB({ 160, 384 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}
	{ //17¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 2416, 944 }); // Center 
		t->AddAABB({ 1632, 96 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}
	{ //2Ãþ
		{//18¹ø 2Ãþ
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 1808, 640 }); // Center 
			t->AddAABB({ 160, 192 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}
		{ //20¹ø 2Ãþ 
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			transform->SetPosition({ 2128, 704 }); // Center 
			t->AddAABB({ 480, 64 }, { 0, 0 }); // Size
			t->SetWallSidable(true);
			objectManager->AddObject(ObjectType::Terrain, t);
		}

		{ //2Ãþ ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			t->SetWallThroughable(true);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {1600, 736}, {1728, 736} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}
		{ //2Ãþ ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {1728, 736}, {2368, 736} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}
		{ //2Ãþ ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			t->SetWallThroughable(true);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {2368, 736}, {2624, 736} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}
		{ //2Ãþ ¹Ù´Ú
			auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
			auto transform = t->AddComponent<TransformComponent>();
			t->AddLine({ {2624, 736}, {3152, 736} });
			objectManager->AddObject(ObjectType::Terrain, t);
		}
	}

	{ //19¹ø ¹Ù´Ú
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 2656, 207 }); // Center 
		t->AddAABB({ 1632, 416 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}

	{ //21¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 2928, 704 }); // Center 
		t->AddAABB({ 608, 64 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}
	{ //22¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 3264, 768 }); // Center 
		t->AddAABB({ 64, 448 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}
	{ //23¹ø º® 
		auto t = std::make_shared<Terrain>(objectManager.get(), ObjectType::Terrain);
		auto transform = t->AddComponent<TransformComponent>();
		transform->SetPosition({ 3424, 832 }); // Center 
		t->AddAABB({ 256, 320 }, { 0, 0 }); // Size
		t->SetWallSidable(true);
		objectManager->AddObject(ObjectType::Terrain, t);
	}

	zigzagTimer = 0.0;
	showGO = false;
	ChangeSceneState(SceneState::TransitionIn);
}

void SmokeScene::Unload()
{
	objectManager->Clear();
}

void SmokeScene::Render()
{
	objectManager->Render();
}


void SmokeScene::Update(double dt)
{

	switch (currState)
	{
	case SceneState::TransitionIn:
	{
		if (objectManager->GetBackObject(ObjectType::EffectOverUI)->GetComponent<SpriteManagerComponent>()->IsDone())
		{
			startTimer += dt;
		}
		if (startTimer > 0.5)
		{
			ChangeSceneState(SceneState::ZigZag);
			objectManager->ClearObjectList(ObjectType::EffectOverUI);
		}
	} break;
	case SceneState::ZigZag:
	{
		Engine::GetInstance().zigzagStrength = 1.0f;
		Engine::GetInstance().zigzagOffset = zigzagTimer * 4.0;
		zigzagTimer += dt;
		if (zigzagTimer > (FirstZigzag ? 0.75 : 0.25)) ChangeSceneState(SceneState::Play);
	} break;
	case SceneState::Play:
	{
		auto monsters = objectManager->GetObjectList(ObjectType::Enemy);
		bool allDie = true;
		for (auto monster : monsters)
		{
			if (!dynamic_cast<Enemy*>(monster.get())->willDie)
			{
				allDie = false;
			}
		}

		if (allDie) showGO = true;
		if (showGO && player->GetComponent<TransformComponent>()->GetPosition().x > sceneSize.x) ChangeSceneState(SceneState::YouCanDoThis);
		if (player->die) ChangeSceneState(SceneState::Die);
	} break;
	case SceneState::Die:
	{
		if (Engine::GetInstance().GetInputSystem()->IsKeyPressed(InputKey::LButton)) ChangeSceneState(SceneState::Rewind);
	} break;
	case SceneState::Rewind:
	{
		if (objectManager->GetFrameIndex() == 0)
		{
			player->currentGround = player->firstGround;
			objectManager->SetState(ObjectManager::OMState::Idle);
			ChangeSceneState(SceneState::ZigZag);
		}
	} break;
	case SceneState::YouCanDoThis:
	{
		if (youCanDoThis->IsDead())
		{
			ChangeSceneState(SceneState::Replay);
			objectManager->ClearObjectList(ObjectType::EffectOverUI);
		}
	} break;
	case SceneState::Replay:
	{
		if (objectManager->GetFrameIndex() == objectManager->GetMaxIndex() || Engine::GetInstance().GetInputSystem()->IsKeyPressed(InputKey::RButton))
		{
			ChangeSceneState(SceneState::TransitionOut);
		}
	} break;
	case SceneState::TransitionOut:
	{
		if (lastTransition->GetComponent<SpriteManagerComponent>()->IsDone())
		{
			Engine::GetInstance().GetSceneManager()->SetActiveScene("BossScene");
		}
	} break;
	case SceneState::Pause:
	{

	} break;
	}

	objectManager->Update(dt);

}

void SmokeScene::ChangeSceneState(SceneState newState)
{
	switch (newState)
	{
	case SceneState::TransitionIn:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("transition_end");

		Engine::GetInstance().mono = false;
		objectManager->SetState(ObjectManager::OMState::Idle);
		for (int i = 0; i < 20; i++)
		{
			auto fadeIn = std::make_shared<Transition>(objectManager.get(), ObjectType::EffectOverUI, true);
			fadeIn->GetComponent<TransformComponent>()->SetPosition(32 + 64 * (19 - i), Engine::GetInstance().GetWindowSize().bottom / 2.);
			fadeIn->SetStartTimer(i * 1 / 120.);
			RenderInfo info{};
			info.isFixed = true;
			fadeIn->SetRenderInfo(info);
			fadeIn->SetIsDeadBySpriteEnd(true);
			objectManager->AddObject(ObjectType::EffectOverUI, fadeIn);
		}
	} break;
	case SceneState::ZigZag:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("degauss");
		zigzagTimer = 0.;
	} break;
	case SceneState::Play:
	{
		FirstZigzag = false;
		Engine::GetInstance().zigzagStrength = 0.0f;
		Engine::GetInstance().zigzagOffset = 0.0f;
		objectManager->SetState(ObjectManager::OMState::Recording);
		player->CanMove = true;
	} break;
	case SceneState::Die:
	{
		objectManager->SetState(ObjectManager::OMState::SetUpRewinding);
		player->CanMove = false;
	} break;
	case SceneState::Rewind:
	{
		Engine::GetInstance().GetAudioManager()->StartSound("Rewind");
		objectManager->SetState(ObjectManager::OMState::Rewinding);
		player->ChangeState(&player->stateIdle);

	} break;
	case SceneState::YouCanDoThis:
	{
		player->CanMove = false;
		objectManager->ClearObjectList(ObjectType::UI);
		youCanDoThis = std::make_shared<YouCanDoThis>(objectManager.get(), ObjectType::EffectOverUI);
		objectManager->AddObject(ObjectType::EffectOverUI, youCanDoThis);
	} break;
	case SceneState::Replay:
	{
		objectManager->SetState(ObjectManager::OMState::Replaying);
		objectManager->AddObject(ObjectType::UI, std::make_shared<ToLeave>(objectManager.get(), ObjectType::UI));

	} break;
	case SceneState::TransitionOut:
	{
		objectManager->SetState(ObjectManager::OMState::Transition);
		Engine::GetInstance().GetAudioManager()->StartSound("transition_begin");

		for (int i = 0; i < 20; i++)
		{
			auto fadeOut = std::make_shared<Transition>(objectManager.get(), ObjectType::EffectOverUI, false);
			if (i == 19) lastTransition = fadeOut;
			fadeOut->GetComponent<TransformComponent>()->SetPosition(32 + 64 * (19 - i), Engine::GetInstance().GetWindowSize().bottom / 2.);
			fadeOut->SetStartTimer(i * 1 / 120.);
			fadeOut->SetTimer(5.0);
			RenderInfo info{};
			info.isFixed = true;
			fadeOut->SetRenderInfo(info);
			fadeOut->SetIsDeadBySpriteEnd(false);
			objectManager->AddObject(ObjectType::EffectOverUI, fadeOut);
		}
	} break;
	case SceneState::Pause:
	{

	} break;
	}
	currState = newState;
}