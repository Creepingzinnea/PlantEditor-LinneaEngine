#pragma once
#include <imgui_docking/imgui_internal.h>
#include <imgui_docking/imgui.h>
#include <unordered_map>
#include "WindowHandler.h"
#include "BufferData.h"
#include "Light.h"
#include "WaterPlane.h"

class GraphicsEngine;
class Plant;

class UI
{
public:
	UI() = default;
	~UI() = default;

	void Render(const ImTextureID aTextureID, WaterPlane& aWaterPlane, Plant* aPlant);
	void Init(GraphicsEngine* aGraphicsEngine, Plant& aPlant);
	void BeginFrame(float aDeltaTime);

private:
	void SetupDockSpace();
	void SetupInitialDockingLayout();
	void RenderSceneWindow(const ImTextureID aTextureID);
	void RenderPlantWindow(Plant* aPlant);
	void RenderLeafWindow(Plant* aPlant);
	void RenderWorldWindow(WaterPlane& aWaterPlane);
	void RenderThemesWindow();
	void DisplayWaitingInfo(bool displayBelow = false);

	//plant
	void GeneralPlant(Plant* aPlant);
	void UILSystem(Plant* aPlant);
	void Presets(Plant* aPlant);
	void GrammarWindow();

	GraphicsEngine* myGraphicsEnginePtr = nullptr;
	TerrainBuffer myTerrainBuffer;
	directionalLight myDirectionalLight;
	ambientLight myAmbientLight;
	std::string presetName;
	bool firstTime = true;
	bool showGrammar = false;
	bool postponeOneframe = false;
	float myDeltaTime = 0.0f;
	float myTextTimer = 0.0f;
	float mySetTextTime = 10.0f;

	LSystem myInputLsystem;
	std::string myInputLeafMesh = "";
};
