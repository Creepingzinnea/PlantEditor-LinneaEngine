#include "pch.h"
#include "Plant.h"
#include "GraphicsEngine.h"
#include "TGPUtilities.hpp"
#include "SerialisePlant.h"
#include <stack>

Plant::Plant()
{
	myPlantData.mesh = std::make_shared<Mesh>();
	myNodes.reserve(10000);
	myPlantData.leafPool.reserve(MAX_LEAVES);
}

void Plant::Update()
{
	if (myFullReset)
	{
		ClearTree();
		GenerateTree();
		myFullReset = false;
	}
	else
	{
		myPlantData.leafPoolCounter = 0;
		ProcessLSystem(false);
		myRoot.ResetNodeChildCounter();
		myRoot.ResetNodeLeafCounter();
		myPlantData.mesh->UpdateVertexData(myGraphicsEnginePtr);
	}

	myStateChanged = false;
}

void Plant::UpdatePreset(const std::string& aPreset)
{
	myStateChanged = true;
	myFullReset = true;

	mySelectedPreset = aPreset;
	myPreset = myPresets[aPreset];
	myParams = myPreset;
}

void Plant::FullUpdate()
{
	myStateChanged = true;
	myFullReset = true;
}

void Plant::InternalInit(const std::string aFilePath)
{
	PlantSerializer serialize;
	serialize.LoadPresets(aFilePath, myPresets);
	myPreset = myPresets["preset1"];
	myParams = myPreset;
}

void Plant::GenerateTree()
{
	myParams.lSystem.LSystemGenerate();
	ProcessLSystem(true);
	myPlantData.mesh->InitAfterVertexCreation(myGraphicsEnginePtr, "Bark");
}

void Plant::Init(const std::string aFilePath, GraphicsEngine* aGraphicsEnginePtr)
{
	myGraphicsEnginePtr = aGraphicsEnginePtr;
	PlantSerializer serialize;
	serialize.LoadPresets(aFilePath, myPresets);
	myPreset = myPresets["preset1"];
	myParams = myPreset;

#ifdef _DEBUG
	myPlantData.leafPool.resize(100, Leaf(myGraphicsEnginePtr, myParams.leafSize));
#else
	myPlantData.leafPool.resize(MAX_LEAVES, Leaf(myGraphicsEnginePtr, myParams.leafSize));
#endif
}

void Plant::ProcessLSystem(bool allocateNodes)
{
	myRoot.myTransform = transform;
	myRoot.myLength = myParams.initialLength;
	myRoot.myWidth = myParams.initialWidth;

	if (allocateNodes)
	{
		myRoot.GenerateVertexData(true, myPlantData);
	}
	else
	{
		myRoot.GenerateVertexData(false, myPlantData);
	}

	std::stack<PlantNode*> nodeStack;
	std::stack<Leaf*> leafStack;
	PlantNode* currentNode = &myRoot;

	for (char c : myParams.lSystem.GetResult())
	{
		switch (c)
		{
		case 'F': // Forward
			if (allocateNodes)
			{
				// Create new branch node
				std::shared_ptr<PlantNode> newBranch = std::make_shared<PlantNode>();

				newBranch->myParent = currentNode;
				newBranch->myLength = currentNode->myLength * myParams.generalScaleFactorHeight;
				newBranch->myWidth = currentNode->myWidth * myParams.generalScaleFactorWidth;
				newBranch->myTransform = currentNode->myTransform;
				vector3f growth = newBranch->myTransform.GetUp() * newBranch->myLength;
				newBranch->myTransform.AccumulateTranslation(growth);

				newBranch->CreateVertices(true, myPlantData);

				currentNode->myChildren.push_back(newBranch);
				currentNode = currentNode->myChildren.back().get();
			}
			else
			{
				// Update existing branch 
				if (currentNode->myChildCounter < currentNode->myChildren.size())
				{
					auto& newBranch = currentNode->myChildren[currentNode->myChildCounter];
					currentNode->myChildCounter++;

					newBranch->myLength = currentNode->myLength * myParams.generalScaleFactorHeight;
					newBranch->myWidth = currentNode->myWidth * myParams.generalScaleFactorWidth;
					newBranch->myTransform = currentNode->myTransform;

					vector3f growth = newBranch->myTransform.GetUp() * newBranch->myLength;
					newBranch->myTransform.AccumulateTranslation(growth);

					newBranch->CreateVertices(false, myPlantData);
					currentNode = newBranch.get();
				}
			}
			break;

		case '+': // Rotate right
			currentNode->myTransform.AddRotation(myParams.rotationA);
			break;

		case '-': // Rotate left
			currentNode->myTransform.AddRotation(myParams.rotationB);
			break;

		case '[': // Open branch
			nodeStack.push(currentNode);

			if (allocateNodes)
			{
				// Create new branch for branching
				std::shared_ptr<PlantNode> newBranch = std::make_shared<PlantNode>();
				newBranch->myVertexIndexes = currentNode->myVertexIndexes;
				newBranch->myParent = currentNode;

				// Update branch parameters
				newBranch->myTransform = currentNode->myTransform;
				newBranch->myLength = currentNode->myLength;
				newBranch->myWidth = currentNode->myWidth;

				currentNode->myChildren.push_back(newBranch);
				currentNode = newBranch.get();
			}
			else
			{
				// Use existing branch
				if (currentNode->myChildCounter < currentNode->myChildren.size())
				{
					auto& newBranch = currentNode->myChildren[currentNode->myChildCounter];
					currentNode->myChildCounter++;

					// Update branch parameters
					newBranch->myTransform = currentNode->myTransform;
					newBranch->myLength = currentNode->myLength;
					newBranch->myWidth = currentNode->myWidth;

					newBranch->CreateVertices(false, myPlantData);
					currentNode = newBranch.get();
				}
			}
			break;

		case ']': // Close branch
			if (!nodeStack.empty())
			{
				currentNode = nodeStack.top();
				nodeStack.pop();
			}
			break;

		case '<': // Scale height down
			currentNode->myLength *= myParams.additionalHeightModifier;
			break;

		case '>': // Scale height up
			currentNode->myLength /= myParams.additionalHeightModifier;
			break;

		case '#': // Scale width down
			currentNode->myWidth *= myParams.additionalWidthModifier;
			break;

		case '!': // Scale width up
			currentNode->myWidth /= myParams.additionalWidthModifier;
			break;

		case '&': // Swap rotation directions
		{
			vector3f tempRot = myParams.rotationA;
			myParams.rotationA = myParams.rotationB;
			myParams.rotationB = tempRot;
			break;
		}

		case '(': // Decrease rotation angle
			myParams.rotationA.x -= myParams.additionalTurningAngleIncrement;
			myParams.rotationA.y -= myParams.additionalTurningAngleIncrement;
			myParams.rotationA.z -= myParams.additionalTurningAngleIncrement;
			break;

		case ')': // Increase rotation angle
			myParams.rotationA.x += myParams.additionalTurningAngleIncrement;
			myParams.rotationA.y += myParams.additionalTurningAngleIncrement;
			myParams.rotationA.z += myParams.additionalTurningAngleIncrement;
			break;
		default:
			break;
		}
	}

	CreateTipsForTerminalBranches(&myRoot, allocateNodes);
}


