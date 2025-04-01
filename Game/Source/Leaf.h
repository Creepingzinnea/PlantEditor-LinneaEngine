#pragma once
#include "Mesh.h"

class GraphicsEngine;

class Leaf
{
public:
	Leaf(GraphicsEngine* aGraphicsEngine, float aLeafSize);
	Leaf(const Leaf& other);
	~Leaf() = default;

	void Init(GraphicsEngine* aGraphicsEngine, float aLeafSize);
	void SetMaterial(GraphicsEngine* aGraphicsEngine, std::string& aMaterialName);
	void UpdateLeaf(Transform& aTransform, float aLeafSize);
	inline Transform& AccessTransform() { return myMesh->myTransform; };
	inline void SetRandomRotation(const CommonUtilities::Vector3<float>& aRotation) { myRandomRotation = aRotation; };
	void CalculateScaleFactor(float& aLeafSize);
	inline const CommonUtilities::Vector3<float>& GetRandomRotation() { return myRandomRotation; };
	inline const float& GetRandomScale() { return myScaleFactor; };
	std::shared_ptr<Mesh> myMesh;
	bool isAtTipOfBranch = false;
	std::string myMaterial = "Leaf/Leaf";

private:
	CommonUtilities::Vector3<float> myRandomRotation = { 0,0,0 };
	float myScaleFactor = 1.0f;
	float myInitialScale = 1.0f;
};

