#pragma once
#include "Matrix4x4.hpp"
#include "Vector3.hpp"
#include <Matrix3x3.hpp>

using vector4f = CommonUtilities::Vector4<float>;
using vector3f = CommonUtilities::Vector3<float>;
using vector2f = CommonUtilities::Vector2<float>;

class Transform
{
public:
	Transform() = default;

	CommonUtilities::Matrix4x4<float> myMatrix;

	//Getters---------------------------------------------------------------------------------------------------------------------------------------
	inline vector3f GetTranslation() { return { myMatrix(4, 1),	myMatrix(4, 2),		myMatrix(4, 3) }; }
	inline vector3f GetScale() { return myScale; };
	inline vector3f GetForward() { return { myMatrix(3, 1),	myMatrix(3, 2),		myMatrix(3, 3) }; };
	inline vector3f GetUp() { return { myMatrix(2, 1),	myMatrix(2, 2),		myMatrix(2, 3) }; }
	inline vector3f GetRight() { return { myMatrix(1, 1),	myMatrix(1, 2),		myMatrix(1, 3) }; };
	inline vector3f GetBack() { return { -myMatrix(3, 1),	-myMatrix(3, 2),	-myMatrix(3, 3) }; };
	inline vector3f GetDown() { return { -myMatrix(2, 1),	-myMatrix(2, 2),	-myMatrix(2, 3) }; }
	inline vector3f GetLeft() { return { -myMatrix(1, 1),	-myMatrix(1, 2),	-myMatrix(1, 3) }; };
	CommonUtilities::Matrix3x3<float> GetRotationMatrix();

	//Transforms------------------------------------------------------------------------------------------------------------------------------------
	void SetTransform(vector3f aPosition, vector3f aRotation, vector3f aScale);
	void AddRotationWithMatrix(const CommonUtilities::Matrix3x3<float>& aRotationMatrix);
	void AddRotation(vector3f aRotationInDegrees);
	void SetRotation(vector3f aRotationInDegrees);
	void RotateWithForwardVector(vector3f aForwardVector);
	void Translate(const vector3f& aPosition);
	void AccumulateTranslation(const vector3f& aPosition);
	void Scale(const float& aScale);

	inline void TranslateX(const float aX) { myMatrix(4, 1) = aX; }
	inline void TranslateY(const float aY) { myMatrix(4, 2) = aY; }
	inline void TranslateZ(const float aZ) { myMatrix(4, 3) = aZ; }

	void TransformPoint(vector3f& aPoint);

	inline void AddRotationX(const float aX)
	{
		float radiansX = aX * (3.14159265358979323846f / 180.0f);
		CommonUtilities::Matrix4x4<float> rotation = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(radiansX);
		myMatrix = rotation * myMatrix;
	};

	inline void AddRotationY(const float aY)
	{
		float radiansY = aY * (3.14159265358979323846f / 180.0f);
		CommonUtilities::Matrix4x4<float> rotation = CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(radiansY);
		myMatrix = rotation * myMatrix;
	};

	inline void AddRotationZ(const float aZ)
	{
		float radiansZ = aZ * (3.14159265358979323846f / 180.0f);
		CommonUtilities::Matrix4x4<float> rotation = CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(radiansZ);
		myMatrix = rotation * myMatrix;
	};

	bool RenderImGuiAndReturnUpdate();

#ifdef _DEBUG
	void DebugOutput();
#endif

private:
	vector3f myScale = {1.0f, 1.0f, 1.0f};
};

