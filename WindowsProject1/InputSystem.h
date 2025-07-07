#pragma once
// InputSystem.h
enum class InputKey : int {
    W, A, S, D,
    Shift,
    Space,
    LButton,
    RButton,
    L,
    ESC,
    Count
};

static constexpr int RAW_KEY_COUNT = static_cast<int>(InputKey::Count);

class InputSystem {
public:
    InputSystem();
    void Tick(HWND hWnd);

    bool IsKeyDown(InputKey key) const;
    bool IsKeyPressed(InputKey key) const;
    bool IsKeyReleased(InputKey key) const;


    glm::vec2 GetMousePos() const;

    void SetKeyWork(bool keyWork);
    void SetMouseWork(bool mouseWork);

private:
    static constexpr int KEY_COUNT = 256;
    // InputSystem.h
    std::vector<bool> keyDown;
    std::vector<bool> wasKeyDown;

    bool isKeyWork = true;

    glm::vec2 mousePos;
};