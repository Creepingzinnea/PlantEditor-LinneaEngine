#include "pch.h"
#include "texture.h"
#include "TGPUtilities.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GraphicsEngine.h"
#include "DDSTextureLoader11.h"

#define USE_MIPMAPS

bool Texture::Initialize(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext, unsigned char* aRgbaPixels, int width, int height, bool useSRGB)
{
	//sets up the texture based on whether mipmaps are used
	// and creates a shader resource view for accessing the texture in shaders.

	ID3D11Texture2D* texture = nullptr;


	//TEXTUREDESC---------------------------------------------------------------------------------------------------
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	if (useSRGB)
	{
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}
	else
	{
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	desc.CPUAccessFlags = 0;

#ifdef USE_MIPMAPS
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MipLevels = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
#else
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
#endif
	//--------------------------------------------------------------------------------------------------------------

	//SUBRESOURCEDATA------------------------------------------------------------------------------------------------
#ifndef USE_MIPMAPS
	D3D11_SUBRESOURCE_DATA subresourceDesc = {};
	subresourceDesc.pSysMem = (void*)aRgbaPixels;
	subresourceDesc.SysMemPitch = width * 4;
	subresourceDesc.SysMemSlicePitch = width * height * 4;

	if (FAILED(aDevice->CreateTexture2D(&desc, &subresourceDesc, &texture)))
	{
		TGPUtilities::CustomdebugOutput("Texture.cpp, failed to create texture");
		return false;
	}
#else
	if (FAILED(aDevice->CreateTexture2D(&desc, nullptr, &texture)))
	{
		TGPUtilities::CustomdebugOutput("Texture.cpp, failed to create texture");
		return false;
	}
#endif
	// --------------------------------------------------------------------------------------------------------------

	//SHADERRESOURCEVIEW---------------------------------------------------------------------------------------------
	if (!aDevice || !texture) 
	{
		TGPUtilities::CustomdebugOutput("Texture.cpp, device or texture is null");
		return false;
	}
	HRESULT hr = aDevice->CreateShaderResourceView(texture, NULL, &myShaderResourceView);
	if (FAILED(hr) || !myShaderResourceView)
	{
		TGPUtilities::CustomdebugOutput("Texture.cpp, failed to create shader resource view");
		return false;
	}
	//--------------------------------------------------------------------------------------------------------------


#ifdef USE_MIPMAPS
	aDeviceContext->UpdateSubresource(texture, 0, nullptr, (void*)aRgbaPixels, width * 4, width * height * 4);
	aDeviceContext->GenerateMips(myShaderResourceView.Get());
#endif

	texture->Release();

	return true;
}

void Texture::Bind(ID3D11DeviceContext* aContext, int aSlot)
{
	aContext->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
}

bool Texture::LoadTexture(const char* filePath, GraphicsEngine* aGraphicsEngine, bool useSRGB)
{
	HRESULT result;

	// Load image with stb_image
	int width = 0, height = 0, channels = 0;
	unsigned char* img = stbi_load(filePath, &width, &height, &channels, 0);
	if (img == nullptr)
	{
		TGPUtilities::CustomdebugOutput("Error: Failed to load texture image");
		return false;
	}

	// Convert image data to RGBA if necessary
	std::vector<unsigned char> imageData;
	if (channels == 3)
	{
		imageData.resize(static_cast<size_t>(width) * height * 4);
		for (int i = 0; i < width * height; ++i)
		{
			imageData[4 * i + 0] = img[3 * i + 0];
			imageData[4 * i + 1] = img[3 * i + 1];
			imageData[4 * i + 2] = img[3 * i + 2];
			imageData[4 * i + 3] = 255;
		}
	}
	else if (channels == 4)
	{
		// Use the image data directly
		imageData.assign(img, img + (width * height * 4));
	}
	else
	{
		TGPUtilities::CustomdebugOutput("Error: Unsupported number of texture channels");
		stbi_image_free(img);
		return false;
	}
	stbi_image_free(img);

	// Initialize the texture
	if (!Initialize(aGraphicsEngine->GetDevice().Get(), aGraphicsEngine->GetDeviceContext().Get(), imageData.data(), width, height, useSRGB))
	{
		TGPUtilities::CustomdebugOutput("Failed to initialize texture");
		return false;
	}

	// Create sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = aGraphicsEngine->GetDevice().Get()->CreateSamplerState(&samplerDesc, aGraphicsEngine->GetSamplerState().GetAddressOf());
	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Error: Failed to create sampler state");
		return false;
	}

	return true;
}

bool Texture::LoadCubeMap(GraphicsEngine* aGraphicsEngine)
{
	HRESULT result = DirectX::CreateDDSTextureFromFile
	(
		aGraphicsEngine->GetDevice().Get(),
		L"../Assets/Textures/Default_cubemap.dds",
		nullptr,
		myShaderResourceView.GetAddressOf()
	);

	if (FAILED(result))
	{
		TGPUtilities::CustomdebugOutput("Failed to Load DDS Texture for terrain");
		return false;
	}

	return true;
}
