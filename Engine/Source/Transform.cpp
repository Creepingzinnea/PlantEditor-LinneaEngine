#include "pch.h"
#include "Transform.h"
#include <imgui.h>

CommonUtilities::Matrix3x3<float> Transform::GetRotationMatrix()
{
	CommonUtilities::Matrix3x3<float> rotationMatrix;

	rotationMatrix(1, 1) = myMatrix(1, 1);
	rotationMatrix(1, 2) = myMatrix(1, 2);
	rotationMatrix(1, 3) = myMatrix(1, 3);

	rotationMatrix(2, 1) = myMatrix(2, 1);
	rotationMatrix(2, 2) = myMatrix(2, 2);
	rotationMatrix(2, 3) = myMatrix(2, 3);

	rotationMatrix(3, 1) = myMatrix(3, 1);
	rotationMatrix(3, 2) = myMatrix(3, 2);
	rotationMatrix(3, 3) = myMatrix(3, 3);

	return rotationMatrix;
}

void Transform::AddRotationWithMatrix(const CommonUtilities::Matrix3x3<float>& aRotationMatrix)
{
	CommonUtilities::Matrix4x4<float> rotationMatrix4x4;

	rotationMatrix4x4(1, 1) = aRotationMatrix(1, 1);
	rotationMatrix4x4(1, 2) = aRotationMatrix(1, 2);
	rotationMatrix4x4(1, 3) = aRotationMatrix(1, 3);

	rotationMatrix4x4(2, 1) = aRotationMatrix(2, 1);
	rotationMatrix4x4(2, 2) = aRotationMatrix(2, 2);
	rotationMatrix4x4(2, 3) = aRotationMatrix(2, 3);

	rotationMatrix4x4(3, 1) = aRotationMatrix(3, 1);
	rotationMatrix4x4(3, 2) = aRotationMatrix(3, 2);
	rotationMatrix4x4(3, 3) = aRotationMatrix(3, 3);

	rotationMatrix4x4(4, 1) = 0.0f;
	rotationMatrix4x4(4, 2) = 0.0f;
	rotationMatrix4x4(4, 3) = 0.0f;
	rotationMatrix4x4(4, 4) = 1.0f;

	myMatrix = rotationMatrix4x4 * myMatrix;
}

void Transform::SetTransform(vector3f aPosition, vector3f aRotation, vector3f aScale)
{
	//AddRotation
	myMatrix = myMatrix * myMatrix.CreateRotationAroundX(aRotation.x);
	myMatrix = myMatrix * myMatrix.CreateRotationAroundY(aRotation.y);
	myMatrix = myMatrix * myMatrix.CreateRotationAroundZ(aRotation.z);
	//Translate
	myMatrix(4, 1) = aPosition.x;
	myMatrix(4, 2) = aPosition.y;
	myMatrix(4, 3) = aPosition.z;
	//scale
	myMatrix(1, 1) = aScale.x;
	myMatrix(2, 2) = aScale.y;
	myMatrix(3, 3) = aScale.z;
}

void Transform::AddRotation(vector3f aRotation)
{
	float radiansX = aRotation.x * (3.14159265358979323846f / 180.0f);
	float radiansY = aRotation.y * (3.14159265358979323846f / 180.0f);
	float radiansZ = aRotation.z * (3.14159265358979323846f / 180.0f);

	// Create rotation matrices around local axes
	CommonUtilities::Matrix4x4<float> rotationX = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(radiansX);
	CommonUtilities::Matrix4x4<float> rotationY = CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(radiansY);
	CommonUtilities::Matrix4x4<float> rotationZ = CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(radiansZ);

	// Combine the rotations and add them to the matrix
	myMatrix = rotationX * rotationY * rotationZ * myMatrix;
}

