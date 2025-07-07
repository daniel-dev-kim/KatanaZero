#pragma once
class ObjectManager;
class Player;
class Object;
class Scene
{
public:
    enum class SceneState
    {
        TransitionIn,
        ChooseMusic,
        ClickToStart,
        ZigZag,
        Play,
        Die,
        Rewind,
        YouCanDoThis,
        Replay,
        TransitionOut,
        Pause,
    };
public:
	virtual ~Scene(); // ¼±¾ð¸¸
	virtual void Load();
	virtual void Unload() = 0;
	virtual void Render() = 0;
	virtual void Update(double dt) = 0;
    virtual void ChangeSceneState(SceneState newState) = 0;
public:

	std::shared_ptr<ObjectManager> objectManager;
	glm::vec2 sceneSize;
	std::string bgmName;
    SceneState currState;
    bool showGO;
    double zigzagTimer;
    bool FirstZigzag;
    bool done;
};