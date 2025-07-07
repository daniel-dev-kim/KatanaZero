#pragma once
#include "Scene.h"
class Transition;
class Boss;
class Player;
class BossDoppleGanger;
class BossEye;
class Filter;
class Mutation;
class Stab;
class LineCollisionComponent;
class UI;
class Background;
class Light;
class BossScene : public Scene
{
public:
    enum class BossSceneState : int
    {
        TransitionIn = 0,
        PlayPhase1,
        PlayPhase2,
        PlayPhase3,
        PlayPhase4,
        ZigZag,
        Phase1Intro,
        Phase3Intro,
        Phase4Intro,
        Phase1Die,
        Die,
        Pause,
    };

    enum class Phase2Pattern
    {
        EllipseSpread,
        EllipseSpin,
        ColumnSpread,
        ColumnAttack,
        RandomSpread,
        RandomSpreadAttack,
    };

    enum class Phase3Pattern
    {
        TwoEyes, //2
        Colorful,//5
        Quad, //4
        FourDirFlip, //4
        FilpOnly, //3
        Trail, //5
    };
    std::shared_ptr<Background> bg2;
    std::shared_ptr<Light> firePlace;

    int phase = 1;
    BossSceneState currBossSceneState;
    double startTimer;
    LineCollisionComponent* floor;
    glm::vec2 savePos;
public:
    std::shared_ptr<Stab> stab;
    std::shared_ptr<Filter> filter;
    std::shared_ptr<Boss> boss;
    std::shared_ptr<Player> player;
    std::shared_ptr<Mutation> mutation;
    std::vector<std::shared_ptr<BossDoppleGanger>> fakeBosses;
    std::vector<std::shared_ptr<BossEye>> eyes;
public:
    Phase2Pattern phase2Pattern = Phase2Pattern::EllipseSpread;
    double phase2Timer = 0.;
    int phase2Attack = 0;
    bool retry = false;
    bool columnAttackStarted = false;
    double stay = 0.0;
    std::vector<double> columnAttackStartTimer;

    Phase3Pattern phase3Pattern = Phase3Pattern::TwoEyes;
    double phase3Timer = 0.;
    double pitch = 1.0;
    double pitchTimer = 0.0;
    double phase4Timer = 0.;

    double headTimer = 3.;
    double brainTimer = 2.;
    double stabTimer = 5.;
    double InjectionTimer = 7.;
    bool clear = false;
public:
    void ChangeTerrain();
    void Load()override;
    void Unload() override;
    void Render() override;
    void Update(double deltaTime) override;
    void ChangeSceneState(SceneState newState) override;
    void ChangeBossSceneState(BossSceneState newState);
    void ChangeBossPhase2Pattern(Phase2Pattern newPattern);
    glm::vec2 phase2Center = { 660, 580 };
    glm::vec2 PointOnEllipse(float r)
    {
        float a = 150.0f; // 가로 반지름
        float b = 50.0f; // 세로 반지름

        float px = phase2Center.x + a * cosf(r);
        float py = phase2Center.y + b * sinf(r);

        return glm::vec2(px, py);
    }

    std::vector<glm::vec2> gridPositions;
    static constexpr int bossHP = 1;
    static constexpr int DopplegangerNum = 1;
    static constexpr int ColorfulEyeNum = 1;
    static constexpr int QuadEyeNum = 1;
    static constexpr int FourDirFlipEyeNum =1;
    static constexpr int FilpOnlyEyeNum = 1;
    static constexpr int TrailEyeNum = 1;
    static constexpr int MutationHP = 3;
};

