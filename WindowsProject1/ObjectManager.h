#pragma once
class Object;
class Pause;
class ObjectManager
{
public:
	enum class OMState {
		Idle,      // Updates happen, no snapshots are saved.
		Recording, // Updates happen, snapshots are saved (dead objects marked but not removed).
		SetUpRewinding,
		Rewinding, // Snapshots loaded in reverse (objects removed if no snapshot exists).
		SetUpReplaying,
		Replaying,  // Snapshots played forward (no new updates; dummy snapshots mark object death).
		Transition,
		Boss,
	};
private:
	OMState currentState = OMState::Idle;
	int recordFrameIndex;
	int maxIndex;
	double rewindSpeed;
	//double snapshotTimer;
public:	
	int GetFrameIndex() { return recordFrameIndex; };
	int GetMaxIndex() { return maxIndex; }
	void SetState(OMState newState);

	ObjectManager();
	~ObjectManager();
	void Initialize();
	void Update(double dt);
	void Render();
	void AddObject(ObjectType type, std::shared_ptr<Object> object);
	void RecordSound(const std::string& name);
	std::shared_ptr<Object> GetFrontObject(ObjectType type);
	std::shared_ptr<Object> GetBackObject(ObjectType type);
	std::shared_ptr<Pause> pause;
	bool pauseSwitch = false;
	std::list<std::shared_ptr<Object>> GetObjectList(ObjectType type);
	void ClearObjectList(ObjectType type);
	void Clear();
private:
	void CleanDeadObjects();
private:
	using ObjectID = uintptr_t;
	std::vector<std::list<std::shared_ptr<Object>>> objects;
	std::vector<std::unordered_map<ObjectID, ObjectSnapshot>> snapshots;
	std::vector<std::vector<std::list<std::shared_ptr<Object>>::iterator>> deadObjects;
	std::unordered_map<int, std::vector<std::string>> recordedSounds;
};