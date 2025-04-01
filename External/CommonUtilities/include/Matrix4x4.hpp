#pragma once
#include <cmath>
#include "Vector.h"

namespace CommonUtilities
{
    template <class T>
    class Matrix4x4
    {
    public:
        // Creates the identity matrix.
        Matrix4x4();
        Matrix4x4(const Matrix4x4<T>& aMatrix);

        // () operator for accessing element (row, column) for read/write or read, respectively.
        T& operator()(const int aRow, const int aColumn);
        const T& operator()(const int aRow, const int aColumn) const;

        // Static functions for creating rotation matrices.
        static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
        static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
        static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);

        // Static function for creating a transpose of a matrix.
        static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
        // Assumes aTransform is made up of nothing but rotations and translations.
        static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);

        // Arithmetic operators
        Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix) const;
        Matrix4x4<T>& operator+=(const Matrix4x4<T>& aMatrix);
        Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix) const;
        Matrix4x4<T>& operator-=(const Matrix4x4<T>& aMatrix);
        Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix) const;
        Matrix4x4<T>& operator*=(const Matrix4x4<T>& aMatrix);
        bool operator==(const Matrix4x4<T>& aMatrix) const;

        // Vector multiplication
        Vector4<T> operator*(Vector4<T>& aVector) const;

        // Assignment operator
        Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix);

    private:
        T myData[4][4];
    };

    template<class T>
    inline Matrix4x4<T>::Matrix4x4()
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
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
    inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                myData[i][j] = aMatrix.myData[i][j];
            }
        }
    }

    template<class T>
    T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
    {
        return myData[aRow - 1][aColumn - 1];
    }

    template<class T>
    const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
    {
        return myData[aRow - 1][aColumn - 1];
    }

    template<class T>
    inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
    {
        Matrix4x4<T> rotationMatrix;

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
    inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
    {
        Matrix4x4<T> rotationMatrix;

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
    inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
    {
        Matrix4x4<T> rotationMatrix;

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
    inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
    {
        Matrix4x4<T> transposedMatrix;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                transposedMatrix.myData[i][j] = aMatrixToTranspose.myData[j][i];
            }
        }

        return transposedMatrix;
    }

    template<class T>
    inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
    {
        Matrix4x4<T> inverse;

        inverse.myData[0][0] = aTransform.myData[0][0];
        inverse.myData[1][0] = aTransform.myData[0][1];
        inverse.myData[2][0] = aTransform.myData[0][2];

        inverse.myData[0][1] = aTransform.myData[1][0];
        inverse.myData[1][1] = aTransform.myData[1][1];
        inverse.myData[2][1] = aTransform.myData[1][2];

        inverse.myData[0][2] = aTransform.myData[2][0];
        inverse.myData[1][2] = aTransform.myData[2][1];
        inverse.myData[2][2] = aTransform.myData[2][2];

        //The last column; [3][X], represents the translation vector.   
        inverse.myData[3][0] = -(aTransform.myData[3][0] * inverse.myData[0][0] + aTransform.myData[3][1] * inverse.myData[1][0] + aTransform.myData[3][2] * inverse.myData[2][0]);
        inverse.myData[3][1] = -(aTransform.myData[3][0] * inverse.myData[0][1] + aTransform.myData[3][1] * inverse.myData[1][1] + aTransform.myData[3][2] * inverse.myData[2][1]);
        inverse.myData[3][2] = -(aTransform.myData[3][0] * inverse.myData[0][2] + aTransform.myData[3][1] * inverse.myData[1][2] + aTransform.myData[3][2] * inverse.myData[2][2]);

        inverse.myData[0][3] = static_cast<T>(0);
        inverse.myData[1][3] = static_cast<T>(0);
        inverse.myData[2][3] = static_cast<T>(0);
        inverse.myData[3][3] = static_cast<T>(1);

        return inverse;
    }

    template<class T>
    inline Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
    {
        if (this == &aMatrix)
        {
            return *this;
        }

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                myData[i][j] = aMatrix.myData[i][j];
            }
        }
        return *this;
    }

    template<class T>
    inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aMatrix) const
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
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
    inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix) const
    {
        Matrix4x4<T> result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result(i + 1, j + 1) = myData[i][j] + aMatrix.myData[i][j];
            }
        }
        return result;
    }

    template<class T>
    inline Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                myData[i][j] += aMatrix.myData[i][j];
            }
        }
        return *this;
    }

    template<class T>
    inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix) const
    {
        Matrix4x4<T> result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result(i + 1, j + 1) = myData[i][j] - aMatrix.myData[i][j];
            }
        }
        return result;
    }

    template<class T>
    inline Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                myData[i][j] -= aMatrix.myData[i][j];
            }
        }
        return *this;
    }

    template<class T>
    inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix) const
    {
        Matrix4x4<T> result;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.myData[i][j] = 0;
                for (int k = 0; k < 4; ++k)
                {
                    result(i + 1, j + 1) += myData[i][k] * aMatrix.myData[k][j];
                }
            }
        }
        return result;
    }

    template<class T>
    inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
    {
        Matrix4x4<T> result;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                T sum = 0;
                for (int k = 0; k < 4; ++k)
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
    Vector4<T> Matrix4x4<T>::operator*(Vector4<T>& aVector) const
    {
        Vector4<T> result;

        result.x = aVector.x * myData[0][0] + aVector.y * myData[1][0] + aVector.z * myData[2][0] + aVector.w * myData[3][0];
        result.y = aVector.x * myData[0][1] + aVector.y * myData[1][1] + aVector.z * myData[2][1] + aVector.w * myData[3][1];
        result.z = aVector.x * myData[0][2] + aVector.y * myData[1][2] + aVector.z * myData[2][2] + aVector.w * myData[3][2];
        result.w = aVector.x * myData[0][3] + aVector.y * myData[1][3] + aVector.z * myData[2][3] + aVector.w * myData[3][3];

        return result;
    }

    template<class T>
    Vector4<T> operator*(Vector4<T> aVector, Matrix4x4<T> aMatrix)
    {
        Vector4<T> result;

        result.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1);
        result.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2);
        result.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3);
        result.w = aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4);

        return result;
    }
}