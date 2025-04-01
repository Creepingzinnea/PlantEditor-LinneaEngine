#include "pch.h"
#include "GraphicsEngine.h"
#include "Mesh.h"
#include "TGPUtilities.hpp"
#include <InputManager.h>
#include <math.h>
#include <fstream>
#include <thread>
#include <mutex>
#include <string>
#include <future>
#include <iostream>

std::mutex textureMutex;

void Mesh::Init(GraphicsEngine* aGraphicsEngine, eShape aShape, std::string aMaterialName)
{
	switch (aShape)
	{
	case eShape::Cube:
	{
		LoadCube();
		break;
	}
	case eShape::Tube:
	{
		LoadTube();
		break;
	}
	case eShape::SkyBox:
	{
		LoadSkyBox();
		break;
	}
	case eShape::Triangle:
	{
		LoadNgon(3);
		break;
	}
	case eShape::Square:
	{
		LoadNgon(4);
		break;
	}
	case eShape::CrossedSquares:
	{
		LoadCrossedSquares();
		break;
	}
	case eShape::Pentagon:
	{
		LoadNgon(5);
		break;
	}
	}

	LoadMaterial(aMaterialName, aGraphicsEngine);
	LoadCustomShaders(*aGraphicsEngine, "../Assets/Shaders/PBRShader_PS.cso", "../Assets/Shaders/MaterialShader_VS.cso");
}

void Mesh::InitAfterVertexCreation(GraphicsEngine* aGraphicsEngine, std::string aMaterialName)
{
	CalculateTBN();
	myIndexCount = static_cast<unsigned int>(myIndices.size());
	LoadMaterial(aMaterialName, aGraphicsEngine);
	LoadCustomShaders(*aGraphicsEngine, "../Assets/Shaders/PBRShader_PS.cso", "../Assets/Shaders/MaterialShader_VS.cso");
}


void Mesh::LoadSkyBox()
{
	myVertices.resize(24);

	// Front
	myVertices[0].pos = { 0.5f, -0.5f, 0.5f };    myVertices[0].uv = { 0.5f, 0.67f };
	myVertices[1].pos = { 0.5f, 0.5f, 0.5f };     myVertices[1].uv = { 0.5f, 0.33f };
	myVertices[2].pos = { -0.5f, 0.5f, 0.5f };    myVertices[2].uv = { 0.25f, 0.33f };
	myVertices[3].pos = { -0.5f, -0.5f, 0.5f };   myVertices[3].uv = { 0.25f, 0.67f };

	// Left
	myVertices[4].pos = { -0.5f, -0.5f, -0.5f };   myVertices[4].uv = { 0.00f, 0.67f };
	myVertices[5].pos = { -0.5f, 0.5f, -0.5f };    myVertices[5].uv = { 0.00f, 0.33f };
	myVertices[6].pos = { -0.5f, 0.5f, 0.5f };   myVertices[6].uv = { 0.25f, 0.33f };
	myVertices[7].pos = { -0.5f, -0.5f, 0.5f };  myVertices[7].uv = { 0.25f, 0.67f };

	// Back
	myVertices[8].pos = { -0.5f, -0.5f, -0.5f };  myVertices[8].uv = { 1.0f, 0.67f };
	myVertices[9].pos = { -0.5f, 0.5f, -0.5f };   myVertices[9].uv = { 1.0f, 0.33f };
	myVertices[10].pos = { 0.5f, 0.5f, -0.5f };   myVertices[10].uv = { 0.75f, 0.33f };
	myVertices[11].pos = { 0.5f, -0.5f, -0.5f };  myVertices[11].uv = { 0.75f, 0.67f };

	// Right
	myVertices[12].pos = { 0.5f, -0.5f, -0.5f };  myVertices[12].uv = { 0.75f, 0.67f };
	myVertices[13].pos = { 0.5f, 0.5f, -0.5f };   myVertices[13].uv = { 0.75f, 0.33f };
	myVertices[14].pos = { 0.5f, 0.5f, 0.5f };    myVertices[14].uv = { 0.50f, 0.33f };
	myVertices[15].pos = { 0.5f, -0.5f, 0.5f };   myVertices[15].uv = { 0.50f, 0.67f };

	// Top
	myVertices[16].pos = { 0.5f, 0.5f, 0.5f };    myVertices[16].uv = { 0.50f, 0.33f };
	myVertices[17].pos = { 0.5f, 0.5f, -0.5f };   myVertices[17].uv = { 0.50f, 0.00f };
	myVertices[18].pos = { -0.5f, 0.5f, -0.5f };  myVertices[18].uv = { 0.25f, 0.00f };
	myVertices[19].pos = { -0.5f, 0.5f, 0.5f };   myVertices[19].uv = { 0.25f, 0.33f };

	// Bottom
	myVertices[20].pos = { 0.5f, -0.5f, -0.5f };	myVertices[20].uv = { 0.50f, 1.00f };
	myVertices[21].pos = { 0.5f, -0.5f, 0.5f };		myVertices[21].uv = { 0.50f, 0.67f };
	myVertices[22].pos = { -0.5f, -0.5f, 0.5f };	myVertices[22].uv = { 0.25f, 0.67f };
	myVertices[23].pos = { -0.5f, -0.5f, -0.5f };   myVertices[23].uv = { 0.25f, 1.00f };

	CalculateTBN();

	myIndices =
	{
		// Front face (reversed)
		2, 1, 0,     0, 3, 2,
		// Left face (reversed)
		4, 5, 6,     6, 7, 4,
		// Back face (reversed)
		10, 9, 8,    8, 11, 10,
		// Right face (reversed)
		14, 13, 12,  12, 15, 14,
		// Top face (reversed)
		18, 17, 16,  16, 19, 18,
		// Bottom face (reversed)
		22, 21, 20,  20, 23, 22
	};
	myIndexCount = static_cast<unsigned int>(myIndices.size());


	myMin = { -0.5f, -0.5f , -0.5f };
	myMax = { 0.5f, 0.5f, 0.5f };
}

