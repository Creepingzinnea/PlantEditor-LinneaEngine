#pragma once

namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		//Creates a null-vector
		Vector4<T>()
			: x(0), y(0), z(0), w(0)
		{
		};

		//Creates a vector (aX, aY, aZ)
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW)
			: x(aX), y(aY), z(aZ), w(aW)
		{
		};

		//Copy constructor (compiler generated)
		Vector4<T>(const Vector4<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector2) = default;

		//Destructor (compiler generated)
		~Vector4<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			return x * x + y * y + z * z + w * w;
		};

		//Returns the length of the vector
		T Length() const
		{
			return std::sqrt(x * x + y * y + z * z + w * w);
		};

		//Returns a normalized copy of this
		Vector4<T> GetNormalized() const
		{
			Vector4<T> copy(x, y, z, w);
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

		//Normalimyzes the vector
		void Normalize()
		{
			Vector4<T> temp(x, y, z, w);
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
					w = temp.w;
				}
		};

		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const
		{
			return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
		};
	};

	//Returns the vector sum of aVectorA and aVectorB
	template <class T> Vector4<T> operator+(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		return Vector4<T>(aVectorA.x + aVectorB.x, aVectorA.y + aVectorB.y, aVectorA.z + aVectorB.z, aVectorA.w + aVectorB.w);
	}

	//Returns the vector difference of aVectorA and aVectorB
	template <class T> Vector4<T> operator-(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		return Vector4<T>(aVectorA.x - aVectorB.x, aVectorA.y - aVectorB.y, aVectorA.z - aVectorB.z, aVectorA.w - aVectorB.w);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> copy
		(
			aVector.x * aScalar,
			aVector.y * aScalar,
			aVector.z * aScalar,
			aVector.w * aScalar
		);		
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		Vector4<T> copy
		(
			aVector.x * aScalar,
			aVector.y * aScalar,
			aVector.z * aScalar,
			aVector.w * aScalar
		);
		return copy;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> copy
		(
			aVector.x * (1 / aScalar),
			aVector.y * (1 / aScalar),
			aVector.z * (1 / aScalar),
			aVector.w * (1 / aScalar)
		);
		return copy;
	}

	//Equivalent to setting aVectorA to (aVectorA + aVectorB)
	template <class T> void operator+=(Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		aVectorA.x += aVectorB.x;
		aVectorA.y += aVectorB.y;
		aVectorA.z += aVectorB.z;
		aVectorA.w += aVectorB.w;
	}

	//Equivalent to setting aVectorA to (aVectorA - aVectorB)
	template <class T> void operator-=(Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		aVectorA.x -= aVectorB.x;
		aVectorA.y -= aVectorB.y;
		aVectorA.z -= aVectorB.z;
		aVectorA.w -= aVectorB.w;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
	}


}