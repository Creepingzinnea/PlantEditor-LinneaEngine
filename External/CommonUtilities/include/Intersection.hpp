#pragma once
#include "AABB3D.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "Plane.hpp"
#include <limits>


namespace CommonUtilities
{
	// If the ray does not intersect the plane aOutIntersectionPoint remains unchanged
	// and false is returned. 
	// If the ray intersects the plane the intersection point is stored in
	// aOutIntersectionPoint and true is returned.
	template<typename T>
	inline bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{

		Vector3<T> planeNormal = aPlane.GetNormal();
		Vector3<T> rayOrigin = aRay.GetOrigin();
		Vector3<T> rayDirection = aRay.GetDirection();
		Vector3<T> pointOnPlane = aPlane.GetPointOnPlane();

		Vector3<T> distance = pointOnPlane - rayOrigin;

		T denom = rayDirection.Dot(planeNormal);



		// If the denominator is close to 0 (perpendicular to the plane),
		// it will never intersect the plane, EXCEPT if the ray-origin is in the plane.
		if (abs(denom) < 1e-6f)
		{
			// Check if the projected distance onto the normal is close to 0. 
			// This means the rayOrigin is on the plane and will therefore intersect. 
			if (abs(distance.Dot(planeNormal)) < 1e-6f)
			{
				aOutIntersectionPoint = rayOrigin;

				return true;

			}

			return false;

		}

		// Calculate the scalar to find the intersection point
		T t = (planeNormal.Dot(distance) / denom);

		// if t is negative, no intersection. 
		if (t < 0)
		{
			return false;

		}

		// scale the ray to find the intersection point
		aOutIntersectionPoint = rayOrigin + rayDirection * t;

		return true;
	}

	// If the ray intersects the AABB, true is returned, if not, false is returned.
	// Any ray starting on the inside is considered to intersect the AABB
	template<typename T>
	inline bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{
		Vector3<T> rayDirection = aRay.GetDirection();
		Vector3<T> rayOrigin = aRay.GetOrigin();
		Vector3<T> BoxMax = aAABB.GetMax();
		Vector3<T> BoxMin = aAABB.GetMin();

		T txmin = (BoxMin.x - rayOrigin.x) / rayDirection.x;
		T txmax = (BoxMax.x - rayOrigin.x) / rayDirection.x;

		T tymin = (BoxMin.y - rayOrigin.y) / rayDirection.y;
		T tymax = (BoxMax.y - rayOrigin.y) / rayDirection.y;

		T tzmin = (BoxMin.z - rayOrigin.z) / rayDirection.z;
		T tzmax = (BoxMax.z - rayOrigin.z) / rayDirection.z;

		if (txmin > txmax)
		{
			std::swap(txmin, txmax);
		}

		if (tymin > tymax)
		{
			std::swap(tymin, tymax);
		}

		if (tzmin > tzmax)
		{
			std::swap(tzmin, tzmax);
		}

		if ((txmin > tymax) || (tymin > txmax))
		{
			return false;
		}

		if (tymin > txmin)
		{
			txmin = tymin;
		}

		if (tymax < txmax)
		{
			txmax = tymax;
		}

		if ((txmin > tzmax) || (tzmin > txmax))
		{
			return false;
		}

		if (tzmin > txmin)
		{
			txmin = tzmin;
		}

		aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * txmin;

		return true;

	}

	// If the ray intersects the sphere, true is returned, if not, false is returned.
	// Any ray starting on the inside is considered to intersect the sphere
	template<typename T>
	inline bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{
		Vector3<T> rayOrigin = aRay.GetOrigin();
		Vector3<T> sphereCenter = aSphere.GetPosition();
		Vector3<T> rayOriginToCenter = sphereCenter - rayOrigin;
		Vector3<T> rayDirection = aRay.GetDirection();

		//project "rayOriginToCenter" onto the ray
		Vector3<T> projectionVector = rayOriginToCenter.Dot(rayDirection) * rayDirection.GetNormalized();

		//Find the length from the center of the sphere, to the projection point on the ray
		T lengtFromCenterToRay = sqrt
		(
			rayOriginToCenter.LengthSqr() - projectionVector.LengthSqr()
		);

		//Is "lengtFromCenterToRay" shorter than the spheres radius?
		if (lengtFromCenterToRay <= aSphere.GetRadius())
		{
			// If true, The ray intersects

			T intersectionDistance = sqrt
			(
				aSphere.GetRadius() * aSphere.GetRadius() 
				- lengtFromCenterToRay * lengtFromCenterToRay
			);

			T rayScalar = projectionVector.Length() - intersectionDistance;

			aOutIntersectionPoint = rayOrigin + (rayScalar * rayDirection);
			return true;

		}
		//No intersection
		return false;

	}

}