void Mesh::LoadCube()
{
	myVertices.resize(24);

	// Front
	myVertices[0].pos = { -0.5f, -0.5f, 0.5f };    myVertices[0].uv = { 0.0f, 1.0f };
	myVertices[1].pos = { -0.5f, 0.5f, 0.5f };     myVertices[1].uv = { 0.0f, 0.0f };
	myVertices[2].pos = { 0.5f, 0.5f, 0.5f };      myVertices[2].uv = { 1.0f, 0.0f };
	myVertices[3].pos = { 0.5f, -0.5f, 0.5f };     myVertices[3].uv = { 1.0f, 1.0f };

	// Left
	myVertices[4].pos = { -0.5f, -0.5f, -0.5f };   myVertices[4].uv = { 0.0f, 1.0f };
	myVertices[5].pos = { -0.5f, 0.5f, -0.5f };    myVertices[5].uv = { 0.0f, 0.0f };
	myVertices[6].pos = { -0.5f, 0.5f, 0.5f };     myVertices[6].uv = { 1.0f, 0.0f };
	myVertices[7].pos = { -0.5f, -0.5f, 0.5f };    myVertices[7].uv = { 1.0f, 1.0f };

	// Back
	myVertices[8].pos = { 0.5f, -0.5f, -0.5f };    myVertices[8].uv = { 0.0f, 1.0f };
	myVertices[9].pos = { 0.5f, 0.5f, -0.5f };     myVertices[9].uv = { 0.0f, 0.0f };
	myVertices[10].pos = { -0.5f, 0.5f, -0.5f };   myVertices[10].uv = { 1.0f, 0.0f };
	myVertices[11].pos = { -0.5f, -0.5f, -0.5f };  myVertices[11].uv = { 1.0f, 1.0f };

	// Right
	myVertices[12].pos = { 0.5f, -0.5f, 0.5f };    myVertices[12].uv = { 0.0f, 1.0f };
	myVertices[13].pos = { 0.5f, 0.5f, 0.5f };     myVertices[13].uv = { 0.0f, 0.0f };
	myVertices[14].pos = { 0.5f, 0.5f, -0.5f };    myVertices[14].uv = { 1.0f, 0.0f };
	myVertices[15].pos = { 0.5f, -0.5f, -0.5f };   myVertices[15].uv = { 1.0f, 1.0f };

	// Top
	myVertices[16].pos = { -0.5f, 0.5f, 0.5f };    myVertices[16].uv = { 0.0f, 1.0f };
	myVertices[17].pos = { -0.5f, 0.5f, -0.5f };   myVertices[17].uv = { 0.0f, 0.0f };
	myVertices[18].pos = { 0.5f, 0.5f, -0.5f };    myVertices[18].uv = { 1.0f, 0.0f };
	myVertices[19].pos = { 0.5f, 0.5f, 0.5f };     myVertices[19].uv = { 1.0f, 1.0f };

	// Bottom
	myVertices[20].pos = { -0.5f, -0.5f, -0.5f };  myVertices[20].uv = { 0.0f, 1.0f };
	myVertices[21].pos = { -0.5f, -0.5f, 0.5f };   myVertices[21].uv = { 0.0f, 0.0f };
	myVertices[22].pos = { 0.5f, -0.5f, 0.5f };    myVertices[22].uv = { 1.0f, 0.0f };
	myVertices[23].pos = { 0.5f, -0.5f, -0.5f };   myVertices[23].uv = { 1.0f, 1.0f };


	myIndices =
	{
		0, 2, 1,  0, 3, 2,
		// Left face
		4, 6, 5,  4, 7, 6,
		// Back face
		8, 10, 9, 8, 11, 10,
		// Right face
		12,14,13, 12,15,14,
		// Top face
		16,18,17, 16,19,18,
		// Bottom face
		20,22,21, 20,23,22
	};

	CalculateTBN(true);
	myIndexCount = static_cast<unsigned int>(myIndices.size());

	myMin = { -0.5f, -0.5f , -0.5f };
	myMax = { 0.5f, 0.5f, 0.5f };
}

