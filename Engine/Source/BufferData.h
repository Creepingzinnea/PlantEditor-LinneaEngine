#pragma once
#include <Matrix4x4.hpp>
#include <Timer.h>

struct FrameBufferData
{
	CommonUtilities::Matrix4x4<float> myWorldToDirectionalLightTransform;        //64 bytes
	CommonUtilities::Matrix4x4<float> worldToClipMatrix;
	CommonUtilities::Vector4<float> directionalLightColorAndIntensity;
	CommonUtilities::Vector4<float> ambientLightColorAndIntensity;

	CommonUtilities::Vector3<float> directionalLightDirection;
	float padding;

	CommonUtilities::Vector3<float>  cameraPosition;
	float padding1;

	CommonUtilities::Vector2<unsigned int> resolution;
	float deltaTime;
	float totalTime;

	float clipHeight;
	CommonUtilities::Vector3<float> padding2;
};

struct ObjectBufferData
{
	CommonUtilities::Matrix4x4<float> modelToWorldMatrix;
};

struct TerrainBuffer //register(b2)
{
	float heightBlendMin = -1.167f;
	float heightBlendMax = -0.4f;

	float slopeBlendMin = 0.6f;
	float slopeBlendMax = 1.0f;

	float blendStrength = 1.0f;
	CommonUtilities::Vector3<float> padding3;
};
