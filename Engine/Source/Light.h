#pragma once
#include <Vector4.hpp>
#include <Vector3.hpp>

struct directionalLight
{
	CommonUtilities::Vector4<float> colorAndIntensity;
	CommonUtilities::Vector3<float> direction;
};

struct ambientLight
{
	CommonUtilities::Vector4<float> colorAndIntensity;
};