void Mesh::LoadNgon(unsigned int aN)
{
	if (aN < 3) { aN = 3; }

	float radius = 1.0f;  // Unit circle
	float pi = 3.14159265359f;

	if (myVertices.size() > 0)
	{
		myVertices.clear();
	}
	if (myVertices.size() > 0)
	{
		myIndices.clear();
	}

	// Center vertex
	vector3f centerPos = { 0.0f, 0.0f, 0.0f };
	vector3f centerNormal = { 0.0f, 1.0f, 0.0f };
	vector2f centerUV = { 0.5f, 0.5f };

	myVertices.push_back({ centerPos, centerNormal, centerUV, {}, {} });

	// Generate outer vertices
	for (unsigned int i = 0; i < aN; ++i)
	{
		float angle = (2.0f * pi * i) / aN;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		vector3f pos = { x, 0.0f, z };
		vector3f normal = centerNormal;

		// UV mapping (projected onto [0,1] space)
		float u = (x + 1.0f) * 0.5f;
		float v = (z + 1.0f) * 0.5f;

		myVertices.push_back({ pos, normal, { u, v }, {}, {} });
	}

	for (unsigned int i = 1; i <= aN; ++i)
	{
		myIndices.push_back(0);                 // Center vertex
		myIndices.push_back((i % aN) + 1);      // Next outer vertex (wrap around)
		myIndices.push_back(i);                 // Current outer vertex
	}

	CalculateTBN(true);
	myIndexCount = static_cast<unsigned int>(myIndices.size());

	myMin = { -1.0f, 0.0f, -1.0f };
	myMax = { 1.0f, 0.0f, 1.0f };
}

