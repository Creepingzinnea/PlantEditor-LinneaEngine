#include "pch.h"
#include "WaterPlane.h"
#include "GraphicsEngine.h"
#include "TGPUtilities.hpp"
#include "Texture.h"

WaterPlane::WaterPlane()
{
	myTexture = std::make_shared<Texture>();
	LoadPlane(1000, 1000);
	myTransform.SetTransform({ 0, 0, 0 }, { 0, 0, 0 }, { 1,1,1 });
	myTransform.AddRotation({ 90.0f, 0, 0 });

}

void WaterPlane::Init(GraphicsEngine* aGraphicsEngine)
{
	if (!LoadCustomShaders(*aGraphicsEngine, "../Assets/Shaders/WaterShader_PS.cso", "../Assets/Shaders/WaterShader_VS.cso"))
	{
		TGPUtilities::CustomdebugOutput("failed to initialize water plane shaders");
	}
}
