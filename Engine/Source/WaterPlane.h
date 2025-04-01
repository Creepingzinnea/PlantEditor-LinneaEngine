#pragma once
#include "Mesh.h"

class GraphicsEngine;

class WaterPlane : public Mesh
{
public:
	WaterPlane();
	void Init(GraphicsEngine* aGraphicsEngine);
	~WaterPlane() = default;

private:
};