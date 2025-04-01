#pragma once

#include <Windows.h>
#include <bitset>

namespace CommonUtilities
{
    class InputManager
    {
    public:
        void Update();
        bool UpdateEvents(UINT aMsg, WPARAM aWparam, LPARAM aLparam);

        bool IsKeyHeld(const int aKeyCode) const;
        bool IsKeyPressed(const int aKeyCode) const;
        bool IsKeyReleased(const int aKeyCode) const;
        bool IsCursorLocked();

        POINT GetRelativeMousePosition() const;
        POINT GetAbsoluteMousePosition() const;
        int GetScrollDelta() const;

        void CaptureMouse(HWND aWindowHandle);
        void ReleaseMouse();
        void SetCursorLocked(bool trueOrFalse);
        void SetCursorPosition(int x, int y);
        void PrintKeyEvent(int aKeyCode, const std::string& aKeyName);

    private:
        //Storing key state
        std::bitset<256> myUpdateState;
        std::bitset<256> myCurrentState;
        std::bitset<256> myLastState;

        //mouse
        POINT myUpdateMousePos = { 0, 0 };
        POINT myCurrentMouseRelativePos = { 0, 0 };
        POINT myLastMousePos = { 0, 0 };

        int myScrollDelta = 0;
        int myUpdateScrollDelta = 0;

        bool isCursorLocked = false;
    };
}
