#pragma once
#include "Vector3.hpp"

namespace CommonUtilities
{
	template <typename T> class Ray
	{
	public:

		// Default constructor: there is no ray, the origin and direction are the 
		// zero vector.
		Ray();

		// Copy constructor.
		Ray(const Ray<T>& aRay);

		// Constructor that takes two points that define the ray, the direction is 
		// aDirectionm and the origin is aOrigin.
		Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

		// Init the ray with two points, the same as the constructor above.
		void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);

		// Init the ray with an origin and a direction.
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

		Vector3<T> GetOrigin() const;
		Vector3<T> GetDirection() const;



	private:

		Vector3<T> myOrigin;
		Vector3<T> myDirection;

	};


	template<typename T>
	inline Ray<T>::Ray()
		: myOrigin({ 0,0,0 }), myDirection({ 0,0,0 })
	{
	}

	template<typename T>
	inline Ray<T>::Ray(const Ray<T>& aRay)
		: myOrigin(aRay.myOrigin), myDirection(aRay.myDirection)
	{
	}

	template<typename T>
	inline Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
		: myOrigin(aOrigin), myDirection(aDirection)
	{
	}

	template<typename T>
	inline void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myOrigin = aOrigin;
		myDirection = aPoint - myOrigin;
		myDirection.Normalize();

	}

	template<typename T>
	inline void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection;

	}



	template<typename T>
	inline Vector3<T> Ray<T>::GetOrigin() const
	{
		return myOrigin;
	}

	template<typename T>
	inline Vector3<T> Ray<T>::GetDirection() const
	{
		return myDirection;
	}


}