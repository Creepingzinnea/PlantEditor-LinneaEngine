#include <assert.h>

namespace CommonUtilities
{
	template <typename T>
	T Max(const T& a, const T& b)
	{
		return (a > b) ? a : b;
	}

	template <typename T>
	T Min(const T& a, const T& b)
	{
		return (a < b) ? a : b;
	}

	template <typename T>
	T Abs(const T& a)
	{
		return (a < 0) ? -a : a;
	}

	template <typename T>
	T Clamp(const T& aValue, const T& aMin, const T& aMax)
	{
		assert(aMin <= aMax);

		if (aValue < aMin)
		{
			return aMin;
		}
		if (aValue > aMax)
		{
			return aMax;
		}
		else
		{
			return aValue;
		}
	}

	template <typename T>
	T Lerp(const T& a, const T& b, const float t)
	{
		return a + t * (b - a);
	}

	template <typename T>
	void Swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}
}