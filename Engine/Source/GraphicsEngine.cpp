#include "pch.h"
#include "GraphicsEngine.h"
#include "TGPUtilities.hpp"
#include "FileLogger.h"
#include "Camera.h"
#include "Terrain.h"
#include "Timer.h"
#include "WaterPlane.h"
#include "BufferData.h"
#include "WindowHandler.h"
#include <InputManager.h>

bool GraphicsEngine::Initialize(CommonUtilities::Timer* aTimerPtr)
{
	if (!InitializeSwapChain(*WindowHandler::GetInstance().GetWindowHandle()))
	{
		FileLogger::Get().LogError("InitializeSwapChain failed.");
		return false;
	}

	if (!CreateBackBuffer())
	{
		FileLogger::Get().LogError("CreateBackBuffer failed.");
		return false;
	}

	if (!CreateConstantBuffers())
	{
		FileLogger::Get().LogError("CreateConstantBuffers failed.");
		return false;
	}

	if (!CreateSamplerState())
	{
		FileLogger::Get().LogError("CreateSamplerState failed.");
		return false;
	}


	if (!CreateDepthBuffer())
	{
		FileLogger::Get().LogError("CreateDepthBuffer failed.");
		return false;
	}

	if (!CreateRasterizerStates())
	{
		FileLogger::Get().LogError("CreateRasterizerStates failed.");
		return false;
	}

	if (!CreateBlendStates())
	{
		FileLogger::Get().LogError("CreateBlendStates failed.");
		return false;
	}

	SetRenderTargetsAndViewport();
	CreateRenderTarget(&myIntermediateRenderTarget);
	CreateRenderTarget(&myMainRenderTarget);

	myTimerPtr = aTimerPtr;

	CommonUtilities::Vector3<float> dirColor(255.0f, 223.0f, 145.0f);
	CommonUtilities::Vector3<float> normDirColor = TGPUtilities::NormaliseRgb(dirColor);
	myDirectionalLight = { {normDirColor.x, normDirColor.y, normDirColor.z, 1.04f}, {0.7f,0.0f,-1.0f} };
	CommonUtilities::Vector3<float> ambColor(198.0f, 119.0f, 255.0f);
	CommonUtilities::Vector3<float> normAmbColor = TGPUtilities::NormaliseRgb(ambColor);
	myAmbientLight = { {normAmbColor.x ,normAmbColor.y, normAmbColor.z,  3.247f} };

	return true;
}

void GraphicsEngine::UpdateObjectBuffer(const CommonUtilities::Matrix4x4<float> aMatrix)
{
	ObjectBufferData objectBufferData = {};
	objectBufferData.modelToWorldMatrix = aMatrix;

	D3D11_MAPPED_SUBRESOURCE mappedObjectBuffer = {};
	myContext->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectBuffer);
	memcpy(mappedObjectBuffer.pData, &objectBufferData, sizeof(ObjectBufferData));
	myContext->Unmap(myObjectBuffer.Get(), 0);
	myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
}