void Mesh::LoadCrossedSquares()
{
	// Clear existing data
	myVertices.clear();
	myIndices.clear();

	// Define square size
	float size = 0.5f;

	// Create first square (in XY plane, extending in +Y direction)
	vector3f v1 = { size, 0.0f, 0.0f };      // Bottom-right
	vector3f v2 = { -size, 0.0f, 0.0f };     // Bottom-left
	vector3f v3 = { -size, 2 * size, 0.0f }; // Top-left
	vector3f v4 = { size, 2 * size, 0.0f };  // Top-right

	// Add vertices for first square (front-facing)
	unsigned int vBaseIndex = static_cast<unsigned int>(myVertices.size());
	vector3f vNormal = { 0.0f, 0.0f, 1.0f }; // Normal points forward (Z+)

	myVertices.push_back({ v1, vNormal, { 1.0f, 1.0f }, {}, {} });
	myVertices.push_back({ v2, vNormal, { 0.0f, 1.0f }, {}, {} });
	myVertices.push_back({ v3, vNormal, { 0.0f, 0.0f }, {}, {} });
	myVertices.push_back({ v4, vNormal, { 1.0f, 0.0f }, {}, {} });

	// Add vertices for back side of first square
	vector3f vBackNormal = { 0.0f, 0.0f, -1.0f }; // Normal points backward (Z-)

	myVertices.push_back({ v1, vBackNormal, { 0.0f, 1.0f }, {}, {} }); // Flipped U
	myVertices.push_back({ v2, vBackNormal, { 1.0f, 1.0f }, {}, {} }); // Flipped U
	myVertices.push_back({ v3, vBackNormal, { 1.0f, 0.0f }, {}, {} }); // Flipped U
	myVertices.push_back({ v4, vBackNormal, { 0.0f, 0.0f }, {}, {} }); // Flipped U

	// Add indices for first square (front and back)
	// Front face
	myIndices.push_back(vBaseIndex);
	myIndices.push_back(vBaseIndex + 1);
	myIndices.push_back(vBaseIndex + 2);

	myIndices.push_back(vBaseIndex);
	myIndices.push_back(vBaseIndex + 2);
	myIndices.push_back(vBaseIndex + 3);

	// Back face
	myIndices.push_back(vBaseIndex + 4);
	myIndices.push_back(vBaseIndex + 6);
	myIndices.push_back(vBaseIndex + 5);

	myIndices.push_back(vBaseIndex + 4);
	myIndices.push_back(vBaseIndex + 7);
	myIndices.push_back(vBaseIndex + 6);

	// Create second square (in ZY plane)
	vector3f h1 = { 0.0f, 0.0f, size };      // Bottom-right
	vector3f h2 = { 0.0f, 0.0f, -size };     // Bottom-left
	vector3f h3 = { 0.0f, 2 * size, -size }; // Top-left
	vector3f h4 = { 0.0f, 2 * size, size };  // Top-right

	// Add vertices for second square
	unsigned int hBaseIndex = static_cast<unsigned int>(myVertices.size());
	vector3f hNormal = { 1.0f, 0.0f, 0.0f }; // Normal points right (X+)

	myVertices.push_back({ h1, hNormal, { 1.0f, 1.0f }, {}, {} });
	myVertices.push_back({ h2, hNormal, { 0.0f, 1.0f }, {}, {} });
	myVertices.push_back({ h3, hNormal, { 0.0f, 0.0f }, {}, {} });
	myVertices.push_back({ h4, hNormal, { 1.0f, 0.0f }, {}, {} });

	// Add vertices for left side of second square
	vector3f hLeftNormal = { -1.0f, 0.0f, 0.0f }; // Normal points left (X-)

	myVertices.push_back({ h1, hLeftNormal, { 0.0f, 1.0f }, {}, {} }); // Flipped U
	myVertices.push_back({ h2, hLeftNormal, { 1.0f, 1.0f }, {}, {} }); // Flipped U
	myVertices.push_back({ h3, hLeftNormal, { 1.0f, 0.0f }, {}, {} }); // Flipped U
	myVertices.push_back({ h4, hLeftNormal, { 0.0f, 0.0f }, {}, {} }); // Flipped U

	// Add indices for second square (right and left faces)
	// Right face
	myIndices.push_back(hBaseIndex);
	myIndices.push_back(hBaseIndex + 1);
	myIndices.push_back(hBaseIndex + 2);

	myIndices.push_back(hBaseIndex);
	myIndices.push_back(hBaseIndex + 2);
	myIndices.push_back(hBaseIndex + 3);

	// Left face
	myIndices.push_back(hBaseIndex + 4);
	myIndices.push_back(hBaseIndex + 6);
	myIndices.push_back(hBaseIndex + 5);

	myIndices.push_back(hBaseIndex + 4);
	myIndices.push_back(hBaseIndex + 7);
	myIndices.push_back(hBaseIndex + 6);

	// Finalize
	CalculateTBN(true);
	myIndexCount = static_cast<unsigned int>(myIndices.size());

	// Update bounding box
	myMin = { -size, 0.0f, -size };
	myMax = { size, 2 * size, size };
}

