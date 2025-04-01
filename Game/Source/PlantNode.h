#pragma once
#include <vector>
#include "memory.h"
#include "Transform.h"
#include "PlantParams.h"

class Leaf;
class Plant;

struct sPlantData
{
    std::vector<Leaf> leafPool;
    std::vector<std::shared_ptr<Mesh>> leafMeshes;
    std::shared_ptr<Mesh> mesh;
    int numSides = 7;
    unsigned int leafPoolCounter;
    unsigned int vertexCount = 0;
};

class PlantNode
{
public:
    PlantNode();
    ~PlantNode() = default;

    void GenerateVertexData(bool allocateVertices, sPlantData& aPlantData);
    void CreateVertices(bool allocateVertices, sPlantData& aPlantData);
    void ResetTransforms();
    void ResetNodeChildCounter();
    void ResetNodeLeafCounter();
    void SetRandomLeafScale(const sPlantParams& aPlantParam);
    void SetRadnomLeafRotation(const sPlantParams& aPlantParam);
    void SetLeafMaterial(GraphicsEngine* aGraphicsEnginePtr, std::string& aMaterialName);
    void CreateBranchTip(bool allocateVertices, sPlantParams aPlantParam, sPlantData& aPlantData, GraphicsEngine* aGraphicsEnginePtr);

#ifdef _DEBUG
    void OutputNodeTransforms(int aDepth, const  sPlantData& aPlantData);
#endif

private:
    friend class Plant;

    std::vector<unsigned int> myVertexIndexes; //All vertices exist in the mesh. With this we can access them from our children to connect the dots
    std::vector<std::shared_ptr<PlantNode>> myChildren;
    std::vector<Leaf*> myLeaves;
    PlantNode* myParent = nullptr; //if this is nullptr it must be the root
    Transform myTransform;
    Transform myTipTransform;
    float myLength = 0.0f;
    float myWidth = 0.0f;
    int myChildCounter = 0;
    int myLSystemLeafCounter = 0; //These leaves are added by finding '@' in the lsystem, NOT the ones at the tips of the tree
    int myTipVertexIndex = 0;

};