bool GraphicsEngine::PreFrame()
{
	// Check if the window has been resized
	if (!WindowHandler::GetInstance().GetWindowResized())
	{
		return true;
	}

	// Release dependent resources first (order matters here)
	myIntermediateRenderTarget.renderTargetView.Reset();
	myIntermediateRenderTarget.shaderResourceView.Reset();
	myMainRenderTarget.renderTargetView.Reset();
	myMainRenderTarget.shaderResourceView.Reset();
	myBackBuffer.Reset();
	myDepthBuffer.Reset();

	// Resize the swap chain buffers to match the new window size
	HRESULT result = mySwapChain->ResizeBuffers(
		1,
		0, 0,  // Use current window size (0,0 means use the current client area)
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		0
	);

	if (FAILED(result))
	{
		FileLogger::Get().LogError("ReziseBuffers failed.", result);
		FileLogger::Get().Flush();
		return false;
	}

	// Recreate the back buffer
	if (!CreateBackBuffer())
	{
		FileLogger::Get().LogError("CreateBackBuffer failed", result);
		FileLogger::Get().Flush();
		return false;
	}

	// Recreate depth buffer with new size
	if (!CreateDepthBuffer())
	{
		FileLogger::Get().LogError("CreateDepthBuffer failed", result);
		FileLogger::Get().Flush();
		return false;
	}

	// Set the new viewport and render targets
	SetRenderTargetsAndViewport();

	// Recreate render targets with the new size
	if (!CreateRenderTarget(&myIntermediateRenderTarget) ||
		!CreateRenderTarget(&myMainRenderTarget))
	{
		FileLogger::Get().LogError("CreateRenderTarget failed", result);
		FileLogger::Get().Flush();
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateBlendStates()
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT result = myDevice->CreateBlendState(&blendDesc, myAlphaBlendState.GetAddressOf());
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create alpha blend state", result);
		FileLogger::Get().Flush();
		return false;
	}
	
	D3D11_BLEND_DESC opaqueBlendDesc = {};
	opaqueBlendDesc.AlphaToCoverageEnable = FALSE;
	opaqueBlendDesc.IndependentBlendEnable = FALSE;
	opaqueBlendDesc.RenderTarget[0].BlendEnable = FALSE; 
	opaqueBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	opaqueBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	opaqueBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	opaqueBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	opaqueBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	opaqueBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	opaqueBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = myDevice->CreateBlendState(&opaqueBlendDesc, myOpaqueBlendState.GetAddressOf());
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create opaque blend state", result);
		FileLogger::Get().Flush();
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateRenderTarget(RenderTarget* aRenderTarget)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = myBackBufferDesc.Width;
	desc.Height = myBackBufferDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	ID3D11Texture2D* texture;


	result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
	if (!SUCCEEDED(result))
	{
		FileLogger::Get().LogError("Failed to create render target" , result);
		FileLogger::Get().Flush();
		return false;
	}

	result = myDevice->CreateShaderResourceView
	(
		texture,
		nullptr,
		&aRenderTarget->shaderResourceView
	);


	if (!SUCCEEDED(result))
	{
		FileLogger::Get().LogError("Failed to create shader resource view for render target", result);
		FileLogger::Get().Flush();
		return false;
	}

	result = myDevice->CreateRenderTargetView
	(
		texture,
		nullptr,
		&aRenderTarget->renderTargetView
	);

	if (!SUCCEEDED(result))
	{
		FileLogger::Get().LogError("Failed to create render target view for render target", result);
		FileLogger::Get().Flush();
		return false;
	}

	texture->Release();

	return true;
}

