#include "pch.h"
#include "Plant.h"
#include "PlantNode.h"
#include "Random.h"
#include <TGPUtilities.hpp>


PlantNode::PlantNode()
{
	myLeaves.reserve(100);
}

void PlantNode::GenerateVertexData(bool allocateVertices, sPlantData& aPlantData)
{
	const float angleStep = 2.0f * 3.14159265358979f / aPlantData.numSides;

	for (int i = 0; i < aPlantData.numSides; ++i)
	{
		float angle = i * angleStep;
		float x = cos(angle) * myWidth;
		float y = 0;
		float z = sin(angle) * myWidth;

		vector3f localPos = vector3f(x, y, z);
		myTransform.TransformPoint(localPos);

		vector3f localNormal = vector3f(cos(angle), 0, sin(angle));
		vector3f normal = myTransform.GetRotationMatrix() * localNormal;
		normal.Normalize();

		float uStepX = (static_cast<float>(i) / aPlantData.numSides) * 2.0f;
		float uStepY = 1.0f / myWidth;
		vector2f uv = { uStepX, uStepY };

		if (allocateVertices)
		{
			sVertex v;
			v.pos = localPos;
			v.normal = normal;
			v.uv = uv;
			aPlantData.mesh->AddVertex(v);
		}
		else
		{
			sVertex* existingVertex = aPlantData.mesh->GetVertex(myVertexIndexes[i]);
			existingVertex->pos = localPos;
			existingVertex->normal = normal;
			existingVertex->uv = uv;
		}
		myVertexIndexes.push_back(aPlantData.vertexCount++);
	}
}

void PlantNode::CreateVertices(bool allocateVertices, sPlantData& aPlantData)
{
	GenerateVertexData(allocateVertices, aPlantData);

	if (allocateVertices)
	{
		// Generate indices for the mesh
		for (int i = 0; i < myVertexIndexes.size(); ++i)
		{
			unsigned int parentIndex1 = myParent->myVertexIndexes[i];
			unsigned int parentIndex2 = myParent->myVertexIndexes[(i + 1) % aPlantData.numSides];
			unsigned int childIndex = myVertexIndexes[i];
			unsigned int nextChildIndex = myVertexIndexes[(i + 1) % aPlantData.numSides];

			if (parentIndex1 < aPlantData.vertexCount && parentIndex2 < aPlantData.vertexCount &&
				childIndex < aPlantData.vertexCount && nextChildIndex < aPlantData.vertexCount)
			{
				aPlantData.mesh->AddIndex(parentIndex1);
				aPlantData.mesh->AddIndex(childIndex);
				aPlantData.mesh->AddIndex(parentIndex2);

				aPlantData.mesh->AddIndex(parentIndex2);
				aPlantData.mesh->AddIndex(childIndex);
				aPlantData.mesh->AddIndex(nextChildIndex);
			}
			else
			{
				TGPUtilities::CustomdebugOutput("Invalid indexing when generating plant");
			}
		}
	}
}

void PlantNode::ResetTransforms()
{
	CommonUtilities::Matrix4x4<float> identity;
	myTransform.myMatrix = identity;

	for (auto& child : myChildren)
	{
		child->ResetTransforms();
	}
}

void PlantNode::ResetNodeChildCounter()
{
	if (this == nullptr) { return; }

	myChildCounter = 0;

	for (std::shared_ptr<PlantNode> child : myChildren)
	{
		child->ResetNodeChildCounter();
	}
}

void PlantNode::ResetNodeLeafCounter()
{
	if (this == nullptr) { return; }

	myLSystemLeafCounter = 0;

	for (std::shared_ptr<PlantNode> child : myChildren)
	{
		child->ResetNodeLeafCounter();
	}
}

void PlantNode::SetRandomLeafScale(const sPlantParams& aPlantParam)//woaw :) 
{
	for (Leaf* leaf : myLeaves)
	{
		float random = Random::GetRandomFloat(-aPlantParam.randomLeafScale, aPlantParam.randomLeafScale);
		leaf->CalculateScaleFactor(random);
	}
}

void PlantNode::SetRadnomLeafRotation(const sPlantParams& aPlantParam)
{
	for (Leaf* leaf : myLeaves)
	{
		float random = Random::GetRandomFloat(-aPlantParam.randomLeafRotation, aPlantParam.randomLeafRotation);
		CommonUtilities::Vector3<float> leafRotation;
		leafRotation.x = aPlantParam.leafRotation.x + random;
		leafRotation.y = aPlantParam.leafRotation.y + random;
		leafRotation.z = aPlantParam.leafRotation.z + random;

		leaf->SetRandomRotation(leafRotation);
	}
}

