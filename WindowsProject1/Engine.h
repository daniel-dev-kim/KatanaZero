#pragma once
class TextureManager;
class SceneManager;
class InputSystem;
class CameraComponent;
class ShaderProgram;
class Random;
class AudioManager;
class Engine
{
public:
    static Engine& GetInstance();
    void SetDeltaTimeFactor(double f);
    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Shutdown();

    CameraComponent* playerCam = nullptr;
    bool debug = false;
    bool pause = false;
    bool mono = false;
    ShaderProgram* GetDebugShaderProgram();
    ShaderProgram* GetShaderProgram();

    AudioManager* GetAudioManager();
    TextureManager* GetTextureManager();
    InputSystem* GetInputSystem();
    Random* GetRandom();
    SceneManager* GetSceneManager();
    RECT GetWindowSize();
    RECT GetOriginalWindowSize();
    double dtFactor = 1.0;
    int hitStopTimer = 0;
    void HitStop();

    GLuint maskFBO, maskColorTexture;
    GLuint sceneFBO, sceneColorTexture, rbo;
    GLuint trailFBO, trailColorTexture;
    GLuint sceneVAO, sceneVBO;

private:
    Engine() = default;
    bool InitWindow(HINSTANCE hInstsance, int nCmdShow);
    void InitQuadBuffer();
    void InitDebugLineBuffer();
    void InitMaskFrameBuffer();
    void InitSceneFrameBuffer();
    void InitTrailFrameBuffer();
    bool InitOpenGL();
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void Render();

private:
    HINSTANCE hInstance;
    HWND hWnd;
    HACCEL hAccel;
    MSG msg{};
    RECT windowSize{};
    RECT originalWindowSize{};

    bool isRunning = true;
    std::chrono::system_clock::time_point lastTick;
    static constexpr double FramePurpose = 1.0 / 144.0;
    int screenWidth;
    int screenHeight;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<InputSystem> inputSystem;
    std::unique_ptr<Random> random;
    std::unique_ptr<AudioManager> audioManager;
    std::unique_ptr<ShaderProgram> shader;
    std::unique_ptr<ShaderProgram> debugShader;
    std::unique_ptr<ShaderProgram> objectMaskShader;
    std::unique_ptr<ShaderProgram> postProcessShader;
    std::unique_ptr<ShaderProgram> simpleDisplayShader;
    HDC hDC = nullptr;
    HGLRC hGLRC = nullptr;

public:

    float waveStrength = 0.0;
    float waveWidth = 40.f;
    float waveOffset = 0.f;

    float zigzagStrength = 0.0f;
    float zigzagWidth = 40.f;
    float zigzagOffset = 0.f;

    
    bool uColorCycleFull = false;
    bool uColorCycleQuadrant = false;
    bool uFlipRenderWithColor = false;
    bool uFlipYOnly = false;
    bool uPersistentTrails = false;
    
    float uTime = 0.0;

    bool uGlitchEnabled = false;
    float uGlitchSeed = 1.0;
};
