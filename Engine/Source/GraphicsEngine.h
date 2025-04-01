#pragma once
#include <d3d11.h>
#include "Terrain.h"
#include "Light.h"
#include "Mesh.h"

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

struct RenderTarget
{
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};

enum class eRasteriserState
{
    frontCulling = 0,
    backCulling
};

//Forward declarations
struct TerrainBuffer;
class Camera;
class Terrain;
class WaterPlane;

class GraphicsEngine
{
public:
    //Constructor/destructor
    GraphicsEngine() = default;
    ~GraphicsEngine() = default;

    //innit
    bool Initialize(CommonUtilities::Timer* aTimerPtr);
    bool CreateRenderTarget(RenderTarget* aRenderTarget);

    //Update
    void PrepareTerrainBufferData(TerrainBuffer* someTerrainData);
    void UpdateObjectBuffer(const CommonUtilities::Matrix4x4<float> aMatrix);
    bool PreFrame();
    void ClearTargets();

    //Render
    void PresentFrame();
    void RenderWithReflection(const std::vector<std::shared_ptr<Mesh>>& aMeshVector, WaterPlane* aWaterPlane, const std::vector<std::shared_ptr<Mesh>>* aTransparentObjects = nullptr);

    //Setters
    inline void SetCamera(Camera* aCamera) { myCamera = aCamera; };
    void SetBackgroundColor(float aR, float aG, float aB, float aA);
    void SetBackBuffer();
    void SetMainRenderTarget();
    void SetRasterizerState(eRasteriserState aRasteriserState);
    void SetDirectionalLight(directionalLight aDirectionalLight);
    void SetAmbientLight(ambientLight aAmbientLight);

    //Getters
    inline const directionalLight GetDirectionalLight() { return myDirectionalLight; };
    inline const ambientLight GetAmbientLight() { return myAmbientLight; };
    ComPtr<ID3D11DeviceContext> GetDeviceContext() const;
    ComPtr<ID3D11Device> GetDevice() const;
    ComPtr<ID3D11SamplerState> GetSamplerState() const;
    Camera& GetCamera();
    RenderTarget& GetMainRenderTarget();

private:
    bool CreateBackBuffer();
    bool CreateConstantBuffers();
    bool CreateSamplerState();
    bool CreateBlendStates();
    bool CreateDepthBuffer();
    bool CreateRasterizerStates();
    bool CreateDepthStencilStates();

    bool InitializeSwapChain(HWND windowHandle);
    void SetRenderTargetsAndViewport();
    void PrepareFrameBufferData(float aClipHeight = 0.0f);
    void PrepareObjectBufferData(std::shared_ptr<Mesh> aMesh);

    ComPtr<ID3D11Device> myDevice;
    ComPtr<ID3D11DeviceContext> myContext;
    ComPtr<IDXGISwapChain> mySwapChain;
    ComPtr<ID3D11RenderTargetView> myBackBuffer;
    ComPtr<ID3D11DepthStencilView> myDepthBuffer;
    ComPtr<ID3D11Buffer> myFrameBuffer;
    ComPtr<ID3D11Buffer> myObjectBuffer;
    ComPtr<ID3D11Buffer> myTerrainBuffer;
    ComPtr<ID3D11ShaderResourceView> myTextureView;
    ComPtr<ID3D11SamplerState> mySamplerState;
    ComPtr<ID3D11RasterizerState> myFrontFaceCullingRasterizerState;
    ComPtr<ID3D11RasterizerState> myBackFaceCullingRasterizerState;
    ComPtr<ID3D11RasterizerState> myNoFaceCullingRasterizerState;
    ComPtr<ID3D11DepthStencilState> myDefaultDepthStencilState;
    ComPtr<ID3D11DepthStencilState> myReadOnlyDepthStencilState;
    ComPtr<ID3D11BlendState> myAlphaBlendState;
    ComPtr<ID3D11BlendState> myOpaqueBlendState;
    D3D11_TEXTURE2D_DESC myBackBufferDesc;

    Camera* myCamera = nullptr;
    float myBackgroundColor[4] = { 1,0,0,1 };
    ambientLight myAmbientLight;
    directionalLight myDirectionalLight;
    RenderTarget myIntermediateRenderTarget;
    RenderTarget myMainRenderTarget;

    CommonUtilities::Timer* myTimerPtr;
};  