void Transform::SetRotation(vector3f aRotation)
{
	// Preserve current translation
	vector3f currentTranslation(myMatrix(4, 1), myMatrix(4, 2), myMatrix(4, 3));

	// Create a new matrix with current translation, but reset rotation
	CommonUtilities::Matrix4x4<float> mat;
	myMatrix = mat;
	myMatrix(4, 1) = currentTranslation.x;
	myMatrix(4, 2) = currentTranslation.y;
	myMatrix(4, 3) = currentTranslation.z;

	float radiansX = aRotation.x * (3.14159265358979323846f / 180.0f);
	float radiansY = aRotation.y * (3.14159265358979323846f / 180.0f);
	float radiansZ = aRotation.z * (3.14159265358979323846f / 180.0f);

	// Create rotation matrices around local axes
	CommonUtilities::Matrix4x4<float> rotationX = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(radiansX);
	CommonUtilities::Matrix4x4<float> rotationY = CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(radiansY);
	CommonUtilities::Matrix4x4<float> rotationZ = CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(radiansZ);

	myMatrix = rotationX * rotationY * rotationZ * myMatrix;
}

void Transform::RotateWithForwardVector(vector3f aForwardVector)
{
	vector3f forward = aForwardVector.GetNormalized();
	vector3f worldUp(0.0f, 1.0f, 0.0f);
	vector3f right = worldUp.Cross(forward);

	if (right.LengthSqr() < 0.0001f) 
	{
		worldUp = vector3f(1.0f, 0.0f, 0.0f);
		right = worldUp.Cross(forward);
	}

	right.Normalize();
	vector3f up = forward.Cross(right);

	//Extract scale
	vector3f scale;
	scale.x = vector3f(myMatrix(1, 1), myMatrix(1, 2), myMatrix(1, 3)).Length();
	scale.y = vector3f(myMatrix(2, 1), myMatrix(2, 2), myMatrix(2, 3)).Length();
	scale.z = vector3f(myMatrix(3, 1), myMatrix(3, 2), myMatrix(3, 3)).Length();

	// Set the rotation part of the matrix
	myMatrix(1, 1) = right.x;   myMatrix(1, 2) = right.y;   myMatrix(1, 3) = right.z;
	myMatrix(2, 1) = up.x;      myMatrix(2, 2) = up.y;      myMatrix(2, 3) = up.z;
	myMatrix(3, 1) = forward.x; myMatrix(3, 2) = forward.y; myMatrix(3, 3) = forward.z;

	// Reapply scale
	myMatrix(1, 1) *= scale.x; myMatrix(1, 2) *= scale.x; myMatrix(1, 3) *= scale.x;
	myMatrix(2, 1) *= scale.y; myMatrix(2, 2) *= scale.y; myMatrix(2, 3) *= scale.y;
	myMatrix(3, 1) *= scale.z; myMatrix(3, 2) *= scale.z; myMatrix(3, 3) *= scale.z;
}

void Transform::Translate(const vector3f& aPosition)
{
	myMatrix(4, 1) = aPosition.x;
	myMatrix(4, 2) = aPosition.y;
	myMatrix(4, 3) = aPosition.z;
}

void Transform::AccumulateTranslation(const vector3f& aPosition)
{
	myMatrix(4, 1) += aPosition.x;
	myMatrix(4, 2) += aPosition.y;
	myMatrix(4, 3) += aPosition.z;
}

void Transform::Scale(const float& aScale)
{
	myScale = { aScale, aScale, aScale };

	// Store translation temporarily
	vector3f translation(myMatrix(4, 1), myMatrix(4, 2), myMatrix(4, 3));

	// Create a pure scale matrix
	CommonUtilities::Matrix4x4<float> scaleMatrix;
	scaleMatrix(1, 1) = aScale;
	scaleMatrix(2, 2) = aScale;
	scaleMatrix(3, 3) = aScale;

	// Apply scale and restore translation
	myMatrix *= scaleMatrix;
	myMatrix(4, 1) = translation.x;
	myMatrix(4, 2) = translation.y;
	myMatrix(4, 3) = translation.z;
	myMatrix(4, 4) = 1.0f;
}

