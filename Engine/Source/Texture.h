#pragma once
#include <wrl/client.h>

class GraphicsEngine;

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;


class Texture
{
public:
	Texture() = default;
	~Texture() = default;

	bool Initialize(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext, unsigned char* aRgbaPixels, int width, int height, bool useSRGB);
	void Bind(ID3D11DeviceContext* aContext, int aSlot);

	bool LoadTexture(const char* filePath, GraphicsEngine* aGraphicsEngine, bool useSRGB);
	bool LoadCubeMap(GraphicsEngine* aGraphicsEngine);

private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};