void Mesh::LoadPlane(float aWidth, float aHeight)
{
	float halfWidth = aWidth / 2.0f;
	float halfHeight = aHeight / 2.0f;

	vector3f normal = { 0.0f, 0.0f, 1.0f }; // Plane facing the positive Z direction
	vector3f tangent = { 1.0f, 0.0f, 0.0f }; // Tangent along the X-axis
	vector3f bitangent = { 0.0f, 1.0f, 0.0f }; // Bitangent along the Y-axis

	myVertices =
	{
		//  Position                            Normal          UV      Tangent        Bitangent
		{ { halfWidth,  halfHeight, 0.0f },   normal, { 1, 0 }, tangent, bitangent }, // Top-right
		{ { halfWidth, -halfHeight, 0.0f },   normal, { 1, 1 }, tangent, bitangent }, // Bottom-right
		{ {-halfWidth, -halfHeight, 0.0f },   normal, { 0, 1 }, tangent, bitangent }, // Bottom-left
		{ {-halfWidth,  halfHeight, 0.0f },   normal, { 0, 0 }, tangent, bitangent }  // Top-left
	};

	myIndices =
	{
		0, 1, 2, // First triangle (top-right, bottom-right, bottom-left)
		0, 2, 3  // Second triangle (top-right, bottom-left, top-left)
	};

	myIndexCount = static_cast<unsigned int>(myIndices.size());

	myMin = { -halfWidth, 0, -halfHeight };
	myMax = { halfWidth, 0,  halfHeight };
}

void Mesh::LoadTube()
{
	float radius = 0.33f;
	float height = 1.0f;
	int segments = 8;
	float pi = 3.14159f;

	myMax = { radius, height, radius };
	myMin = { -radius , -height, -radius };

	myVertices.clear();
	myIndices.clear();

	float halfHeight = height / 2.0f;
	float angleStep = 2.0f * pi / segments;

	float uTiling = 1.0f;
	float vTiling = 1.0f;

	// Generate side vertices
	for (int i = 0; i <= segments; ++i)
	{
		float angle = i * angleStep;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		vector3f normal = { cos(angle), 0.0f, sin(angle) };

		// Calculate U coordinate based on angle around the tube
		float u = (static_cast<float>(i) / segments) * uTiling;

		// V coordinates span from 0 (top) to vTiling (bottom)
		float vTop = 0.0f;
		float vBottom = vTiling;

		// Bottom vertex
		myVertices.push_back({ { x, -halfHeight, z }, normal, { u, vBottom }, {}, {} });

		// Top vertex
		myVertices.push_back({ { x, halfHeight, z }, normal, { u, vTop }, {}, {} });
	}

	// Generate side indices (corrected winding order)
	for (int i = 0; i < segments; ++i)
	{
		int bottom0 = i * 2;
		int top0 = bottom0 + 1;
		int bottom1 = (i + 1) * 2;  // No modulo
		int top1 = bottom1 + 1;

		// First triangle
		myIndices.push_back(bottom0);
		myIndices.push_back(top0);
		myIndices.push_back(top1);

		// Second triangle
		myIndices.push_back(bottom0);
		myIndices.push_back(top1);
		myIndices.push_back(bottom1);
	}

	CalculateTBN(true);
	myIndexCount = static_cast<unsigned int>(myIndices.size());
}

