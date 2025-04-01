#include "pch.h"
#include <InputManager.h>
#include <cmath>
#include <UtilityFunctions.hpp>
#include "Camera.h"
#include "TGPUtilities.hpp"


Camera::Camera()
{
	myRenderSize = { static_cast<float>(WindowHandler::GetInstance().GetWidth()),
		static_cast<float>(WindowHandler::GetInstance().GetHeight()) };
	myAspectRatio = myRenderSize.x / myRenderSize.y;
	const float radFov = myFov * (3.14f / 180); // Convert from degrees to radians

	// Projection Matrix
	myProjection(1, 1) = 1 / (tanf(radFov / 2));
	myProjection(2, 2) = myAspectRatio * (1 / (tanf(radFov / 2)));
	myProjection(3, 3) = 1.0f * myFarPlane / (myFarPlane - myNearPlane);
	myProjection(3, 4) = 1.0f;
	myProjection(4, 3) = -1.0f * myNearPlane * myFarPlane / (myFarPlane - myNearPlane);
	myProjection(4, 4) = 0;
}

vector4f Camera::Project(vector4f aVertex)
{
	CommonUtilities::Vector4<float> cameraSpaceVertex;

	// Transform the vertex to camera space
	CommonUtilities::Matrix4x4<float> inverseCameraTransform = myTransform.myMatrix.GetFastInverse(myTransform.myMatrix);
	cameraSpaceVertex = aVertex * inverseCameraTransform;

	// Apply projection matrix
	cameraSpaceVertex = cameraSpaceVertex * myProjection;

	return cameraSpaceVertex;
}

vector4f Camera::Rasterize(vector4f aVertex)
{
	// Perspective divide if necessary
	if (aVertex.w != 1.0f)
	{
		aVertex.x /= aVertex.w;
		aVertex.y /= aVertex.w;
		aVertex.z /= aVertex.w;
	}

	// Convert the vertex to raster space
	CommonUtilities::Vector4<float> rasterSpaceVertex;
	rasterSpaceVertex.x = aVertex.x * (myRenderSize.x / 2);
	rasterSpaceVertex.y = aVertex.y * (myRenderSize.y / 2);
	rasterSpaceVertex.x += myRenderSize.x / 2;
	rasterSpaceVertex.y += myRenderSize.y / 2;

	return rasterSpaceVertex;
}

void Camera::SetTarget(Transform* aTransform, vector3f aTargetTranslationOffset, float aDistance)
{
	myTarget = aTransform;
	myTargetTranslationOffset = aTargetTranslationOffset;
	myTargetDistance = aDistance;
}

void Camera::SetTransform(vector3f aPosition, vector3f aRotation)
{
	myTransform.myMatrix += myTransform.myMatrix.CreateRotationAroundX(aRotation.x);
	myTransform.myMatrix += myTransform.myMatrix.CreateRotationAroundY(aRotation.y);
	myTransform.myMatrix += myTransform.myMatrix.CreateRotationAroundZ(aRotation.z);

	myTransform.myMatrix(4, 1) = aPosition.x;
	myTransform.myMatrix(4, 2) = aPosition.y;
	myTransform.myMatrix(4, 3) = aPosition.z;
}

void Camera::FreeMovement(CommonUtilities::InputManager& aInputManager, float aTimeDelta)
{
	if (aInputManager.IsKeyHeld(VK_RBUTTON))
	{
		if (previousMouseposition.x == 0 && previousMouseposition.y == 0)
		{
			previousMouseposition = aInputManager.GetAbsoluteMousePosition();
		}

		vector4f forwardVector =
		{
			myTransform.myMatrix(3, 1),
			myTransform.myMatrix(3, 2),
			myTransform.myMatrix(3, 3),
			0.0f
		};

		CommonUtilities::Vector4<float> rightVector =
		{
			myTransform.myMatrix(1, 1),
			myTransform.myMatrix(1, 2),
			myTransform.myMatrix(1, 3),
			0.0f
		};

		vector4f up =
		{
			myTransform.myMatrix(2, 1),
			myTransform.myMatrix(2, 2),
			myTransform.myMatrix(2, 3),
			0
		};

		POINT currentMousePosition = aInputManager.GetAbsoluteMousePosition();
		CommonUtilities::Vector2<float> mouseDelta =
		{
			static_cast<float>(currentMousePosition.x - previousMouseposition.x),
			static_cast<float>(currentMousePosition.y - previousMouseposition.y)
		};
		previousMouseposition = currentMousePosition;

		//Rotation logic-------------------------------------------------------------------------------------------------------------------
		vector4f targetDirection = forwardVector;
		targetDirection = targetDirection + (rightVector * (mouseDelta.x * mySensitivity * aTimeDelta));
		targetDirection = targetDirection - (up * (mouseDelta.y * mySensitivity * aTimeDelta));
		targetDirection.Normalize();
		myTransform.RotateWithForwardVector({ targetDirection.x, targetDirection.y, targetDirection.z });
		//----------------------------------------------------------------------------------------------------------------------------------

		//Move back and forth
		if (aInputManager.IsKeyHeld('W'))
		{
			CommonUtilities::Vector4<float> velocity = (float)mySpeed * forwardVector;
			myTransform.myMatrix(4, 1) += velocity.x * aTimeDelta;
			myTransform.myMatrix(4, 2) += velocity.y * aTimeDelta;
			myTransform.myMatrix(4, 3) += velocity.z * aTimeDelta;
		}
		else if (aInputManager.IsKeyHeld('S'))
		{
			CommonUtilities::Vector4<float> velocity = (float)-mySpeed * forwardVector;
			myTransform.myMatrix(4, 1) += velocity.x * aTimeDelta;
			myTransform.myMatrix(4, 2) += velocity.y * aTimeDelta;
			myTransform.myMatrix(4, 3) += velocity.z * aTimeDelta;
		}
		//Move sideways
		if (aInputManager.IsKeyHeld('D'))
		{
			CommonUtilities::Vector4<float> velocity = (float)mySpeed * rightVector;
			myTransform.myMatrix(4, 1) += velocity.x * aTimeDelta;
			myTransform.myMatrix(4, 2) += velocity.y * aTimeDelta;
			myTransform.myMatrix(4, 3) += velocity.z * aTimeDelta;
		}
		else if (aInputManager.IsKeyHeld('A'))
		{
			CommonUtilities::Vector4<float> velocity = (float)-mySpeed * rightVector;
			myTransform.myMatrix(4, 1) += velocity.x * aTimeDelta;
			myTransform.myMatrix(4, 2) += velocity.y * aTimeDelta;
			myTransform.myMatrix(4, 3) += velocity.z * aTimeDelta;
		}
	}
	else
	{
		previousMouseposition.x = 0;
		previousMouseposition.y = 0;
	}
}

