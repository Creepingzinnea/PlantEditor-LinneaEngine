#pragma once
#include <cmath>

namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;

		//Creates a null-vector
		Vector2<T>()
			: x(0), y(0)
		{
		};

		//Creates a vector
		Vector2<T>(const T& aX, const T& aY)
			: x(aX), y(aY)
		{
		};

		//Copy constructor (compiler generated)
		Vector2<T>(const Vector2<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		//Destructor (compiler generated)
		~Vector2<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			return x * x + y * y;
		};

		//Returns the length of the vector
		T Length() const
		{
			return std::sqrt(x * x + y * y);
		};

		//Returns a normalized copy of this
		Vector2<T> GetNormalized() const
		{
			Vector2<T> copy(x, y);
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
			Vector2<T> temp(x, y);
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
			}
		};

		//Returns the dot product of this and aVector
		T Dot(const Vector2<T>& aVector) const
		{
			return x * aVector.x + y * aVector.y;
		};
	};

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector2<T> operator+(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		Vector2<T> copy(aVectorA.x + aVectorB.x, aVectorA.y + aVectorB.y);		
		return copy;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector2<T> operator-(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB) 
	{
		Vector2<T> copy(aVectorA.x - aVectorB.x, aVectorA.y - aVectorB.y);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar) 
	{
		Vector2<T> copy(aVector.x * aScalar, aVector.y * aScalar);
		return copy;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector) 
	{
		Vector2<T> copy(aVector.x * aScalar, aVector.y * aScalar);
		return copy;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar) 
	{
		Vector2<T> copy(aVector.x *= (1 / aScalar), aVector.y *= (1 / aScalar));
		return copy;
	}

	//Equivalent to setting aVectorA to (aVectorA + aVectorB)
	template <class T> void operator+=(Vector2<T>& aVectorA, const Vector2<T>& aVectorB) 
	{
		aVectorA.x += aVectorB.x;
		aVectorA.y += aVectorB.y;
	}

	//Equivalent to setting aVectorA to (aVectorA - aVectorB)
	template <class T> void operator-=(Vector2<T>& aVectorA, const Vector2<T>& aVectorB) 
	{
		aVectorA.x -= aVectorB.x;
		aVectorA.y -= aVectorB.y;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar) 
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar) 
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
	}

}