bool Mesh::LoadCustomShaders(GraphicsEngine& aGraphicsEngine, const std::string& aPixelShaderFile, const std::string& aVertexShaderFile)
{
	HRESULT result;

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(sVertex) * myVertices.size());
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = myVertices.data();

	result = aGraphicsEngine.GetDevice().Get()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Failed to Create Vertex Buffer");
		return false;
	}

	// Create index buffer
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * myIndexCount); // Cast to UINT
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = myIndices.data();

	result = aGraphicsEngine.GetDevice().Get()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Failed to Create Index Buffer");
		return false;
	}

	std::string vsData;

	// Load shaders
	if (!LoadVertexShader(aGraphicsEngine.GetDevice().Get(), aVertexShaderFile, vsData) ||
		!LoadPixelShader(aGraphicsEngine.GetDevice().Get(), aPixelShaderFile))
	{
		return false;
	}

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	result = aGraphicsEngine.GetDevice().Get()->CreateInputLayout(layout, _countof(layout), vsData.data(), vsData.size(),
		&myInputLayout);
	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Failed to Create Input Layout");
		return false;
	}

	return true;
}

void Mesh::Render(GraphicsEngine* aGraphicsEngine)
{
	myTexture->Bind(aGraphicsEngine->GetDeviceContext().Get(), 11);
	myColorTexture->Bind(aGraphicsEngine->GetDeviceContext().Get(), 12);
	myMaterialTexture->Bind(aGraphicsEngine->GetDeviceContext().Get(), 13);
	myNormalTexture->Bind(aGraphicsEngine->GetDeviceContext().Get(), 14);


	ID3D11DeviceContext* deviceContext = aGraphicsEngine->GetDeviceContext().Get();
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(myInputLayout.Get());

	//object buffer
	aGraphicsEngine->UpdateObjectBuffer(myTransform.myMatrix);

	unsigned int stride = sizeof(sVertex);
	unsigned int offset = 0;

	//Vertex and index buffer
	deviceContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//Set vertex shader
	deviceContext->VSSetShader(myVertexShader.Get(), nullptr, 0);

	//Set shader
	deviceContext->PSSetShader(myPixelShader.Get(), nullptr, 0);

	//Render vertices
	deviceContext->DrawIndexed(myIndexCount, 0, 0);
}

Texture* Mesh::GetTexture()
{
	return myTexture.get();
}

float Mesh::GetWidth()
{
	float width = std::abs(myMax.x - myMin.x);
	if (myTransform.GetScale().x != 0.0f)
	{
		width *= myTransform.GetScale().x;
	}
	return width;
}

float Mesh::GetHeight()
{
	float height = std::abs(myMax.y - myMin.y);
	if (myTransform.GetScale().y != 0.0f)
	{
		height *= myTransform.GetScale().y;
	}
	return height;
}

float Mesh::GetDepth()
{
	float depth = std::abs(myMax.z - myMin.z);
	if (myTransform.GetScale().z != 0.0f)
	{
		depth *= myTransform.GetScale().z;
	}
	return depth;
}

bool Mesh::LoadPixelShader(ID3D11Device* aDevice, std::string aFile)
{
	// Open the compiled pixel shader file
	std::ifstream PSFile(aFile, std::ios::binary);
	if (!PSFile.is_open())
	{
		TGPUtilities::CustomdebugOutput("Failed to open file");
		return false;
	}

	// Read the file content into a vector
	std::vector<char> PSData((std::istreambuf_iterator<char>(PSFile)), std::istreambuf_iterator<char>());
	PSFile.close();

	// Check if the file is empty
	if (PSData.empty())
	{
		TGPUtilities::CustomdebugOutput("File is empty");
		return false;
	}

	// Create the pixel shader
	ID3D11PixelShader* shader = nullptr;

	HRESULT result = aDevice->CreatePixelShader(PSData.data(), PSData.size(), nullptr, &shader);

	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Failed to create pixel shader");
		return false;
	}

	myPixelShader = shader;
	return true;
}