void Transform::TransformPoint(vector3f& aPoint)
{
	float x = aPoint.x;
	float y = aPoint.y;
	float z = aPoint.z;
	float w = 1.0f;

	float newX = myMatrix(1, 1) * x + myMatrix(2, 1) * y + myMatrix(3, 1) * z + myMatrix(4, 1) * w;
	float newY = myMatrix(1, 2) * x + myMatrix(2, 2) * y + myMatrix(3, 2) * z + myMatrix(4, 2) * w;
	float newZ = myMatrix(1, 3) * x + myMatrix(2, 3) * y + myMatrix(3, 3) * z + myMatrix(4, 3) * w;

	aPoint.x = newX;
	aPoint.y = newY;
	aPoint.z = newZ;
}

bool Transform::RenderImGuiAndReturnUpdate()
{
	bool updated = false;

	ImGui::Begin("Matrix Transform");
	ImGui::Text("Transform");

	if (ImGui::BeginTable("matrix_table", 4, ImGuiTableFlags_Borders))
	{
		for (int row = 1; row <= 4; ++row)
		{
			ImGui::TableNextRow();
			for (int col = 1; col <= 4; ++col)
			{
				ImGui::TableSetColumnIndex(col - 1);

				// Create label for the matrix element
				std::string label = "Row:" + std::to_string(row) + " Col:" + std::to_string(col);
				std::string slider_id = "##" + label;

				// Additional logic to label the transformation components
				if (row == 1 && col == 1) 
				{
					label = "Scale X"; 
				}
				else if (row == 1 && col == 2) 
				{
					label = ""; 
				}
				else if (row == 1 && col == 3) 
				{
					label = ""; 
				}
				else if (row == 1 && col == 4) 
				{
					label = ""; 
				}
				else if (row == 2 && col == 1) 
				{
					label = ""; 
				}
				else if (row == 2 && col == 2) 
				{
					label = "Scale Y";
				}
				else if (row == 2 && col == 3) 
				{
					label = ""; 
				}
				else if (row == 2 && col == 4) 
				{
					label = "";
				}
				else if (row == 3 && col == 1) 
				{
					label = ""; 
				}
				else if (row == 3 && col == 2) 
				{
					label = ""; 
				}
				else if (row == 3 && col == 3) 
				{
					label = "Scale Z"; 
				}
				else if (row == 3 && col == 4) 
				{
					label = "";
				}
				else if (row == 4 && col == 1)
				{
					label = "Translate X";
				}
				else if (row == 4 && col == 2)
				{
					label = "Translate Y";
				}
				else if (row == 4 && col == 3)
				{
					label = "Translate Z";
				}
				else if (row == 4 && col == 4)
				{
					label = "Identity";
				}

				ImGui::Text("%s", label.c_str());

				if (ImGui::SliderFloat(slider_id.c_str(), &myMatrix(row, col), -10.0f, 10.0f))
				{
					updated = true;
				}
			}
		}
		ImGui::EndTable();

		ImGui::PushID(this);
		if (ImGui::Button("Reset to Identity Matrix", ImVec2(45, 20)))
		{
			CommonUtilities::Matrix4x4<float> identity;
			myMatrix = identity;
			updated = true;
		}
		ImGui::PopID();
	}

	ImGui::End();

	return updated;
}

#ifdef _DEBUG
void Transform::DebugOutput()
{
	static int frameCounter = 0;
	const int printInterval = 5;

	if (frameCounter % printInterval == 0)
	{
		system("cls");
		std::cout << "Unit Cube Transform Matrix:" << std::endl;
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				std::cout << myMatrix(row + 1, col + 1) << "\t";
			}
			std::cout << std::endl;
		}
	}

	frameCounter++;
}
#endif
