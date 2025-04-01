#pragma once
#include <wrl/client.h>
#include <Matrix4x4.hpp>
#include <vector>
#include <string>
#include <Timer.h>
#include "Mesh.h"
#include "Transform.h"

class Texture;
class GraphicsEngine;

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

using vector3f = CommonUtilities::Vector3<float>;
using vector2f = CommonUtilities::Vector2<float>;

class Terrain : public Mesh
{
public:
	//CONSTRUCTORS
	Terrain();
	~Terrain();

	//GENERAL FUNCTIONS
	bool GenerateTerrain(int initialResolution, int numOctaves,
		float initialAmplitudef, float terrainSize);
	bool LoadMaterial(GraphicsEngine* aGraphicsEngine);

private:
	//buffers
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	//vertex shader
	ComPtr<ID3D11VertexShader> myVertexShader;
	//pixel shader
	ComPtr<ID3D11PixelShader> myPixelShader;
	//input layout
	ComPtr<ID3D11InputLayout> myInputLayout;

	Texture* myGrassMaterial;
	Texture* myRockMaterial;
	Texture* mySandMaterial;

	Texture* myGrassColor;
	Texture* myRockColor;
	Texture* mySandColor;
			
	Texture* myGrassNormalMap;
	Texture* myRockNormalMap;
	Texture* mySandNormalMap;

	Texture* myCubeMap;
};