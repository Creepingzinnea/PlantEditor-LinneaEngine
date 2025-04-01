#pragma once
#include "Vector2.hpp"

namespace CommonUtilities
{
	class BoxCollider2D
	{
	public:

		BoxCollider2D();
		BoxCollider2D(const Vector2<float>& aPosition, float aWidth, float aHeight);

		bool IsCollidingWith(const BoxCollider2D& aCollider);
		void Update(const Vector2<float> aPosition);

	private:

		//Collider
		Vector2<float> myPosition;
		Vector2<float> myTopLeft;
		Vector2<float> myBottomRight;

		float myWidth;
		float myHeight;
	};
}
