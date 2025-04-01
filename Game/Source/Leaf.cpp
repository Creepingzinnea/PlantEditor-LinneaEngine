#include "pch.h"
#include "Leaf.h"
#include "GraphicsEngine.h"
#include "CommonUtilities/include/UtilityFunctions.hpp"
#include <Random.h>

Leaf::Leaf(GraphicsEngine* aGraphicsEngine, float aLeafSize)
{
	myMesh = std::make_shared<Mesh>();
	myMesh->Init(aGraphicsEngine, eShape::CrossedSquares, "Leaf/Leaf");
	myInitialScale = aLeafSize;
	myMesh->myTransform.Scale(aLeafSize * myScaleFactor);
}

Leaf::Leaf(const Leaf& other) 
{
	myMesh = std::make_shared<Mesh>(*other.myMesh);
}

void Leaf::Init(GraphicsEngine* aGraphicsEngine, float aLeafSize)
{
	myMesh->Init(aGraphicsEngine, eShape::CrossedSquares, myMaterial);
	myInitialScale = aLeafSize;
	myMesh->myTransform.Scale(aLeafSize * myScaleFactor);
}

void Leaf::SetMaterial(GraphicsEngine* aGraphicsEngine, std::string& aMaterialName)
{
	myMaterial = aMaterialName;
	myMesh->LoadMaterial(myMaterial, aGraphicsEngine);
}

void Leaf::UpdateLeaf(Transform& aTransform, float aLeafSize)
{
	myMesh->myTransform = aTransform;
	myInitialScale = aLeafSize;
	myMesh->myTransform.Scale(aLeafSize * myScaleFactor);
}

void Leaf::CalculateScaleFactor(float& aRandomFactor) 
{
	float randomIntensity = aRandomFactor * aRandomFactor; 

	float randomVariation = Random::GetRandomFloat(-randomIntensity, randomIntensity);
	float scaleFactor = myInitialScale * (1.0f + randomVariation);
	scaleFactor = CommonUtilities::Clamp(scaleFactor, myInitialScale * 0.01f, myInitialScale * 2.0f);

	myMesh->myTransform.Scale(scaleFactor);
	myScaleFactor = scaleFactor / myInitialScale;
}