bool Mesh::LoadVertexShader(ID3D11Device* aDevice, std::string aFile, std::string& aVSData)
{
	std::ifstream vsFile;
	vsFile.open(aFile, std::ios::binary);
	aVSData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	HRESULT result = aDevice->CreateVertexShader(aVSData.data(), aVSData.size(), nullptr, &myVertexShader);
	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Failed to load vertex shader");
		return false;
	}
	vsFile.close();
	return true;
}

bool Mesh::LoadSingleTexture(const char* filePath, GraphicsEngine* aGraphicsEngine)
{
	if (!myTexture->LoadTexture(filePath, aGraphicsEngine, true))
	{
		TGPUtilities::CustomdebugOutput("Failed to load texture");
		return false;
	}

	return true;
}

bool Mesh::LoadMaterial(std::string aMaterialName, GraphicsEngine* aGraphicsEngine)
{
#ifdef _DEBUG
	std::string relativePath = "../Assets/Textures/";

	//color
	std::string colorTexture = relativePath + aMaterialName + "_c.png";
	if (!myColorTexture->LoadTexture(colorTexture.c_str(), aGraphicsEngine, false))
	{
		TGPUtilities::CustomdebugOutput("Failed to load texture");
		return false;
	}

	//material
	std::string materialTexture = relativePath + aMaterialName + "_m.png";
	if (!myMaterialTexture->LoadTexture(materialTexture.c_str(), aGraphicsEngine, true))
	{
		TGPUtilities::CustomdebugOutput("Failed to load texture");
		return false;
	}

	//normal
	std::string normaltexture = relativePath + aMaterialName + "_n.png";
	if (!myNormalTexture->LoadTexture(normaltexture.c_str(), aGraphicsEngine, true))
	{
		TGPUtilities::CustomdebugOutput("Failed to load texture");
		return false;
	}

#else 
	std::string relativePath = "../Assets/Textures/";

	//color
	std::string colorTexture = relativePath + aMaterialName + "_c.png";
	myColorTexture->LoadTexture(colorTexture.c_str(), aGraphicsEngine, false);

	//material
	std::string materialTexture = relativePath + aMaterialName + "_m.png";
	myMaterialTexture->LoadTexture(materialTexture.c_str(), aGraphicsEngine, true);

	//normal
	std::string normaltexture = relativePath + aMaterialName + "_n.png";
	myNormalTexture->LoadTexture(normaltexture.c_str(), aGraphicsEngine, true);

#endif

	return true;
}

