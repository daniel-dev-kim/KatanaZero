#include "pch.h"
#include "ObjectManager.h"

#include "Object.h"
#include "Player.h"
#include "CameraComponent.h"
#include "Engine.h"
#include "AudioManager.h"
#include "Pause.h"

constexpr int OBJECT_TYPE_SIZE = static_cast<int>(ObjectType::Count);
ObjectManager::ObjectManager() 
{
}
ObjectManager::~ObjectManager() {} // <-- 정의 OK?

void ObjectManager::SetState(OMState newState)
{
	switch (newState)
	{
	case OMState::Idle:
	{
		Engine::GetInstance().waveOffset = 0.0;
		Engine::GetInstance().waveStrength = 0.0;

		recordFrameIndex = 0;
		snapshots.clear();
		recordedSounds.clear();
	} break;
	case OMState::Recording:
	{
		snapshots.resize(20000);
	} break;
	case OMState::SetUpRewinding:
	{
		recordFrameIndex--;
	} break;
	case OMState::Rewinding:
	{
		rewindSpeed = 3.;
	} break;
	case OMState::SetUpReplaying:
	{
		// 현재 존재하던 모든 오브젝트에 대해
		for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
		{
			for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
			{
				// 녹화 대상 오브젝트
				if (iter->get()->IsRecorded())
				{
					ObjectID id = reinterpret_cast<uintptr_t>(iter->get());
					// 더미 데이터가 로드된 경우, 해당 프레임에 존재하지 않았음
					if (snapshots[0][id].dummy)
					{
						// 렌더링 및 업데이트 제외
						snapshots[0][id].isDead = true;
						iter->get()->LoadSnapshot(snapshots[0][id]);
					}
					else
					{
						// 초기 시점으로 돌림
						iter->get()->LoadSnapshot(snapshots[0][id]);
					}
				}
			}
		}
		
	} break;
	case OMState::Replaying:
	{
		maxIndex = recordFrameIndex;
		recordFrameIndex = 0;
	} break;
	case OMState::Boss:
	{
		snapshots.clear();
		recordedSounds.clear();
		recordFrameIndex = 1;
	} break;
	}
	currentState = newState;
}



void ObjectManager::Initialize()
{
	objects.resize(static_cast<int>(ObjectType::Count));
	deadObjects.resize(static_cast<int>(ObjectType::Count));

}

