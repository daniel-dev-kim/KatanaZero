// InputSystem.cpp
#include "pch.h"
#include "InputSystem.h"
#include "Engine.h"
static const std::unordered_map<InputKey, int> keyMap = {
    {InputKey::W, 'W'},
    {InputKey::A, 'A'},
    {InputKey::S, 'S'},
    {InputKey::D, 'D'},
    {InputKey::Shift, VK_SHIFT},
    {InputKey::Space, VK_SPACE},
    {InputKey::LButton, VK_LBUTTON},
    {InputKey::RButton, VK_RBUTTON},
    {InputKey::L, 'L'},
    {InputKey::ESC, VK_ESCAPE},
};

InputSystem::InputSystem() : keyDown(KEY_COUNT, false), wasKeyDown(KEY_COUNT, false) {}

void InputSystem::Tick(HWND hWnd) {
    wasKeyDown = keyDown;

    for (int i = 0; i < RAW_KEY_COUNT; ++i) {
        InputKey input = static_cast<InputKey>(i);
        int vk = keyMap.at(input);
        SHORT state = GetAsyncKeyState(vk);
        keyDown[i] = (state & 0x8000) != 0;
    }

    POINT pt;
    if (GetCursorPos(&pt)) {
        ScreenToClient(hWnd, &pt); // 윈도우 클라이언트 좌표로 변환
        mousePos = glm::vec2(static_cast<float>(pt.x), static_cast<float>(Engine::GetInstance().GetWindowSize().bottom - pt.y));
    }
}

bool InputSystem::IsKeyDown(InputKey key) const {
    return isKeyWork && keyDown[static_cast<int>(key)];
}

bool InputSystem::IsKeyPressed(InputKey key) const {
    int i = static_cast<int>(key);
    return isKeyWork && keyDown[i] && !wasKeyDown[i];
}

bool InputSystem::IsKeyReleased(InputKey key) const {
    int i = static_cast<int>(key);
    return isKeyWork && !keyDown[i] && wasKeyDown[i];
}   

glm::vec2 InputSystem::GetMousePos() const
{
    return mousePos;
}


void InputSystem::SetKeyWork(bool keyWork)
{
    isKeyWork = keyWork;
}
