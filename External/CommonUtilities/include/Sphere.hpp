#pragma once
#include "Vector.h"

namespace CommonUtilities
{

	template <typename T> class Sphere
	{
	public:

		// Default constructor: there is no sphere, the radius is zero and the position is 
		// the zero vector.
		Sphere();

		// Copy constructor.
		Sphere(const Sphere<T>& aSphere);

		// Constructor that takes the center position and radius of the sphere.
		Sphere(const Vector3<T>& aPosition, T aRadius);

		// Init the sphere with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector3<T>& aPosition, T aRadius);

		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInside(const Vector3<T>& aPosition) const;

		Vector3<T> GetPosition() const;
		T GetRadius() const;


	private:
		Vector3<T> myPosition;
		T myRadius;

	};

	template<typename T>
	inline Sphere<T>::Sphere()
		: myPosition({ 0, 0, 0 }), myRadius(0)
	{
	}

	template<typename T>
	inline Sphere<T>::Sphere(const Sphere<T>& aSphere)
	{
		myPosition = aSphere.myPosition;
		myRadius = aSphere.myRadius;

	}

	template<typename T>
	inline Sphere<T>::Sphere(const Vector3<T>& aPosition, T aRadius)
	{
		myPosition = aPosition;
		myRadius = aRadius;

	}

	template<typename T>
	inline void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aPosition, T aRadius)
	{
		myPosition = aPosition;
		myRadius = aRadius;

	}

	template<typename T>
	inline bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
	{
		// Get the vector from the center of the sphere to aPosition
		Vector3<T> vectorTemp = aPosition - myPosition;

		// Calculate the length of the vector
		T distance = vectorTemp.Length();

		// Compare the length with the radius
		// If the distance from the point to the sphere is smaller than the radius of the sphere,
		// it IS inside the sphere, will return true.
		return distance <= myRadius;

	}

	template<typename T>
	inline Vector3<T> Sphere<T>::GetPosition() const
	{
		return myPosition;
	}

	template<typename T>
	inline T Sphere<T>::GetRadius() const
	{
		return myRadius;
	}


}