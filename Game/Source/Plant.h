#pragma once
#include <Mesh.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "LSystem.h"
#include "Leaf.h"
#include "PlantNode.h"
#define MAX_LEAVES 10000

class GraphicsEngine;

class Plant
{
public:
    Plant();
    ~Plant() = default;

    void GenerateTree();
    void Init(const std::string aFilePath, GraphicsEngine* aGraphicsEnginePtr);
    inline std::vector<std::shared_ptr<Mesh>>& GetLeafMeshes() { return myPlantData.leafMeshes; };
    void SetLeafRandomRotation();
    void SetLeafRandomScale();
    void SetLeafMaterial(std::string& aMaterialName, GraphicsEngine* aGraphicsEnginePtr);
    void Update();
    void UpdatePreset(const std::string& aPreset);
    void FullUpdate();
#ifdef _DEBUG
    void ImGuiLeafTransform();
#endif

    inline const std::shared_ptr<Mesh>& GetMesh() { return myPlantData.mesh; };

    sPlantParams myPreset = {};
    sPlantParams myParams = {};
    std::unordered_map<std::string, sPlantParams> myPresets;
    std::string mySelectedPreset = "preset1";
    Transform transform;
    bool myStateChanged = false;

private:
    void InternalInit(const std::string aFilePath);
    void RecursiveSetLeafRandomRotation(PlantNode* aNode);
    void RecursiveSetLeafRandomScale(PlantNode* aNode);
    void RecursiveSetLeafMaterial(PlantNode* aNode, std::string& aMaterialName, GraphicsEngine* aGraphicsEnginePtr);
    void ClearTree();
    void CreateTipsForTerminalBranches(PlantNode* aNode, bool allocateVertices);
    void ProcessLSystem(bool allocateNodes);

#ifdef _DEBUG
    void OutputResult();
#endif

    sPlantData myPlantData;
    GraphicsEngine* myGraphicsEnginePtr = nullptr;
    std::vector<PlantNode> myNodes;
    PlantNode myRoot;
    bool myFullReset = false;
};