void Camera::OrbitTarget(CommonUtilities::InputManager& aInputManager, float aTimeDelta)
{
	if (aInputManager.IsKeyHeld(VK_RBUTTON) || myFirstUpdate)
	{
		myFirstUpdate = false;
		if (previousMouseposition.x == 0 && previousMouseposition.y == 0)
		{
			previousMouseposition = aInputManager.GetAbsoluteMousePosition();
		}

		vector4f forwardVector =
		{
			myTransform.myMatrix(3, 1),
			myTransform.myMatrix(3, 2),
			myTransform.myMatrix(3, 3),
			0.0f
		};

		CommonUtilities::Vector4<float> rightVector =
		{
			myTransform.myMatrix(1, 1),
			myTransform.myMatrix(1, 2),
			myTransform.myMatrix(1, 3),
			0.0f
		};

		vector4f up =
		{
			myTransform.myMatrix(2, 1),
			myTransform.myMatrix(2, 2),
			myTransform.myMatrix(2, 3),
			0
		};

		POINT currentMousePosition = aInputManager.GetAbsoluteMousePosition();
		CommonUtilities::Vector2<float> mouseDelta =
		{
			static_cast<float>(currentMousePosition.x - previousMouseposition.x),
			static_cast<float>(currentMousePosition.y - previousMouseposition.y)
		};
		previousMouseposition = currentMousePosition;

		//Rotation logic-------------------------------------------------------------------------------------------------------------------
		vector4f targetDirection = forwardVector;
		targetDirection = targetDirection + (rightVector * (mouseDelta.x * mySensitivity * aTimeDelta));
		targetDirection = targetDirection - (up * (mouseDelta.y * mySensitivity * aTimeDelta));
		targetDirection.Normalize();
		myTransform.RotateWithForwardVector({ targetDirection.x, targetDirection.y, targetDirection.z });
		//----------------------------------------------------------------------------------------------------------------------------------

		float zoom = static_cast<float>(aInputManager.GetScrollDelta()) * mySensitivity;
		myTargetDistance = CommonUtilities::Clamp(myTargetDistance -= zoom * aTimeDelta, 0.1f, 10.0f);

		//Translate-------------------------------------------------------------------------------------------------------------------------
		vector3f forwardVec3(forwardVector.x, forwardVector.y, forwardVector.z);
		vector3f translation = (myTarget->GetTranslation() + myTargetTranslationOffset) - forwardVec3 * myTargetDistance;
		myTransform.Translate(translation);

	}
	else
	{
		previousMouseposition.x = 0;
		previousMouseposition.y = 0;
	}
}

void Camera::Update(CommonUtilities::InputManager& aInputManager, float aTimeDelta, eMovement aMovement)
{
#ifdef _DEBUG
	if (aInputManager.IsKeyReleased('Q'))
	{
		freeCam = !freeCam;
	}
	if (freeCam)
	{
		aMovement = eMovement::free;
	}
#endif

	switch (aMovement)
	{
	case eMovement::free:
	{
		FreeMovement(aInputManager, aTimeDelta);
		break;
	}
	case eMovement::orbit:
	{
		OrbitTarget(aInputManager, aTimeDelta);
		break;
	}
	}

}

void Camera::UpdateRenderSize()
{
	myRenderSize = { static_cast<float>(WindowHandler::GetInstance().GetWidth()),
		static_cast<float>(WindowHandler::GetInstance().GetHeight()) };
	myAspectRatio = myRenderSize.x / myRenderSize.y;
	const float radFov = myFov * (3.14f / 180);

	myProjection(1, 1) = 1 / (tanf(radFov / 2));
	myProjection(2, 2) = myAspectRatio * (1 / (tanf(radFov / 2)));
	myProjection(3, 3) = 1.0f * myFarPlane / (myFarPlane - myNearPlane);
	myProjection(3, 4) = 1.0f;
	myProjection(4, 3) = -1.0f * myNearPlane * myFarPlane / (myFarPlane - myNearPlane);
	myProjection(4, 4) = 0;
}

CommonUtilities::Matrix4x4<float> Camera::GetWorldToClipMatrix() const
{
	return (myTransform.myMatrix.GetFastInverse(myTransform.myMatrix) * myProjection);
}

vector3f Camera::GetForwardVector() const
{
	vector3f forwardVector =
	{
		myTransform.myMatrix(3, 1),
		myTransform.myMatrix(3, 2),
		myTransform.myMatrix(3, 3)
	};

	return forwardVector;
}




