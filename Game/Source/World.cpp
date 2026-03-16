#include "pch.h"
#include "World.h"
#include "Camera.h"
#include "TGPUtilities.hpp"
#include "FileLogger.h"
#include <InputManager.h>
#include <thread>

World::World()
{
}

World::~World()
{
}

bool World::Init(GraphicsEngine& aGraphicsEngine)
{
	myGraphicsEnginePtr = &aGraphicsEngine;
	myGraphicsEnginePtr->SetBackgroundColor(0.2f, 0.3f, 0.9f, 1.0f);
	myGraphicsEnginePtr->SetCamera(&myCamera);


	//Init shared ptrs
	myTerrain = std::make_shared<Terrain>();
	mySkyCube = std::make_shared<Mesh>();


	//TERRAIN------------------------------------------------------------------------------
	int res = 6;
#ifdef _RELEASE
	int octaves = 10;
#else
	int octaves = 4;
#endif
	float amp = 3.0f;
	float size = 100.0f;
	std::thread terrainThread(&Terrain::GenerateTerrain, myTerrain, res, octaves, amp, size);
	
	//Meshes----------------------------------------------------------------------------------------------------------------------------------------------

	//Sky cube
	mySkyCube->Init(&aGraphicsEngine, eShape::SkyBox);
	mySkyCube->LoadSingleTexture("../Assets/Textures/Skybox.png", myGraphicsEnginePtr);
	mySkyCube->LoadCustomShaders(*myGraphicsEnginePtr, "../Assets/Shaders/Skybox_PS.cso", "../Assets/Shaders/VertexShader_VS.cso");
	mySkyCube->myTransform.SetTransform({ 0, 0, 0 }, { 0, 0, 0 }, { 500,500,500 });

	//Water
	myWater.Init(myGraphicsEnginePtr);
	myWater.myTransform.TranslateY(-1.1f);

	//plant
	myPlant.transform.Translate({ 9.193f, -0.68f, -0.92f });
	myPlant.Init("../Assets/Config/PlantPresets.txt", myGraphicsEnginePtr);
	myPlant.GenerateTree();

	//add to vector
	myMeshesToRender.push_back(mySkyCube);
	myMeshesToRender.push_back(myTerrain);
	myMeshesToRender.push_back(myPlant.GetMesh());

	//Camera
	myCamera.myTransform.SetRotation({ 0, 140.0f, 0 });
	myCamera.SetTarget(&myPlant.transform, { 0, 0.5f, 0 }, 7.0f);
	CommonUtilities::Vector3<float> translation = myPlant.transform.GetTranslation();
	translation = myCamera.myTransform.GetForward() * -myCamera.GetTargetDistance();
	myCamera.myTransform.Translate(translation);

	//UI
	myUI.Init(myGraphicsEnginePtr, myPlant);

	//rest of terrain
	terrainThread.join();
	myTerrain->LoadMaterial(myGraphicsEnginePtr);
	myTerrain->LoadCustomShaders(*myGraphicsEnginePtr, "../Assets/Shaders/TerrainMaterialShader_PS.cso", "../Assets/Shaders/MaterialShader_VS.cso");
	myTerrain->myTransform.SetTransform({ 0, 0.0f, 0 }, { 0, 0, 0 }, { 1,1,1 });

	return true;
}

void World::Update(CommonUtilities::InputManager* aInputmanager, float aTimeDelta)
{
	myUI.BeginFrame(aTimeDelta);

	if (WindowHandler::GetInstance().GetWindowResized())
	{
		myCamera.UpdateRenderSize();
	}
	myCamera.Update(*aInputmanager, aTimeDelta, eMovement::orbit);

	//Move the sky with the camera for parallax effect
	vector3f translation = myGraphicsEnginePtr->GetCamera().myTransform.GetTranslation();
	mySkyCube->myTransform.TranslateX(translation.x);
	mySkyCube->myTransform.TranslateZ(translation.z);

	myGraphicsEnginePtr->SetCamera(&myCamera);
}

void World::Render()
{
	//Scene
	myGraphicsEnginePtr->RenderWithReflection(myMeshesToRender, &myWater, &myPlant.GetLeafMeshes());
	myGraphicsEnginePtr->SetBackBuffer();

	//UI
	ImTextureID textureID = reinterpret_cast<ImTextureID>(myGraphicsEnginePtr->GetMainRenderTarget().shaderResourceView.Get());
	myUI.Render(textureID, myWater, &myPlant);
}

