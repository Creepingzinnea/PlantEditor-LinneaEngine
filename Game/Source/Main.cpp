#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "pch.h"
#include <InputManager.h>
#include <Timer.h>
#include <GraphicsEngine.h>
#include <WindowHandler.h>
#include <imgui_docking/imgui.h>
#include "Main.h"
#include "World.h"
#include "TGPUtilities.hpp"
#include "ImguiInterface.h"
#include "Random.h"


#pragma region handleInput
void HandleInput(MSG& aMsg, CommonUtilities::InputManager* aInputmanager, bool& aShouldRun)
{
	// Process messages in the message queue
	while (PeekMessage(&aMsg, nullptr, 0, 0, PM_REMOVE))
	{
		if (aMsg.message == WM_QUIT) { aShouldRun = false; break; }

		//Imgui input
		extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
            HWND aImguihWnd, UINT aImguimsg, WPARAM aImguiwParam, LPARAM aImguilParam);
		ImGui_ImplWin32_WndProcHandler(*WindowHandler::GetInstance().GetWindowHandle(), aMsg.message, aMsg.wParam, aMsg.lParam);

		//Input manager
		aInputmanager->UpdateEvents(aMsg.message, aMsg.wParam, aMsg.lParam);

		TranslateMessage(&aMsg);
		DispatchMessage(&aMsg);
		DefWindowProc(*WindowHandler::GetInstance().GetWindowHandle(), aMsg.message, aMsg.wParam, aMsg.lParam);
	}
}
#pragma endregion //HandleInput function

int APIENTRY wWinMain//-----------------------------------------------------------------------------
(
	_In_ HINSTANCE hInstance,  // Handle to the application instance
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,               // Command line for the application
	_In_ int nCmdShow          // Show window flag
)
{

	// Allocate console in debug mode
#ifdef _DEBUG
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	std::cout << "Debug" << std::endl;
#endif

	//timer and input adn utilities
	CommonUtilities::Timer* timer = new CommonUtilities::Timer;
	CommonUtilities::InputManager* inputManager = new CommonUtilities::InputManager;
	Random rand;
	rand.Initialize();

	//window
	WindowHandler::GetInstance().Initialize(hInstance);
	ShowWindow(*WindowHandler::GetInstance().GetWindowHandle(), nCmdShow); // Show the window
	UpdateWindow(*WindowHandler::GetInstance().GetWindowHandle());         // Update the window's client area

	//graphics engine
	GraphicsEngine graphicsEngine;
	graphicsEngine.Initialize(timer);

	//imgui
	ImGuiInterface::GetInstance().Init(&graphicsEngine, *WindowHandler::GetInstance().GetWindowHandle());

	//game world
	World gameWorld;
	if (!gameWorld.Init(graphicsEngine)) { return 3; };


	// Main loop-------------------------------------------------------------------------------------
	MSG msg = {};
	bool shouldRun = true;
	while (shouldRun)
	{
		if (!shouldRun) break;

		//input handling--------------------------------------------
		inputManager->Update();
		HandleInput(msg, inputManager, shouldRun);

		//Pre frame-------------------------------------------------
		graphicsEngine.PreFrame();
		ImGuiInterface::GetInstance().PreFrame();

		//general Update--------------------------------------------
		timer->Update();
		gameWorld.Update(inputManager, timer->GetDeltaTime());

		//Render----------------------------------------------------
		gameWorld.Render();
		ImGuiInterface::GetInstance().EndFrame();

		//present frame---------------------------------------------
		graphicsEngine.PresentFrame();


		//Post frame------------------------------------------------
		WindowHandler::GetInstance().PostFrame();
	}

	//clean up
	delete timer;
	delete inputManager;


	// Free the console when done
#ifdef _DEBUG
	FreeConsole();
#endif

	//exit
	return (int)msg.wParam;
}