void GraphicsEngine::RenderWithReflection(const std::vector<std::shared_ptr<Mesh>>& aMeshVector, WaterPlane* aWaterPlane, const std::vector<std::shared_ptr<Mesh>>* aTransparentObjects)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Step 1: Render reflection texture to a separate render target--------------------------------------------

	// Unbind the shader resource view to avoid conflicts
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	myContext->PSSetShaderResources(10, 1, nullSRV);

	// Set render target to the reflection texture

	myContext->OMSetBlendState(myOpaqueBlendState.Get(), blendFactor, 0xffffffff);
	myContext->OMSetRenderTargets(1, myIntermediateRenderTarget.renderTargetView.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myIntermediateRenderTarget.renderTargetView.Get(), blendFactor);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	myContext->RSSetState(myFrontFaceCullingRasterizerState.Get());
	myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());

	// Prepare frame buffer data for reflection
	{
		FrameBufferData frameBufferData = {};
		CommonUtilities::Matrix4x4<float> reflectionMatrix;
		// Set scaling factors for reflection
		reflectionMatrix(2, 2) = -1.0f; // Invert Y-axis for reflection
		reflectionMatrix(4, 2) = 2.0f * aWaterPlane->myTransform.GetTranslation().y; // Adjust based on water height
		frameBufferData.worldToClipMatrix = reflectionMatrix * myCamera->GetWorldToClipMatrix();
		frameBufferData.directionalLightColorAndIntensity = myDirectionalLight.colorAndIntensity;
		frameBufferData.directionalLightDirection = myDirectionalLight.direction;
		frameBufferData.ambientLightColorAndIntensity = myAmbientLight.colorAndIntensity;
		frameBufferData.cameraPosition = myCamera->myTransform.GetTranslation();
		frameBufferData.resolution = { WindowHandler::GetInstance().GetWidth(), WindowHandler::GetInstance().GetHeight() };
		frameBufferData.deltaTime = static_cast<float>(myTimerPtr->GetDeltaTime());
		frameBufferData.totalTime = static_cast<float>(myTimerPtr->GetTotalTime());
		frameBufferData.clipHeight = aWaterPlane->myTransform.GetTranslation().y;

		D3D11_MAPPED_SUBRESOURCE mappedFrameBuffer = {};
		myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedFrameBuffer);
		memcpy(mappedFrameBuffer.pData, &frameBufferData, sizeof(frameBufferData));
		myContext->Unmap(myFrameBuffer.Get(), 0);

		myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	}

	for (std::shared_ptr<Mesh> mesh : aMeshVector)
	{
		PrepareObjectBufferData(mesh);
		mesh->Render(this);
	}
	if (aTransparentObjects)
	{
		myContext->OMSetBlendState(myAlphaBlendState.Get(), blendFactor, 0xffffffff);
		myContext->OMSetDepthStencilState(myReadOnlyDepthStencilState.Get(), 0);

		for (std::shared_ptr<Mesh> mesh : *aTransparentObjects)
		{
			PrepareObjectBufferData(mesh);
			mesh->Render(this);
		}

		myContext->OMSetBlendState(myOpaqueBlendState.Get(), blendFactor, 0xffffffff);
		myContext->OMSetDepthStencilState(myDefaultDepthStencilState.Get(), 0);
		myContext->RSSetState(myBackFaceCullingRasterizerState.Get());
	}

	// Step 2: Render the main scene to the main render target--------------------------------------------------------------

	// Set render target back to the main render taget
	myContext->OMSetRenderTargets(1, myMainRenderTarget.renderTargetView.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myMainRenderTarget.renderTargetView.Get(), blendFactor);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	PrepareFrameBufferData(-1000.0f);
	myContext->RSSetState(myBackFaceCullingRasterizerState.Get());

	for (std::shared_ptr<Mesh> mesh : aMeshVector)
	{
		PrepareObjectBufferData(mesh);
		mesh->Render(this);
	}
	if (aTransparentObjects)
	{
		myContext->OMSetBlendState(myAlphaBlendState.Get(), blendFactor, 0xffffffff);
		myContext->OMSetDepthStencilState(myReadOnlyDepthStencilState.Get(), 0);

		for (std::shared_ptr<Mesh> mesh : *aTransparentObjects)
		{
			PrepareObjectBufferData(mesh);
			mesh->Render(this);
		}
	}

	// Step 3: Render the water plane with the reflection texture----------------------------------------------------
	{
		ObjectBufferData objectBufferData = {};
		objectBufferData.modelToWorldMatrix = aWaterPlane->myTransform.myMatrix;

		D3D11_MAPPED_SUBRESOURCE mappedObjectBuffer = {};
		myContext->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectBuffer);
		memcpy(mappedObjectBuffer.pData, &objectBufferData, sizeof(ObjectBufferData));
		myContext->Unmap(myObjectBuffer.Get(), 0);

		myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
		myContext->PSSetShaderResources(10, 1, myIntermediateRenderTarget.shaderResourceView.GetAddressOf());

		aWaterPlane->Render(this);
	}

}

//GETTERS AND SETTERS--------------------------------------------------------------------------------------------------------

void GraphicsEngine::SetBackgroundColor(float aR, float aG, float aB, float aA)
{
	myBackgroundColor[0] = aR;
	myBackgroundColor[1] = aG;
	myBackgroundColor[2] = aB;
	myBackgroundColor[3] = aA;
}

void GraphicsEngine::SetBackBuffer()
{
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
}

void GraphicsEngine::SetMainRenderTarget()
{
	myContext->PSSetShaderResources(10, 1, myMainRenderTarget.shaderResourceView.GetAddressOf());
}

void GraphicsEngine::SetRasterizerState(eRasteriserState aRasteriserState)
{
	switch (aRasteriserState)
	{
	case eRasteriserState::frontCulling:
	{
		myContext->RSSetState(myFrontFaceCullingRasterizerState.Get());
		break;
	}
	case eRasteriserState::backCulling:
	{
		myContext->RSSetState(myBackFaceCullingRasterizerState.Get());
		break;
	}
	}
}

