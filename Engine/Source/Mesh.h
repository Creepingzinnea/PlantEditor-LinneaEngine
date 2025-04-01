#pragma once
#include <wrl/client.h>
#include <Matrix4x4.hpp>
#include <string>
#include <vector>
#include "Transform.h"
#include "Texture.h"

class Texture;
class GraphicsEngine;

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

using vector4f = CommonUtilities::Vector4<float>;
using vector3f = CommonUtilities::Vector3<float>;
using vector2f = CommonUtilities::Vector2<float>;

enum class eShape
{
	Tube = 0,
	Cube,
	SkyBox,

	Triangle,
	Square,
	CrossedSquares,
	Pentagon

	//TODO: shapes to do later
	//Pyramid
	//PentagonalPyramid
};

struct sVertex
{
	vector3f pos = { 0,0,0 };
	vector3f normal ={ 0,0,0 };
	vector2f uv = {0,0};
	vector3f tangent = { 0,0,0 };
	vector3f bitangent = { 0,0,0 };
};

class Mesh
{
public:
	//CONSTRUCTORS
	Mesh() = default;
	~Mesh() = default;
	void Init(GraphicsEngine* aGraphicsEngine, eShape aShape, std::string aMaterialName = "Default");
	bool LoadCustomShaders(GraphicsEngine& aGraphicsEngine,
			const std::string& aPixelShaderFile,
			const std::string& aVertexShaderFile
	);
	void InitAfterVertexCreation(GraphicsEngine* aGraphicsEngine, std::string aMaterialName);

	//GENERAL FUNCTIONS
	void Render(GraphicsEngine* aGraphicsEngine);
	void ClearVertexData();
	bool CalculateTBN(bool softEdges = false);
	bool LoadSingleTexture(const char* filePath, GraphicsEngine* aGraphicsEngine);
	bool LoadMaterial(std::string, GraphicsEngine* aGraphicsEngine);
	bool LoadPixelShader(ID3D11Device* aDevice, std::string aFile);
	bool LoadVertexShader(ID3D11Device* aDevice, std::string aFile,
		std::string& aVSData);
	bool UpdateVertexData(GraphicsEngine* aGraphicsEngine);

	//GETTERS
	sVertex* GetVertex(const unsigned int aIndex) { return &myVertices[aIndex]; };
	Texture* GetTexture();
	float GetWidth();
	float GetHeight();
	float GetDepth();

	//PUBLIC VIRIABLES
	Transform myTransform;

	//Setters
	inline void AddVertex(sVertex aVertex) { myVertices.push_back(aVertex); }
	inline void RemoveVertex(unsigned int aIndex) { myVertices.erase(myVertices.begin() + aIndex); }
	inline void AddIndex(unsigned int aIndex) { myIndices.push_back(aIndex); }
	inline void RemoveIndex(unsigned int aIndex) { myIndices.erase(myIndices.begin() + aIndex); }

protected:
	//vertex storage variables
	int myIndexCount = 0;
	int myVertexCount = 0;
	std::vector<sVertex> myVertices;
	std::vector<unsigned int> myIndices;

	void LoadSkyBox();
	void LoadCube();
	void LoadNgon(unsigned int aN);
	void LoadCrossedSquares();
	void LoadTube();
	void LoadPlane(float aWidth, float aHeight);

	void TBNHelper(const vector3f& p0, const vector3f& p1, const vector3f& p2, const vector2f& uv0, const vector2f& uv1, const vector2f& uv2, vector3f& tangent, vector3f& bitangent);

	//buffers
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	//vertex shader
	ComPtr<ID3D11VertexShader> myVertexShader;
	//pixel shader
	ComPtr<ID3D11PixelShader> myPixelShader;
	//input layout
	ComPtr<ID3D11InputLayout> myInputLayout;

	std::shared_ptr<Texture> myTexture = std::make_shared<Texture>();
	std::shared_ptr<Texture> myColorTexture = std::make_shared<Texture>();
	std::shared_ptr<Texture> myNormalTexture = std::make_shared<Texture>();
	std::shared_ptr<Texture> myMaterialTexture = std::make_shared<Texture>();

	vector3f myMin = { 0,0,0 };
	vector3f myMax = { 0,0,0 };
};
