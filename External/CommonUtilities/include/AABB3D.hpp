#pragma once
#include "Vector.h"

namespace CommonUtilities
{
	template <typename T> class AABB3D
	{
	public:

		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB3D();

		// Copy constructor.
		AABB3D(const AABB3D<T>& aAABB3D);

		// Constructor taking the positions of the minimum and maximum corners.
		AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax);

		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);

		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector3<T>& aPosition) const;

		Vector3<T> GetMin() const;
		Vector3<T> GetMax() const;


	private:

		Vector3<T> myMin;
		Vector3<T> myMax;
	

	};


	
	template<typename T>
	inline AABB3D<T>::AABB3D()
		: myMin({ 0,0,0 }), myMax({ 0,0,0 })
	{
	}

	template<typename T>
	inline AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
		: myMin(aAABB3D.myMin), myMax(aAABB3D.myMax)
	{
	}

	template<typename T>
	inline AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
		: myMin(aMin), myMax(aMax)
	{
	}

	template<typename T>
	inline void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}

	template<typename T>
	inline bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
	{
		//Check if the AABB is below the point
		//X
		if (myMax.x < aPosition.x)
		{
			return false;
		}
		//Y
		if (myMax.y < aPosition.y)
		{
			return false;
		}
		//Z
		if (myMax.z < aPosition.z)
		{
			return false;
		}
		//Check if the AABB is above the point
		//X
		if (myMin.x > aPosition.x)
		{
			return false;
		}
		//Y
		if (myMin.y > aPosition.y)
		{
			return false;
		}
		//Z
		if (myMin.z > aPosition.z)
		{
			return false;
		}

		//If all above are false, the point must be inside the AABB
		return true;
	}

	template<typename T>
	inline Vector3<T> AABB3D<T>::GetMin() const
	{
		return myMin;
	}

	template<typename T>
	inline Vector3<T> AABB3D<T>::GetMax() const
	{
		return myMax;
	}

}