void GraphicsEngine::SetDirectionalLight(directionalLight aDirectionalLight)
{
	myDirectionalLight = aDirectionalLight;
}

void GraphicsEngine::SetAmbientLight(ambientLight aAmbientLight)
{
	myAmbientLight = aAmbientLight;
}

void GraphicsEngine::ClearTargets()
{
	myContext->ClearRenderTargetView(myIntermediateRenderTarget.renderTargetView.Get(), myBackgroundColor);
	myContext->ClearRenderTargetView(myMainRenderTarget.renderTargetView.Get(), myBackgroundColor);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

ComPtr<ID3D11DeviceContext> GraphicsEngine::GetDeviceContext() const
{
	return myContext;
}

ComPtr<ID3D11Device> GraphicsEngine::GetDevice() const
{
	return myDevice;
}

ComPtr<ID3D11SamplerState> GraphicsEngine::GetSamplerState() const
{
	return mySamplerState;
}

Camera& GraphicsEngine::GetCamera()
{
	return *myCamera;
}

RenderTarget& GraphicsEngine::GetMainRenderTarget()
{
	return myMainRenderTarget;
}

//INTERNAL FUNCTIONS__________________________________________________________________________________________________________
void GraphicsEngine::PresentFrame()
{
	// Present the frame
	HRESULT result = mySwapChain->Present(1, 0);
	if (FAILED(result))
	{
		FileLogger::Get().LogWarning("Failed to present the swap chain", result);
		FileLogger::Get().Flush();
	}

	myContext->ClearRenderTargetView(myBackBuffer.Get(), myBackgroundColor);
	myContext->ClearDepthStencilView
	(
		myDepthBuffer.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);
}

bool GraphicsEngine::InitializeSwapChain(HWND windowHandle)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	UINT creationFlags = 0;

#if _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext
	);

	if (FAILED(result))
	{
		FileLogger::Get().LogError("D3D11CreateDeviceAndSwapChain failed", result);
		return false;
	}

	return SUCCEEDED(result);
}

bool GraphicsEngine::CreateBackBuffer()
{
	ID3D11Texture2D* backBufferTexture = nullptr;
	HRESULT result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("GetBuffer failed", result);
		FileLogger::Get().Flush();
		return false;
	}

	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);
	backBufferTexture->GetDesc(&myBackBufferDesc);
	backBufferTexture->Release();

	return SUCCEEDED(result);
}

bool GraphicsEngine::CreateConstantBuffers()
{
	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	HRESULT result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create frame buffer", result);
		FileLogger::Get().Flush();
		return false;
	}

	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create object buffer", result);
		FileLogger::Get().Flush();
		return false;
	}

	bufferDescription.ByteWidth = sizeof(TerrainBuffer);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myTerrainBuffer);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create terrain buffer", result);
		FileLogger::Get().Flush();
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT result = myDevice->CreateSamplerState(&samplerDesc, &mySamplerState);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create sampler state", result);
		FileLogger::Get().Flush();
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = static_cast<unsigned int>(myBackBufferDesc.Width);
	depthBufferDesc.Height = static_cast<unsigned int>(myBackBufferDesc.Height);
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* depthBufferTexture = nullptr;
	HRESULT result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBufferTexture);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create texture 2D", result);
		FileLogger::Get().Flush();
		return false;
	}

	result = myDevice->CreateDepthStencilView(depthBufferTexture, nullptr, &myDepthBuffer);
	depthBufferTexture->Release();
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create depth stencil view", result);
		FileLogger::Get().Flush();
		return false;
	}

	return true;
}

