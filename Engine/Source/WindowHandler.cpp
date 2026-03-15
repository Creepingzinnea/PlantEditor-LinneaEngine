#include "pch.h"
#include "../../Game/resource.h"
#include <dwmapi.h>
#include <string>
#include "FileLogger.h"

// Window procedure function to handle messages sent to the window----------------------------------
LRESULT CALLBACK WndProc(HWND hWND, UINT message, WPARAM wParam, LPARAM lParam);

WindowHandler* WindowHandler::instance = nullptr;

WindowHandler& WindowHandler::GetInstance(HINSTANCE aHinstance)
{
    if (instance == nullptr)
    {
        instance = new WindowHandler();

        if (aHinstance && !instance->Initialize(aHinstance))
        {
            delete instance;
            instance = nullptr;
        }
    }

    return *instance;
}


bool WindowHandler::Initialize(HINSTANCE aHinstance)
{
    //Init size with the monitors size
    myWidth = static_cast<unsigned int>(GetSystemMetrics(SM_CXSCREEN) * 0.7f); 
    myHeight = static_cast<unsigned int>(GetSystemMetrics(SM_CYSCREEN) * 0.7f);

    // Initialize the window class structure--------------------------------------------------------
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);                   // Size of the structure
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;    // Window style
    wcex.lpfnWndProc = WndProc;                         // Function to handle messages
    wcex.hInstance = aHinstance;                        // Handle to the application instance
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);      // Cursor to use
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);        // Background color
    wcex.lpszClassName = L"LinneaEngine";               // Class name for the window
    wcex.hIcon = LoadIcon(aHinstance, MAKEINTRESOURCE(IDI_GAME)); // Large icon
    wcex.hIconSm = LoadIcon(aHinstance, MAKEINTRESOURCE(IDI_SMALL));

    // Register the window class--------------------------------------------------------------------
    ATOM result = RegisterClassExW(&wcex);
    if (!result)
    {
        DWORD error = GetLastError();
        FileLogger::Get().LogError("RegisterClassExW failed. Error: " + std::to_string(error));
        return false;
    }

    // Create the window----------------------------------------------------------------------------
    myWindowHandle = CreateWindow
    (
        L"LinneaEngine",   // Window class name
        L"LinneaEngine",   // Window title

        //window styles
        WS_OVERLAPPED |
        WS_CAPTION |
        WS_SYSMENU |
        WS_MINIMIZEBOX |
        WS_MAXIMIZEBOX,

        CW_USEDEFAULT, CW_USEDEFAULT, myWidth, myHeight,    // Window position and size
        nullptr, nullptr, aHinstance, nullptr            // Parent window, menu, instance handle, and additional data
    );

    if (!myWindowHandle) // Check if the window was created successfully--------------------------------------
    {
        DWORD error = GetLastError();
        FileLogger::Get().LogError("Windowhandle is null after createWindow. Error: " + std::to_string(error));
        return false;
    }

    FileLogger::Get().Log("Window created. Width: " + std::to_string(myWidth) + "Height: " + std::to_string(myHeight));

    return true;
}

void WindowHandler::Resize(int newWidth, int newHeight)
{
    myWidth = newWidth;
    myHeight = newHeight;
    myWindowResized = true;
}

// Window procedure function to handle messages sent to the window----------------------------------
LRESULT CALLBACK WndProc(HWND hWND, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:        // Handle the window destroy message
    {
        FileLogger::Get().Log("WM_DESTROY received");
        PostQuitMessage(0); // Post a quit message to the message queue
        break;
    }
    case WM_SIZE: // Detect window resizing
    {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        WindowHandler::GetInstance().Resize(newWidth, newHeight);

        FileLogger::Get().Log("Window resized to " +std::to_string(newWidth) + "x" +std::to_string(newHeight));

        break;
    }
    default:
    {
        return DefWindowProc(hWND, message, wParam, lParam); // Pass unhandled messages to the default window procedure
    }
    }
    return 0;                   // Return zero to indicate the message was handled
}

