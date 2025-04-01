#pragma once
#include <cmath>
#include "Vector.h"
#include "Matrix4x4.hpp"

namespace CommonUtilities
{
	template <class T>
	class Matrix3x3
	{
	public:
		// Constructor
		Matrix3x3();
		Matrix3x3(const Matrix3x3<T>& aMatrix);
		Matrix3x3(const Matrix4x4<T>& aMatrix);

		// Access operators
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		// Static functions for creating rotation matrices
		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

		// Matrix-vector multiplication
		Vector3<T> operator*(const Vector3<T>& aVector) const;

		// Arithmetic operators
		Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>& operator+=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>& operator-=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>& operator*=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix);

		// Comparison operator
		bool operator==(const Matrix3x3<T>& aMatrix) const;

	private:
		T myData[3][3];
	};

	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (!(i < j) && !(i > j))
				{
					myData[i][j] = static_cast<T>(1);
				}
				else
				{
					myData[i][j] = static_cast<T>(0);
				}
			}
		}
	}

	template<class T>
	Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				myData[i][j] = aMatrix.myData[i][j];
			}
		}
	}

	template<class T>
	Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				myData[i][j] = aMatrix(i + 1, j + 1);
			}
		}
	}

	template<class T>
	T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		return myData[aRow - 1][aColumn - 1];
	}

	template<class T>
	const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		return myData[aRow - 1][aColumn - 1];
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;

		T cosAngle = std::cos(aAngleInRadians);
		T sinAngle = std::sin(aAngleInRadians);

		rotationMatrix.myData[0][0] = static_cast<T>(1);
		rotationMatrix.myData[1][1] = cosAngle;
		rotationMatrix.myData[1][2] = sinAngle;
		rotationMatrix.myData[2][1] = -sinAngle;
		rotationMatrix.myData[2][2] = cosAngle;

		return rotationMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;

		T cosAngle = std::cos(aAngleInRadians);
		T sinAngle = std::sin(aAngleInRadians);

		rotationMatrix.myData[0][0] = cosAngle;
		rotationMatrix.myData[0][2] = -sinAngle;
		rotationMatrix.myData[1][1] = static_cast<T>(1);
		rotationMatrix.myData[2][0] = sinAngle;
		rotationMatrix.myData[2][2] = cosAngle;

		return rotationMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;

		T cosAngle = std::cos(aAngleInRadians);
		T sinAngle = std::sin(aAngleInRadians);

		rotationMatrix.myData[0][0] = cosAngle;
		rotationMatrix.myData[0][1] = sinAngle;
		rotationMatrix.myData[1][0] = -sinAngle;

		rotationMatrix.myData[1][1] = cosAngle;
		rotationMatrix.myData[2][2] = static_cast<T>(1);

		return rotationMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> transposedMatrix;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				transposedMatrix.myData[i][j] = aMatrixToTranspose.myData[j][i];
			}
		}

		return transposedMatrix;
	}

	template<class T>
	Vector3<T> Matrix3x3<T>::operator*(const Vector3<T>& aVector) const
	{
		Vector3<T> result;

		result.x = myData[0][0] * aVector.x + myData[0][1] * aVector.y + myData[0][2] * aVector.z;
		result.y = myData[1][0] * aVector.x + myData[1][1] * aVector.y + myData[1][2] * aVector.z;
		result.z = myData[2][0] * aVector.x + myData[2][1] * aVector.y + myData[2][2] * aVector.z;

		return result;
	}

	template<class T>
	Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> result;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				result(i + 1, j + 1) = myData[i][j] + aMatrix.myData[i][j];
			}
		}
		return result;
	}

	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				myData[i][j] += aMatrix.myData[i][j];
			}
		}
		return *this;
	}

	template<class T>
	Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> result;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				result(i + 1, j + 1) = myData[i][j] - aMatrix.myData[i][j];
			}
		}
		return result;
	}

	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				myData[i][j] -= aMatrix.myData[i][j];
			}
		}
		return *this;
	}

	template<class T>
	Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> result;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				result.myData[i][j] = 0;
				for (int k = 0; k < 3; ++k)
				{
					result(i + 1, j + 1) += myData[i][k] * aMatrix.myData[k][j];
				}
			}
		}
		return result;
	}

	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> result;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				T sum = 0;
				for (int k = 0; k < 3; ++k)
				{
					sum += myData[i][k] * aMatrix.myData[k][j];
				}
				result(i + 1, j + 1) = sum;
			}
		}
		*this = result;
		return *this;
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		if (this == &aMatrix)
		{
			return *this;
		}

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				myData[i][j] = aMatrix.myData[i][j];
			}
		}
		return *this;
	}

	template<class T>
	inline bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aMatrix) const
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (myData[i][j] != aMatrix.myData[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}

	template<class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		Vector3<T> result;

		result.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1);
		result.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2);
		result.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3);

		return result;
	}

}