bool Mesh::CalculateTBN(bool softEdges)
{
	// Ensure correct number of indices
	if (myIndices.size() % 3 != 0)
	{
		TGPUtilities::CustomdebugOutput("Generate TBN, Invalid number of indices");
		return false;
	}

	// Initialize tangents, bitangents, and normals to zero
	for (auto& vertex : myVertices)
	{
		vertex.tangent = vector3f(0.0f, 0.0f, 0.0f);
		vertex.bitangent = vector3f(0.0f, 0.0f, 0.0f);
		vertex.normal = vector3f(0.0f, 0.0f, 0.0f);
	}

	// Process each triangle
	for (size_t i = 0; i < myIndices.size(); i += 3)
	{
		unsigned int i0 = myIndices[i];
		unsigned int i1 = myIndices[i + 1];
		unsigned int i2 = myIndices[i + 2];

		// Get vertex positions and UVs
		const vector3f& p0 = myVertices[i0].pos;
		const vector3f& p1 = myVertices[i1].pos;
		const vector3f& p2 = myVertices[i2].pos;

		const vector2f& uv0 = myVertices[i0].uv;
		const vector2f& uv1 = myVertices[i1].uv;
		const vector2f& uv2 = myVertices[i2].uv;

		// Compute Tangent and Bitangent
		vector3f tangent, bitangent;
		TBNHelper(p0, p1, p2, uv0, uv1, uv2, tangent, bitangent);

		// Compute face normal
		vector3f edge1 = p1 - p0;
		vector3f edge2 = p2 - p0;
		vector3f faceNormal = edge1.Cross(edge2);
		faceNormal.Normalize();

		if (softEdges)
		{
			// Accumulate Tangent, Bitangent, and Normal for each vertex
			myVertices[i0].tangent += tangent;
			myVertices[i1].tangent += tangent;
			myVertices[i2].tangent += tangent;

			myVertices[i0].bitangent += bitangent;
			myVertices[i1].bitangent += bitangent;
			myVertices[i2].bitangent += bitangent;

			myVertices[i0].normal += faceNormal;
			myVertices[i1].normal += faceNormal;
			myVertices[i2].normal += faceNormal;
		}
		else
		{
			// Explicit per-face normals and TBN for hard edges
			myVertices[i0].tangent = tangent;
			myVertices[i1].tangent = tangent;
			myVertices[i2].tangent = tangent;

			myVertices[i0].bitangent = bitangent;
			myVertices[i1].bitangent = bitangent;
			myVertices[i2].bitangent = bitangent;

			myVertices[i0].normal = faceNormal;
			myVertices[i1].normal = faceNormal;
			myVertices[i2].normal = faceNormal;
		}
	}

	// Post-process only for soft edges
	if (softEdges)
	{
		for (auto& vertex : myVertices)
		{
			// Normalize normal
			vertex.normal.Normalize();

			// Gram-Schmidt orthogonalization for tangent
			vertex.tangent = (vertex.tangent - vertex.normal * vertex.normal.Dot(vertex.tangent));
			vertex.tangent.Normalize();

			// Calculate handedness for bitangent
			float handedness = (vertex.normal.Cross(vertex.tangent).Dot(vertex.bitangent) < 0.0f) ? -1.0f : 1.0f;
			vertex.bitangent = vertex.normal.Cross(vertex.tangent) * handedness;

			// Normalize bitangent
			vertex.bitangent.Normalize();
		}
	}
	else
	{
		// In hard edge mode, simply normalize without orthogonalization
		for (auto& vertex : myVertices)
		{
			vertex.normal.Normalize();
			vertex.tangent.Normalize();
			vertex.bitangent.Normalize();
		}
	}

	return true;
}


void Mesh::TBNHelper(const vector3f& p0, const vector3f& p1, const vector3f& p2,
	const vector2f& uv0, const vector2f& uv1, const vector2f& uv2,
	vector3f& tangent, vector3f& bitangent)
{
	vector3f deltaPos1 = p1 - p0;
	vector3f deltaPos2 = p2 - p0;

	vector2f deltaUV1 = uv1 - uv0;
	vector2f deltaUV2 = uv2 - uv0;

	float r = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	if (fabs(r) < 1e-6f) // Avoid division by zero
		r = 1.0f;
	else
		r = 1.0f / r;

	tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

	tangent.Normalize();
	bitangent.Normalize();
}

bool Mesh::UpdateVertexData(GraphicsEngine* aGraphicsEngine)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = aGraphicsEngine->GetDeviceContext()->Map(myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Failed to map vertex buffer for updating vertices");
		return false;
	}
	memcpy(mappedResource.pData, myVertices.data(), sizeof(sVertex) * myVertices.size());
	aGraphicsEngine->GetDeviceContext()->Unmap(myVertexBuffer.Get(), 0);

	CalculateTBN(false);

	return true;
}

void Mesh::ClearVertexData()
{
	myVertices.clear();
	myIndices.clear();
	myVertexCount = 0;
	myIndexCount = 0;
}