void PlantNode::SetLeafMaterial(GraphicsEngine* aGraphicsEnginePtr, std::string& aMaterialName)
{
	for (Leaf* leaf : myLeaves)
	{
		leaf->SetMaterial(aGraphicsEnginePtr, aMaterialName);
	}
}

void PlantNode::CreateBranchTip(bool allocateVertices, sPlantParams aPlantParam, sPlantData& aPlantData, GraphicsEngine* aGraphicsEnginePtr)
{
	// Create a single vertex at the tip													
	vector3f tipPosition = myTransform.GetTranslation() + (myTransform.GetUp() * (myLength * aPlantParam.branchSpikyness));

	// Normal points in the same direction as the branch
	vector3f tipNormal = myTransform.GetUp();
	tipNormal.Normalize();

	if (allocateVertices)
	{
		sVertex v;
		v.pos = tipPosition;
		v.normal = tipNormal;
		v.uv = vector2f(0.5f, 0.0f); // Center of UV space
		aPlantData.mesh->AddVertex(v);


		unsigned int tipIndex = aPlantData.vertexCount++;
		myTipVertexIndex = tipIndex;

		// Create triangles connecting the tip to the rim vertices
		for (int i = 0; i < aPlantData.numSides; ++i)
		{
			unsigned int currentIndex = myVertexIndexes[i];
			unsigned int nextIndex = myVertexIndexes[(i + 1) % aPlantData.numSides];

			aPlantData.mesh->AddIndex(currentIndex);
			aPlantData.mesh->AddIndex(tipIndex);
			aPlantData.mesh->AddIndex(nextIndex);
		}

		//Add leaves
		if (aPlantParam.useLeaves && aPlantData.leafPoolCounter < MAX_LEAVES)
		{
			Leaf* leaf = nullptr;
			if (aPlantData.leafPoolCounter > aPlantData.leafPool.size() - 1)
			{
				aPlantData.leafPool.emplace_back(Leaf(aGraphicsEnginePtr, aPlantParam.leafSize));
			}
			leaf = &aPlantData.leafPool[aPlantData.leafPoolCounter++];

			myTipTransform.SetRotation(aPlantParam.leafRotation + leaf->GetRandomRotation());
			myTipTransform.Translate(tipPosition);
			leaf->UpdateLeaf(myTipTransform, aPlantParam.leafSize);

			leaf->isAtTipOfBranch = true;

			myLeaves.push_back(leaf);
			aPlantData.leafMeshes.push_back(leaf->myMesh);
		}
	}
	else
	{
		sVertex* existingVertex = aPlantData.mesh->GetVertex(myTipVertexIndex);
		existingVertex->pos = tipPosition;
		existingVertex->normal = tipNormal;

		//Update leaves
		if (aPlantParam.useLeaves && aPlantData.leafPoolCounter < MAX_LEAVES)
		{
			for (Leaf* leafPtr : myLeaves)
			{
				if (leafPtr->isAtTipOfBranch == true)
				{
					myTipTransform.SetRotation(aPlantParam.leafRotation + leafPtr->GetRandomRotation());
					myTipTransform.Translate(tipPosition);
					leafPtr->UpdateLeaf(myTipTransform, aPlantParam.leafSize);
				};
			}
		}
	}

}

#ifdef _DEBUG
void PlantNode::OutputNodeTransforms(int aDepth, const  sPlantData& aPlantData)
{
	static int depthCounter = 0;
	depthCounter++;
	if (depthCounter >= aDepth)
	{
		depthCounter = 0;
		return;
	};
	std::string indent(aDepth * 2, ' ');

	// Print node details
	std::cout << indent << "Node Depth: " << aDepth
		<< ", Length: " << myLength
		<< ", Width: " << myWidth
		<< "\n" << std::endl;

	// Print accumulated transform details
	std::cout << indent << "Accumulated Transform Matrix:" << std::endl;
	for (int row = 0; row < 4; ++row)
	{
		std::cout << indent;
		for (int col = 0; col < 4; ++col)
		{
			std::cout << myTransform.myMatrix(row + 1, col + 1) << "\t";
		}
		std::cout << "\n" << std::endl;
	}

	// Print vertex positions if available
	std::cout << indent << "Vertex Positions:" << std::endl;
	for (unsigned int idx : myVertexIndexes)
	{
		sVertex* v = aPlantData.mesh->GetVertex(idx);
		std::cout << indent << "  (" << v->pos.x << ", " << v->pos.y << ", " << v->pos.z << ")" << "\n" << std::endl;
	}
}
#endif
