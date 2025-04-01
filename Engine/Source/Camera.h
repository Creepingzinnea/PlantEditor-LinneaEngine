#pragma once
#include <Matrix4x4.hpp>
#include <InputManager.h>
#include "Transform.h"

using vector3f = CommonUtilities::Vector3<float>;
using vector4f = CommonUtilities::Vector4<float>;

enum class eMovement
{
	free = 0,
	orbit
};

class Camera
{
public:
	Camera();
	~Camera() = default;

	Transform myTransform;
	vector4f Project(vector4f aVertex);
	vector4f Rasterize(vector4f aVertex);

	void SetTarget(Transform* aTransform, vector3f aTargetTranslationOffset = { 0,0,0 }, float aDistance = 1.0f);

	inline void SetTargetTranslationOffset(vector3f aTranslationOffset) { myTargetTranslationOffset = aTranslationOffset; }
	void SetTransform(vector3f aPosition, vector3f aRotation);
	void FreeMovement(CommonUtilities::InputManager& aInputManager,
		float aTimeDelta);
	void OrbitTarget(CommonUtilities::InputManager& aInputManager,
		float aTimeDelta);
	void Update(CommonUtilities::InputManager& aInputManager,
		float aTimeDelta, eMovement aMovement);
	void UpdateRenderSize();

	CommonUtilities::Matrix4x4<float> GetWorldToClipMatrix() const;
	inline float GetTargetDistance() { return myTargetDistance; };

private:
	Transform* myTarget = nullptr;
	vector3f myTargetTranslationOffset = { 0,0,0 };

	CommonUtilities::Matrix4x4<float> myProjection{};
	CommonUtilities::Vector2<float> myRenderSize{};

	vector3f GetForwardVector() const;

	POINT previousMouseposition = { 0,0 };

	const float myNearPlane = 0.01f;
	const float myFarPlane = 10000.0f;
	const float myFov = 90.0f;

	float myYawAngle = 0.0f;
	float myPitchAngle = 0.0f;
	float mySensitivity = 0.1f;
	float myAspectRatio;
	float myTargetDistance = 0.0f;
	const int mySpeed = 12;

	bool myFirstUpdate = true;

#ifdef  _DEBUG
	bool freeCam = false;
#endif //  _DEBUG
};

