#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "pch.h"
#include <InputManager.h>
#include <Timer.h>
#include <GraphicsEngine.h>
#include <WindowHandler.h>
#include <imgui_docking/imgui.h>
#include <FileLogger.h>
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

LONG WINAPI TopLevelExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo)
{
	if (pExceptionInfo && pExceptionInfo->ExceptionRecord)
	{
		DWORD code = pExceptionInfo->ExceptionRecord->ExceptionCode;
		void* addr = pExceptionInfo->ExceptionRecord->ExceptionAddress;

		FileLogger::Get().LogError("Program crashed! Exception code: 0x" +std::to_string(code) +
			", Address: " +std::to_string(reinterpret_cast<uintptr_t>(addr)));
		FileLogger::Get().Flush();
	}
	else
	{
		FileLogger::Get().LogError("Program crashed due to an unknown exception :(");
		FileLogger::Get().Flush();
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

int APIENTRY wWinMain//-----------------------------------------------------------------------------
(
	_In_ HINSTANCE hInstance,  // Handle to the application instance
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,               // Command line for the application
	_In_ int nCmdShow          // Show window flag
)
{
	SetUnhandledExceptionFilter(TopLevelExceptionHandler);

	try
	{

		// Allocate console in debug mode
#ifdef _DEBUG
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		std::cout << "Debug" << std::endl;
#endif

		//timer and input and utilities
		CommonUtilities::Timer* timer = new CommonUtilities::Timer;
		CommonUtilities::InputManager* inputManager = new CommonUtilities::InputManager;
		Random rand;
		rand.Initialize();

		//window
		FileLogger::Get().Log("Starting window initialization");
		FileLogger::Get().Flush();
		if (WindowHandler::GetInstance().Initialize(hInstance))
		{
			FileLogger::Get().Log("window initialization successfull!");
		}
		else
		{
			FileLogger::Get().LogError("window initialization failed.");
		}
		ShowWindow(*WindowHandler::GetInstance().GetWindowHandle(), nCmdShow); // Show the window
		UpdateWindow(*WindowHandler::GetInstance().GetWindowHandle());         // Update the window's client area

		//graphics engine
		FileLogger::Get().Log("Starting graphics-engine initialization");
		FileLogger::Get().Flush();
		GraphicsEngine graphicsEngine;
		if (graphicsEngine.Initialize(timer))
		{
			FileLogger::Get().Log("graphics-engine initialization successfull!");
		}
		else
		{
			FileLogger::Get().LogError("graphics-engine initialization failed.");
		}

		//imgui
		FileLogger::Get().Log("Starting ImGui initialization");
		FileLogger::Get().Flush();
		if (ImGuiInterface::GetInstance().Init(&graphicsEngine, *WindowHandler::GetInstance().GetWindowHandle()))
		{
			FileLogger::Get().Log("ImGui initialization successfull!");
		}
		else
		{
			FileLogger::Get().LogError("ImGui initialization failed");
		}

		//game world
		FileLogger::Get().Log("Starting game-world initialization");
		FileLogger::Get().Flush();
		World gameWorld;
		if (gameWorld.Init(graphicsEngine))
		{
			FileLogger::Get().Log("game-world initialization successfull!");
		}
		else
		{
			FileLogger::Get().LogError("game-world initialization failed.");
		}

		// Main loop-------------------------------------------------------------------------------------
		MSG msg = {};
		bool shouldRun = true;
		FileLogger::Get().Log("Running game...");
		FileLogger::Get().Flush();
		while (shouldRun)
		{
			if (!shouldRun)
			{
				FileLogger::Get().Log("Stopped running game");
				FileLogger::Get().Flush();
				break;
			}


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
		FileLogger::Get().Log("Exiting program. Last message ID: " + std::to_string(msg.message) +
			", wParam: " + std::to_string(msg.wParam) +
			", lParam: " + std::to_string(msg.lParam));

		return (int)msg.wParam;




	}//try
	catch (const std::exception& e)
	{
		FileLogger::Get().LogError(std::string("Unhandled std::exception: ") + e.what());
		FileLogger::Get().Flush();
	}
	catch (...)
	{
		FileLogger::Get().LogError("Unhandled unknown exception!");
		FileLogger::Get().Flush();
	}

	return -1;
}



