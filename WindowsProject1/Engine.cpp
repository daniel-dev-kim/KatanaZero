#include "pch.h"
#include "Engine.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "InputSystem.h"
#include "ShaderProgram.h"
#include "Random.h"
#include "AudioManager.h"
#include "CameraComponent.h"
#include "ObjectManager.h"
#include "Object.h"
#include "SpriteManagerComponent.h"
#include "TransformComponent.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"

Engine& Engine::GetInstance() {
    static Engine instance;
    return instance;
}

void Engine::SetDeltaTimeFactor(double f) {
    dtFactor = f;
}

bool Engine::Initialize(HINSTANCE hInstance, int nCmdShow) {
    this->hInstance = hInstance;
    msg.message = WM_NULL;

    if (!InitWindow(hInstance, nCmdShow))
        return false;

    if (!InitOpenGL())
        return false;

    shader = std::make_unique<ShaderProgram>();
    shader->LoadFromFiles("Resource/Shader/Object.vert", "Resource/Shader/Object.frag");

    debugShader = std::make_unique<ShaderProgram>();
    debugShader->LoadFromFiles("Resource/Shader/Debug.vert", "Resource/Shader/Debug.frag");

    objectMaskShader = std::make_unique<ShaderProgram>();
    objectMaskShader->LoadFromFiles("Resource/Shader/objectMaskShader.vert", "Resource/Shader/objectMaskShader.frag");

    postProcessShader = std::make_unique<ShaderProgram>();
    postProcessShader->LoadFromFiles("Resource/Shader/PostProcessing.vert", "Resource/Shader/PostProcessing.frag");

    simpleDisplayShader = std::make_unique<ShaderProgram>();
    simpleDisplayShader->LoadFromFiles("Resource/Shader/SimpleDisplay.vert", "Resource/Shader/SimpleDisplay.frag");

    textureManager = std::make_unique<TextureManager>();
    textureManager->LoadTexture("default", "Resource/Texture/default.png");
    textureManager->LoadTexture("neutron", "Resource/Texture/Neutron.png");
    textureManager->LoadTexture("electron", "Resource/Texture/Electron.png");

    audioManager = std::make_unique<AudioManager>();
    audioManager->LoadAllSoundsFromFile("Resource/Sound/SoundList.txt");
    inputSystem = std::make_unique<InputSystem>();
    random = std::make_unique<Random>();
    sceneManager = std::make_unique<SceneManager>();
    sceneManager->Initialize();

    return true;
}