void GraphicsEngine::SetRenderTargetsAndViewport()
{
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(myBackBufferDesc.Width);
	viewport.Height = static_cast<float>(myBackBufferDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	myContext->RSSetViewports(1, &viewport);
}

bool GraphicsEngine::CreateRasterizerStates()
{
	HRESULT result;

	//frontface---------------------------------------------------------------------------------------------------
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	result = myDevice->CreateRasterizerState
	(
		&rasterizerDesc, &myFrontFaceCullingRasterizerState
	);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create front face culling razterizer state", result);
		FileLogger::Get().Flush();
		return false;
	}

	//frontface---------------------------------------------------------------------------------------------------
	rasterizerDesc = {};
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	result = myDevice->CreateRasterizerState
	(
		&rasterizerDesc, &myBackFaceCullingRasterizerState
	);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create back face culling razterizer state", result);
		FileLogger::Get().Flush();
		return false;
	}

	//no cull---------------------------------------------------------------------------------------------------
	rasterizerDesc = {};
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	result = myDevice->CreateRasterizerState
	(
		&rasterizerDesc, &myNoFaceCullingRasterizerState
	);
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create no face culling razterizer state", result);
		FileLogger::Get().Flush();
		return false;
	}

	return true;
}

void GraphicsEngine::PrepareFrameBufferData(float aClipHeight)
{
	// Prepare FrameBufferData
	// This sets up the camera on the GPU

	// packs data in struct
	FrameBufferData frameBufferData = {};
	frameBufferData.worldToClipMatrix = myCamera->GetWorldToClipMatrix();
	frameBufferData.directionalLightColorAndIntensity = myDirectionalLight.colorAndIntensity;
	frameBufferData.directionalLightDirection = myDirectionalLight.direction;
	frameBufferData.ambientLightColorAndIntensity = myAmbientLight.colorAndIntensity;
	frameBufferData.cameraPosition = myCamera->myTransform.GetTranslation();
	frameBufferData.resolution = { WindowHandler::GetInstance().GetWidth(), WindowHandler::GetInstance().GetHeight() };
	frameBufferData.deltaTime = static_cast<float>(myTimerPtr->GetDeltaTime());
	frameBufferData.totalTime = static_cast<float>(myTimerPtr->GetTotalTime());
	frameBufferData.clipHeight = aClipHeight;

	// Maps struct to GPU memory
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer.Get(), 0);

	// Sets cbuffer to slot 0
	myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
}

void GraphicsEngine::PrepareObjectBufferData(std::shared_ptr<Mesh> aMesh)
{
	ObjectBufferData objectBufferData = {};
	objectBufferData.modelToWorldMatrix = aMesh->myTransform.myMatrix;

	D3D11_MAPPED_SUBRESOURCE mappedObjectBuffer = {};
	myContext->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectBuffer);
	memcpy(mappedObjectBuffer.pData, &objectBufferData, sizeof(ObjectBufferData));
	myContext->Unmap(myObjectBuffer.Get(), 0);
	myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
}

void GraphicsEngine::PrepareTerrainBufferData(TerrainBuffer* someTerrainData)
{
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	HRESULT result = myContext->Map(myTerrainBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	if (SUCCEEDED(result))
	{
		memcpy(mappedBuffer.pData, someTerrainData, sizeof(TerrainBuffer));
		myContext->Unmap(myTerrainBuffer.Get(), 0);

		// Sets cbuffer to slot 2
		myContext->VSSetConstantBuffers(2, 1, myTerrainBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers(2, 1, myTerrainBuffer.GetAddressOf());
	}
	else
	{
		FileLogger::Get().LogWarning("Failed to map terrain buffer", result);
		FileLogger::Get().Flush();
	}
}

bool GraphicsEngine::CreateDepthStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC defaultDesc = {};
	defaultDesc.DepthEnable = TRUE; 
	defaultDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; 
	defaultDesc.DepthFunc = D3D11_COMPARISON_LESS; 
	defaultDesc.StencilEnable = FALSE; 
	HRESULT result = myDevice->CreateDepthStencilState(&defaultDesc, myDefaultDepthStencilState.GetAddressOf());
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create default depth stencil state", result);
		FileLogger::Get().Flush();
		return false;
	}


	D3D11_DEPTH_STENCIL_DESC readOnlyDesc = {};
	readOnlyDesc.DepthEnable = FALSE; 
	readOnlyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDesc.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDesc.StencilEnable = FALSE; 
	result = myDevice->CreateDepthStencilState(&readOnlyDesc, myReadOnlyDepthStencilState.GetAddressOf());
	if (FAILED(result))
	{
		FileLogger::Get().LogError("Failed to create read only depth stencil state", result);
		FileLogger::Get().Flush();
		return false;
	}

	return true;
}

