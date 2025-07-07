#include "pch.h"
#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    AllocConsole();                            // 콘솔 창 생성
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);    // stdout 연결
    freopen_s(&fp, "CONIN$", "r", stdin);      // stdin 연결
    Engine& Engine = Engine::GetInstance();
    if (!Engine.Initialize(hInstance, nCmdShow)) return -1;
    Engine.Run();
    Engine.Shutdown();
    return 0;
}