void ObjectManager::Update(double dt)
{
	if (pauseSwitch)
	{
		pause->Update(dt);
	}
	else
	{
		switch (currentState)
		{
		case OMState::Idle:
		{
			// 업데이트 O, 스냅샷 저장 X
			for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
			{
				for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
				{
					if (!iter->get()->IsDead())
					{
						iter->get()->Update(dt);
					}
				}
			}
		} break;

		case OMState::Recording:
		{
			// 업데이트 O, 스냅샷 저장 O
			for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
			{
				for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
				{
					if (!iter->get()->IsDead())
					{
						iter->get()->Update(dt);

					}
					else if (iter->get()->IsDead() && !iter->get()->IsRecorded())
					{
						deadObjects[type].emplace_back(iter);
					}
					if (iter->get()->IsRecorded())
					{
						ObjectID id = reinterpret_cast<uintptr_t>(iter->get());
						snapshots[recordFrameIndex].emplace(id, iter->get()->GetSnapshot());
					}
				}
			}
			recordFrameIndex++;
		} break;
		case OMState::SetUpRewinding:
		{
			for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
			{
				for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
				{
					if (!iter->get()->IsDead())
					{
						iter->get()->Update(dt);
					}
				}
			}
		} break;
		case OMState::Rewinding:
		{
			Engine::GetInstance().waveOffset += 0.01;
			Engine::GetInstance().waveStrength = std::clamp(Engine::GetInstance().waveOffset / 2.f, 0.0f, 0.5f);

			for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
			{
				for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
				{
					if (iter->get()->IsRecorded())
					{
						ObjectID id = reinterpret_cast<uintptr_t>(iter->get());
						if (snapshots[recordFrameIndex][id].dummy)
						{
							if (type == 4)
							{
								int a = 0;
							}
							deadObjects[type].emplace_back(iter);
						}
						else
						{
							iter->get()->LoadSnapshot(snapshots[recordFrameIndex][id]);
						}
					}
				}
			}
			Engine::GetInstance().playerCam->Update(dt);
			recordFrameIndex -= floor(rewindSpeed);
			rewindSpeed += dt * 5.;
			if (recordFrameIndex < 0)
			{
				recordFrameIndex = 0;
				for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
				{
					for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
					{
						if (iter->get()->IsRecorded())
						{
							ObjectID id = reinterpret_cast<uintptr_t>(iter->get());
							if (snapshots[0][id].dummy)
							{
								deadObjects[type].push_back(iter);
							}
							else
							{
								iter->get()->LoadSnapshot(snapshots[0][id]);
							}
						}
					}
				}
			}
		} break;
		case OMState::SetUpReplaying:
		{
			int transition = static_cast<int>(ObjectType::EffectOverUI);
			for (auto iter = objects[transition].begin(); iter != objects[transition].end(); iter++)
			{
				iter->get()->Update(dt);
			}
			Engine::GetInstance().playerCam->Update(dt);
		} break;
		case OMState::Replaying:
		{
			for (auto name : recordedSounds[recordFrameIndex])
			{
				Engine::GetInstance().GetAudioManager()->StartSound(name, false);
			}
			for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
			{
				for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
				{
					if (iter->get()->IsRecorded())
					{
						ObjectID id = reinterpret_cast<uintptr_t>(iter->get());
						if (!snapshots[recordFrameIndex][id].dummy)
						{
							iter->get()->LoadSnapshot(snapshots[recordFrameIndex][id]);
						}
					}
				}
			}
			Engine::GetInstance().playerCam->Update(dt);
			recordFrameIndex++;
		} break;
		case OMState::Transition:
		{
			if (!objects.empty())
			{
				int transition = static_cast<int>(ObjectType::EffectOverUI);
				for (auto iter = objects[transition].begin(); iter != objects[transition].end(); iter++)
				{
					iter->get()->Update(dt);
				}
			}

		} break;
		case OMState::Boss:
		{
			for (int type = 0; type < OBJECT_TYPE_SIZE; type++)
			{
				for (auto iter = objects[type].begin(); iter != objects[type].end(); iter++)
				{
					iter->get()->Update(dt);
					if (iter->get()->IsDead())
					{
						deadObjects[type].emplace_back(iter);
					}
				}
			}
		} break;
		}
		CleanDeadObjects();
	}
}

void ObjectManager::Render()
{
	for (const auto& objList : objects)
	{
		for (const auto& obj : objList)
		{
			if(!obj->IsDead()) obj->Render();
		}
	}
	if (pauseSwitch) pause->Render();
}

void ObjectManager::AddObject(ObjectType type, std::shared_ptr<Object> object)
{
	objects[static_cast<int>(type)].push_back(object);
}

void ObjectManager::RecordSound(const std::string& name)
{
	recordedSounds[recordFrameIndex].push_back(name);
}

std::shared_ptr<Object> ObjectManager::GetFrontObject(ObjectType type)
{
	return objects[static_cast<int>(type)].empty() ? nullptr : objects[static_cast<int>(type)].front();
}

std::shared_ptr<Object> ObjectManager::GetBackObject(ObjectType type)
{
	return objects[static_cast<int>(type)].empty() ? nullptr : objects[static_cast<int>(type)].back();
}


std::list<std::shared_ptr<Object>> ObjectManager::GetObjectList(ObjectType type)
{
	return objects[static_cast<int>(type)];
}

void ObjectManager::ClearObjectList(ObjectType type)
{
	objects[static_cast<int>(type)].clear();
}

void ObjectManager::Clear()
{
	objects.clear();
}

void ObjectManager::CleanDeadObjects() {
	constexpr int typeSize = static_cast<int>(ObjectType::Count);
	
	for (int type = 0; type < typeSize; type++)
	{
		for (const auto& deadObj : deadObjects[type])
		{
			objects[type].erase(deadObj);
		}
		deadObjects[type].clear();
	}
}