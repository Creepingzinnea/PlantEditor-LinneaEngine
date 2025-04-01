#pragma once
#include "Vector.h"

namespace CommonUtilities
{

	template <typename T> class Plane
	{
	public:

		// Default constructor.
		Plane();

		// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 - aPoint0).
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);

		// Constructor taking a point and a normal.
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal);

		// Init the plane with three points, the same as the constructor above.
		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);

		// Init the plane with a point and a normal, the same as the constructor above.
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);

		// Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
		bool IsInside(const Vector3<T>& aPosition) const;

		// Returns the normal of the plane.
		const Vector3<T>& GetNormal() const;
		const Vector3<T>& GetPointOnPlane() const;


	private: 

		Vector3<T> myPoint0;
		Vector3<T> myNormal;

	};

	template<typename T>
	inline Plane<T>::Plane()
		: myPoint0({ 0, 0, 0 }), myNormal({ 0, 0, 0 })
	{
	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPoint0 = aPoint0;

		//With two direction-vectors we can calculate the normal by findning the cross product of the two vectors.
		//(aPoint1 - aPoint0) x (aPoint2 - aPoint0)
		Vector3<T> riktningsVektor0 = aPoint1 - myPoint0;
		Vector3<T> riktningsVektor1 = aPoint2 - myPoint0;

		myNormal = riktningsVektor0.Cross(riktningsVektor1);
	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
	{
		myPoint0 = aPoint0;
		myNormal = aNormal;

	}

	template<typename T>
	inline void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPoint0 = aPoint0;

		//With two direction-vectors we can calculate the normal by findning the cross product of the two vectors.
		//(aPoint1 - aPoint0) x (aPoint2 - aPoint0)
		Vector3<T> riktningsVektor0 = aPoint1 - myPoint0;
		Vector3<T> riktningsVektor1 = aPoint2 - myPoint0;

		myNormal = riktningsVektor0.Cross(riktningsVektor1);

	}

	template<typename T>
	inline void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myPoint0 = aPoint;
		myNormal = aNormal;

	}

	template<typename T>
	inline bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
	{
		return (aPosition - myPoint0).Dot(myNormal) <= 0;

	}



	template<typename T>
	inline const Vector3<T>& Plane<T>::GetNormal() const
	{
		return myNormal;

	}

	template<typename T>
	inline const Vector3<T>& Plane<T>::GetPointOnPlane() const
	{
		return myPoint0;

	}

}

