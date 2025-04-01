#pragma once
#include "GraphicsEngine.h"
#include "Mesh.h"
#include "Terrain.h"
#include "WaterPlane.h"
#include "UI.h"
#include "Camera.h"
#include "Plant.h"

namespace CommonUtilities
{
	class InputManager;
}

class World
{
public:
	World();
	~World();

	bool Init(GraphicsEngine& aGraphicsEngine);
	void Update(CommonUtilities::InputManager* aInputmanager, float aTimeDelta);
	void Render();

private:
	std::vector<std::shared_ptr<Mesh>> myMeshesToRender;
	std::vector<std::shared_ptr<Mesh>> myTransparentMeshesToRender;

	GraphicsEngine* myGraphicsEnginePtr = nullptr;
	std::shared_ptr<Terrain> myTerrain = nullptr;
	std::shared_ptr<Mesh> mySkyCube = nullptr;
	WaterPlane myWater;
	Camera myCamera;
	UI myUI;
	Plant myPlant;
};