void Plant::SetLeafRandomRotation()
{
	RecursiveSetLeafRandomRotation(&myRoot);
}

void Plant::SetLeafRandomScale()
{
	RecursiveSetLeafRandomScale(&myRoot);
}

void Plant::SetLeafMaterial(std::string& aMaterialName, GraphicsEngine* aGraphicsEnginePtr)
{
	RecursiveSetLeafMaterial(&myRoot, aMaterialName, aGraphicsEnginePtr);
}

void Plant::RecursiveSetLeafRandomRotation(PlantNode* aNode)
{
	aNode->SetRadnomLeafRotation(myParams);
	for (auto& child : aNode->myChildren)
	{
		RecursiveSetLeafRandomRotation(child.get());
	}
}

void Plant::RecursiveSetLeafRandomScale(PlantNode* aNode)
{
	aNode->SetRandomLeafScale(myParams);
	for (auto& child : aNode->myChildren)
	{
		RecursiveSetLeafRandomScale(child.get());
	}
}

void Plant::RecursiveSetLeafMaterial(PlantNode* aNode, std::string& aMaterialName, GraphicsEngine* aGraphicsEnginePtr)
{
	aNode->SetLeafMaterial(aGraphicsEnginePtr, aMaterialName);
	for (auto& child : aNode->myChildren)
	{
		RecursiveSetLeafMaterial(child.get(), aMaterialName, aGraphicsEnginePtr);
	}
}

#ifdef _DEBUG
void Plant::ImGuiLeafTransform()
{
	for (auto& leaf : myPlantData.leafMeshes)
	{
		leaf->myTransform.RenderImGuiAndReturnUpdate();
	}
}

void Plant::OutputResult()
{
	std::cout << "L-System Result: " << myParams.lSystem.GetResult() << std::endl;
	std::cout << "Result Length: " << myParams.lSystem.GetResult().length() << std::endl;
}
#endif

void Plant::ClearTree()
{
	myRoot.ResetTransforms();
	myRoot.ResetNodeLeafCounter();
	myRoot.ResetNodeChildCounter();
	myPlantData.leafMeshes.clear();
	myPlantData.mesh->ClearVertexData();
	myPlantData.vertexCount = 0;
	myRoot.myChildren.clear();
	myRoot.myVertexIndexes.clear();
	myPlantData.leafPoolCounter = 0;
}

void Plant::CreateTipsForTerminalBranches(PlantNode* aNode, bool allocateVertices)
{
	if (aNode->myChildren.empty())
	{
		aNode->CreateBranchTip(allocateVertices, myParams, myPlantData, myGraphicsEnginePtr);
	}
	else
	{
		for (auto& child : aNode->myChildren)
		{
			CreateTipsForTerminalBranches(child.get(), allocateVertices);
		}
	}
}



