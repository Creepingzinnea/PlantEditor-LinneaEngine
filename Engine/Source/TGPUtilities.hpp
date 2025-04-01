#pragma once
#include <string>

namespace TGPUtilities
{
	inline void CustomdebugOutput(const char* aMessage)
	{
		std::string fullMessage = "LinneaWarning: ";
		fullMessage += aMessage;
		fullMessage += "\n";

		OutputDebugStringA(fullMessage.c_str());
	}

	inline float Clampf(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	inline int Clampi(int value, int min, int max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	inline CommonUtilities::Vector3<float> NormaliseRgb(CommonUtilities::Vector3<float> aRgbColor)
	{
		CommonUtilities::Vector3<float> newColor
		(
			aRgbColor.x / 255.0f,
			aRgbColor.y / 255.0f,
			aRgbColor.z / 255.0f
		);

		return newColor;
	}

	inline CommonUtilities::Vector4<float> NormaliseRgb(CommonUtilities::Vector4<float> aRgbColor)
	{
		CommonUtilities::Vector4<float> newColor
		(
			aRgbColor.x / 255.0f,
			aRgbColor.y / 255.0f,
			aRgbColor.z / 255.0f,
			aRgbColor.w / 225.0f
		);

		return newColor;
	}

	inline float NormaliseRgb(float aRgbColor)
	{
		float newColor = aRgbColor / 255.0f;
		return newColor;
	}
}

