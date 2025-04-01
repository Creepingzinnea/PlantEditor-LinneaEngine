#pragma once

namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		//Creates a null-vector
		Vector3<T>()
			: x(0), y(0), z(0)
		{
		};

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const T& aX, const T& aY, const T& aZ) 
			: x(aX), y(aY), z(aZ)
		{
		};

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector2) = default;

		//Destructor (compiler generated)
		~Vector3<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			return x * x + y * y + z * z;
		};

		//Returns the length of the vector
		T Length() const
		{
			return std::sqrt(x * x + y * y + z * z);
		};

		//Returns a normalized copy of this
		Vector3<T> GetNormalized() const
		{
			Vector3<T> copy(x, y, z);
			T m = copy.Length();

				if (m == 0)
				{
					return copy; //Om längden är 0 försök inte dividera
				}
				else
				{
					return copy * (1 / m);
				}
		};

		//Normalizes the vector
		void Normalize()
		{
			Vector3<T> temp(x, y, z);
			T m = temp.Length();

				if (m == 0)
				{
					return; //Om längden är 0 försök inte dividera
				}
				else
				{
					temp *= (1 / m);
					x = temp.x;
					y = temp.y;
					z = temp.z;
				}
		};

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const
		{
			return Vector3<T>
			(
				y * aVector.z - z * aVector.y,
				z * aVector.x - x * aVector.z,
				x * aVector.y - y * aVector.x
			);
		};

		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const
		{
			return x * aVector.x + y * aVector.y + z * aVector.z;
		};
	};

	//Returns the vector sum of aVectorA and aVectorB
	template <class T> Vector3<T> operator+(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return Vector3<T>(aVectorA.x + aVectorB.x, aVectorA.y + aVectorB.y, aVectorA.z + aVectorB.z);
	}

	//Returns the vector difference of aVectorA and aVectorB
	template <class T> Vector3<T> operator-(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return Vector3<T>(aVectorA.x - aVectorB.x, aVectorA.y - aVectorB.y, aVectorA.z - aVectorB.z);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> copy
		(
			aVector.x * aScalar,
			aVector.y * aScalar,
			aVector.z * aScalar
		);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		Vector3<T> copy
		(
			aVector.x * aScalar,
			aVector.y * aScalar,
			aVector.z * aScalar
		);
		return copy;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> copy
		(
			aVector.x * (1 / aScalar),
			aVector.y * (1 / aScalar),
			aVector.z * (1 / aScalar)
		);
		return copy;
	}

	//Equivalent to setting aVectorA to (aVectorA + aVectorB)
	template <class T> void operator+=(Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		aVectorA.x += aVectorB.x;
		aVectorA.y += aVectorB.y;
		aVectorA.z += aVectorB.z;
	}

	//Equivalent to setting aVectorA to (aVectorA - aVectorB)
	template <class T> void operator-=(Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		aVectorA.x -= aVectorB.x;
		aVectorA.y -= aVectorB.y;
		aVectorA.z -= aVectorB.z;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
	}


}