bool Engine::InitWindow(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSEXW wcex{};
    const wchar_t CLASS_NAME[] = L"WindowClass";

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = CLASS_NAME;

    if (!RegisterClassExW(&wcex)) {
        OutputDebugStringW(L"RegisterClassEx failed!\n");
        return false;
    }

    originalWindowSize = { 0, 0, 1280, 720 };
    windowSize = originalWindowSize;
    AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW, FALSE);
    screenWidth = originalWindowSize.right - originalWindowSize.left;
    screenHeight = originalWindowSize.bottom - originalWindowSize.top;
    hWnd = CreateWindowW(CLASS_NAME, L"My Game", WS_OVERLAPPEDWINDOW,
        100, 100,
        windowSize.right - windowSize.left,
        windowSize.bottom - windowSize.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        OutputDebugStringW(L"CreateWindow failed!\n");
        return false;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return true;
}
void Engine::InitQuadBuffer() {
    if (quadVAO != 0) return; // 이미 생성됨

    float vertices[] = {
        // pos       // texcoord
        -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  1.0f, 1.0f,

        -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // texCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &sceneVAO);
    glGenBuffers(1, &sceneVBO);
    glBindVertexArray(sceneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Engine::InitDebugLineBuffer()
{
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glEnableVertexAttribArray(0); // position only
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glBindVertexArray(0);
}

void Engine::InitMaskFrameBuffer()
{
    glGenFramebuffers(1, &maskFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, maskFBO);

    glGenTextures(1, &maskColorTexture);
    glBindTexture(GL_TEXTURE_2D, maskColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 마스크니까 NEAREST 권장
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, maskColorTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "❌ Mask FBO not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::InitSceneFrameBuffer()
{

    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    // Color attachment
    glGenTextures(1, &sceneColorTexture);
    glBindTexture(GL_TEXTURE_2D, sceneColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "FBO not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::InitTrailFrameBuffer()
{// setup 시
    glGenFramebuffers(1, &trailFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, trailFBO);
    // Color attachment
    glGenTextures(1, &trailColorTexture);
    glBindTexture(GL_TEXTURE_2D, trailColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, trailColorTexture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "trailFBO is incomplete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Engine::InitOpenGL() {
    hDC = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        24, 8, 0,
        PFD_MAIN_PLANE, 0, 0,0,0
    };

    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    // 1. 임시 컨텍스트 생성
    HGLRC tempContext = wglCreateContext(hDC);
    wglMakeCurrent(hDC, tempContext);

    // 2. GLEW 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        MessageBoxA(nullptr, "GLEW 초기화 실패", "Error", MB_OK);
        return false;
    }

    // 3. OpenGL 4.6 Core Profile 컨텍스트 생성
    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    if (!wglewIsSupported("WGL_ARB_create_context")) {
        MessageBoxA(nullptr, "WGL_ARB_create_context 지원 안됨", "Error", MB_OK);
        return false;
    }

    hGLRC = wglCreateContextAttribsARB(hDC, nullptr, attribs);
    if (!hGLRC) {
        MessageBoxA(nullptr, "OpenGL 4.6 컨텍스트 생성 실패", "Error", MB_OK);
        return false;
    }

    // 4. 임시 컨텍스트 삭제 후 실제 컨텍스트로 전환
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempContext);
    wglMakeCurrent(hDC, hGLRC);

    // 여기서 뷰포트 설정 추가
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    glViewport(0, 0, width, height);

    // 렌더링 설정
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    InitQuadBuffer();
    InitDebugLineBuffer();
    InitSceneFrameBuffer();
    InitMaskFrameBuffer();
    InitTrailFrameBuffer();

    // 1. 생성
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // 2. 플랫폼/렌더링 초기화
    ImGui_ImplWin32_Init(hWnd); // 너의 HWND 전달
    ImGui_ImplOpenGL3_Init("#version 460");
    return true;
}

void Engine::Run() {
    static auto fpsStart = std::chrono::system_clock::now();
    static int fpsCounter = 0;

    while (isRunning) {
        auto now = std::chrono::system_clock::now();
        double dt = std::chrono::duration<double>(now - lastTick).count();

        if (dt > FramePurpose * 2) dt = FramePurpose;
        if (dt >= FramePurpose) {
            lastTick = now;
            fpsCounter++;

            if (std::chrono::duration<double>(now - fpsStart).count() > 5.0) {
                std::cout << "FPS: " << fpsCounter / 5.0 << std::endl;
                fpsCounter = 0;
                fpsStart = now;
            }
            audioManager->Update();
            inputSystem->Tick(hWnd);
            hitStopTimer++;
            sceneManager->Update(hitStopTimer < HITSTOP_DUR ? 0 : dt * dtFactor);
            Render();

            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) return;
                if (!TranslateAccelerator(msg.hwnd, hAccel, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
    }
}

void Engine::Render() {
    // ---------------------------------------
    // 1. Mask Pass - Player와 Enemy만 마스킹함.
    // ---------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, maskFBO);
    glViewport(0, 0, screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    auto enemies = sceneManager->activeScene->objectManager->GetObjectList(ObjectType::Enemy);
    for (auto enemy : enemies)
    {
        objectMaskShader->Use();
        glActiveTexture(GL_TEXTURE0);
        auto textureData = enemy->GetComponent<SpriteManagerComponent>()->GetCurrentSprite()->GetCurrentData();
        glBindTexture(GL_TEXTURE_2D, textureData->textureID);
        objectMaskShader->SetUniform("uObjectTexture", 0); // 정수 슬롯 번호
    
        glm::vec2 texSize = glm::vec2(textureData->width, textureData->height);
        glm::mat4 m = glm::scale(glm::mat4(1.0f), { texSize, 1. });
   
        glm::mat4 model = enemy->GetComponent<TransformComponent>()->GetModelMatrix();

        objectMaskShader->SetUniform("uModel", model * m);
        objectMaskShader->SetUniform("uView", playerCam->GetViewMatrix());
        objectMaskShader->SetUniform("uProjection", playerCam->GetProjectionMatrix());

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    auto player = sceneManager->activeScene->objectManager->GetFrontObject(ObjectType::Player);
    if (player)
    {
        objectMaskShader->Use();
        glActiveTexture(GL_TEXTURE0);
        auto textureData = player->GetComponent<SpriteManagerComponent>()->GetCurrentSprite()->GetCurrentData();

        glBindTexture(GL_TEXTURE_2D, textureData->textureID);
        objectMaskShader->SetUniform("uObjectTexture", 0); // 정수 슬롯 번호
        glm::vec2 texSize = glm::vec2(textureData->width, textureData->height);
        glm::mat4 m = glm::scale(glm::mat4(1.0f), { texSize, 1. });

        glm::mat4 model = player->GetComponent<TransformComponent>()->GetModelMatrix();
        objectMaskShader->SetUniform("uModel", model * m);
        objectMaskShader->SetUniform("uView", playerCam->GetViewMatrix());
        objectMaskShader->SetUniform("uProjection", playerCam->GetProjectionMatrix());
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }



    // ---------------------------------------
    // 2. Normal Pass - 전체 렌더
    // ---------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glViewport(0, 0, screenWidth, screenHeight);
    if (uPersistentTrails)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    sceneManager->Render();

    if (uPersistentTrails) {
        // ---------------------------------------
        // 잔상 모드: trailFBO에 누적
        // ---------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, trailFBO);
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        postProcessShader->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneColorTexture);
        postProcessShader->SetUniform("screenTexture", 0);
        postProcessShader->SetUniform("waveStrength", waveStrength);
        postProcessShader->SetUniform("waveWidth", waveWidth);
        postProcessShader->SetUniform("waveOffset", waveOffset);

        postProcessShader->SetUniform("zigzagStrength", zigzagStrength);
        postProcessShader->SetUniform("zigzagWidth", zigzagWidth);
        postProcessShader->SetUniform("zigzagOffset", zigzagOffset);
        postProcessShader->SetUniform("grayscale", mono);

        postProcessShader->SetUniform("uColorCycleFull", uColorCycleFull);
        postProcessShader->SetUniform("uColorCycleQuadrant", uColorCycleQuadrant);
        postProcessShader->SetUniform("uFlipRenderWithColor", uFlipRenderWithColor);
        postProcessShader->SetUniform("uFlipYOnly", uFlipYOnly);
        postProcessShader->SetUniform("uPersistentTrails", uPersistentTrails);
        postProcessShader->SetUniform("uTime", uTime);

        postProcessShader->SetUniform("uGlitchEnabled", uGlitchEnabled);

        uTime += 0.01;

        glBindVertexArray(sceneVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //glDisable(GL_BLEND);

        // 최종 출력: trail 텍스처
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        simpleDisplayShader->Use(); // 단순 출력용 셰이더
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, trailColorTexture);
        simpleDisplayShader->SetUniform("screenTexture", 0);

        glBindVertexArray(sceneVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    else {
        // ---------------------------------------
        // 일반 모드: 바로 화면에 포스트 프로세싱
        // ---------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        postProcessShader->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneColorTexture);
        postProcessShader->SetUniform("screenTexture", 0);
        postProcessShader->SetUniform("waveStrength", waveStrength);
        postProcessShader->SetUniform("waveWidth", waveWidth);
        postProcessShader->SetUniform("waveOffset", waveOffset);

        postProcessShader->SetUniform("zigzagStrength", zigzagStrength);
        postProcessShader->SetUniform("zigzagWidth", zigzagWidth);
        postProcessShader->SetUniform("zigzagOffset", zigzagOffset);
        postProcessShader->SetUniform("grayscale", mono);

        postProcessShader->SetUniform("uColorCycleFull", uColorCycleFull);
        postProcessShader->SetUniform("uColorCycleQuadrant", uColorCycleQuadrant);
        postProcessShader->SetUniform("uFlipRenderWithColor", uFlipRenderWithColor);
        postProcessShader->SetUniform("uFlipYOnly", uFlipYOnly);
        postProcessShader->SetUniform("uPersistentTrails", uPersistentTrails);
        postProcessShader->SetUniform("uTime", uTime);
        
        postProcessShader->SetUniform("uGlitchEnabled", uGlitchEnabled);
        postProcessShader->SetUniform("uGlitchSeed", uGlitchSeed);

        uTime += 0.01;

        glBindVertexArray(sceneVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplWin32_NewFrame();
    //ImGui::NewFrame();
    //ImGui::Begin("Debug Texture");
    //ImGui::Image((ImTextureID)(intptr_t)sceneColorTexture,
    //     ImVec2(256, 256),
    //     ImVec2(0, 1),   // UV 좌하단
    //     ImVec2(1, 0));  // UV 우상단        
    //ImGui::End();
    //ImGui::Render();
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    SwapBuffers(hDC);
}

void Engine::Shutdown() {
    if (hGLRC) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hGLRC);
        hGLRC = nullptr;
    }
    if (hDC) {
        ReleaseDC(hWnd, hDC);
        hDC = nullptr;
    }
    DestroyWindow(hWnd);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

ShaderProgram* Engine::GetDebugShaderProgram()
{
    return debugShader.get();
}

ShaderProgram* Engine::GetShaderProgram()
{
    return shader.get();
}

AudioManager* Engine::GetAudioManager()
{
    return audioManager.get();
}

TextureManager* Engine::GetTextureManager() { return textureManager.get(); }
InputSystem* Engine::GetInputSystem() { return inputSystem.get(); }
Random* Engine::GetRandom() { return random.get(); }
SceneManager* Engine::GetSceneManager() { return sceneManager.get(); }
RECT Engine::GetWindowSize() { return windowSize; }
RECT Engine::GetOriginalWindowSize() { return originalWindowSize; }

void Engine::HitStop()
{
    hitStopTimer = 0;
}

LRESULT CALLBACK Engine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
