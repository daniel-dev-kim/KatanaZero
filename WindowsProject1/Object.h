#pragma once
#include "ObjectComponent.h"
constexpr float GRAVITY = 2200.;


class ShaderProgram;
class CameraComponent;
class TextureData;
struct Afterimage {
    glm::mat4 modelMat;
    RenderInfo info;
    float lifetime;
    TextureData* texture = nullptr;
    void Render(ShaderProgram* shader, CameraComponent* camera);
};
class ObjectManager;
class Object
{
public:
    RenderInfo specialInfo{};
    bool useSpecial;
protected:    
    glm::vec2 velocity = {0, 0};
    ObjectType objType;
    ObjectManager* owner;
    bool isDead;
public:
    Object(ObjectManager*, ObjectType);
    virtual ~Object() {};
    bool hit = false;

    virtual void Update(double dt);
    virtual void Render();
    virtual void SetDead() { isDead = true; };
    void Hit(bool h)
    {
        hit = h;
    }
    bool IsRecorded();
    void AddVelocity(glm::vec2 v);
    void SetVelocity(glm::vec2 v);
    glm::vec2 GetVelocity();
    bool IsDead() { return isDead; }
    ObjectManager* GetOwner() { return owner; };
    ObjectSnapshot GetSnapshot();
    void LoadSnapshot(const ObjectSnapshot& snapshot);
public:
    template<typename T, typename... Args> 
    T* AddComponent(Args&&... args) {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->owner = this;
        T* raw = comp.get();
        components.push_back(std::move(comp));
        componentMap[typeid(T)] = raw;
        return raw;
    }

    template<typename T>
    T* GetComponent() const {
        auto it = componentMap.find(typeid(T));
        if (it != componentMap.end()) return static_cast<T*>(it->second);
        return nullptr;
    }

    template<typename T>
    void RemoveComponent()
    {
        auto it = componentMap.find(typeid(T));
        if (it != componentMap.end())
        {
            ObjectComponent* target = it->second;
            components.erase(
                std::remove_if(components.begin(), components.end(),
                    [target](const std::unique_ptr<ObjectComponent>& comp)
                    {
                        return comp.get() == target;
                    }),
                components.end()
            );
            componentMap.erase(it);
        }
    }

private:
    std::vector<std::unique_ptr<ObjectComponent>> components;
    std::unordered_map<std::type_index, ObjectComponent*> componentMap;

protected:
    class State {
    public:
        virtual void Enter(Object* object) = 0;
        virtual void Update(Object* object, double dt) = 0;
        virtual void TestForExit(Object* object) = 0;
    };
    class State_Nothing : public State {
    public:
        void Enter(Object*) override {};
        void Update(Object*, double) override {};
        void TestForExit(Object*) override {};
    };

    State_Nothing stateNothing;
    State* currState;

public:
    void ChangeState(State* newState);
    State* GetCurrState()
    {
        return currState;
    }

};

