#pragma once
#include <windows.h>

class WindowHandler
{
public:
    WindowHandler(const WindowHandler&) = delete;
    WindowHandler& operator=(const WindowHandler&) = delete;
    static WindowHandler& GetInstance(HINSTANCE aHinstance = nullptr);
    bool Initialize(HINSTANCE aHinstance);

    inline HWND* GetWindowHandle() { return &myWindowHandle; }
    inline unsigned int GetWidth() const { return myWidth; }
    inline unsigned int GetHeight() const { return myHeight; }
    inline bool GetWindowResized() { return myWindowResized; }
    inline void PostFrame() { myWindowResized = false; }

    void Resize(int newWidth, int newHeight);

private:
    WindowHandler() = default;    
    ~WindowHandler() = default;   
    static WindowHandler* instance;
    HWND myWindowHandle = nullptr;
    unsigned int myWidth = 0;
    unsigned int myHeight = 0;
    bool myWindowResized = false;
};