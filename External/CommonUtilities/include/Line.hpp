#pragma once
#include "Vector.h"

namespace CommonUtilities
{

	template <typename T> class Line
	{
	public:

		// Default constructor: there is no line, the normal is the zero vector.
		Line();

		// Copy constructor.
		Line(const Line <T>& aLine);

		// Constructor that takes two points that define the line, the direction is aPoint1 - aPoint0.
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

		// Init the line with two points, the same as the constructor above.
		void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

		// Init the line with a point and a direction.
		void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);

		// Returns whether a point is inside the line: it is inside when the point is on the line or on the side the normal is pointing away from.
		bool IsInside(const Vector2<T>& aPosition) const;

		// Returns the direction of the line.
		const Vector2<T>& GetDirection() const;

		// Returns the normal of the line, which is (-direction.y, direction.x).
		const Vector2<T>& GetNormal() const;

	private:

		Vector2<T> myPoint0;
		Vector2<T> myPoint1;


	};

	template<typename T>
	inline Line<T>::Line()
		: myPoint0({ 0, 0 }), myPoint1({ 0, 0 })
	{
	}

	template<typename T>
	inline Line<T>::Line(const Line<T>& aLine)
	{
		myPoint0 = aLine.myPoint0;
		myPoint1 = aLine.myPoint1;

	}

	template<typename T>
	inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint0 = aPoint0;
		myPoint1 = aPoint1;

	}

	template<typename T>
	inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint0 = aPoint0;
		myPoint1 = aPoint1;

	}

	template<typename T>
	inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
	{
		myPoint0 = aPoint;
		myPoint1 = myPoint0 + aDirection;

	}

	template<typename T>
	inline bool Line<T>::IsInside(const Vector2<T>& aPosition) const
	{
		Vector2<T> pa = aPosition - myPoint0;

		// Check if the point is on the side the normal is pointing away from
		return pa.Dot(GetNormal()) <= static_cast<T>(0);

	}

	template<typename T>
	inline const Vector2<T>& Line<T>::GetDirection() const
	{
		return myPoint1 - myPoint0;
	}

	template<typename T>
	inline const Vector2<T>& Line<T>::GetNormal() const
	{
		Vector2<T> direction = GetDirection();
		static Vector2<T> temp;
		temp = { -direction.y, direction.x };
		return temp;
	}

}

