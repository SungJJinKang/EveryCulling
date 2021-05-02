#pragma once

#include <algorithm>
#include <vector>

namespace culling
{
	struct Vector2
	{
		float x, y;
	};

	struct Vector3
	{
		float x, y, z;
	};

	struct TwoDTriangle
	{
		Vector2 Point1;
		Vector2 Point2;
		Vector2 Point3;
	};

	struct ThreeDTriangle
	{
		Vector3 Point1;
		Vector3 Point2;
		Vector3 Point3;
	};

	/// <summary>
	/// TriangleList
	/// 
	/// TwoDTriangle should be reserved after initialized!!!!
	/// </summary>
	struct TriangleList
	{
		size_t mMaxTriangleCount;
		size_t mCurrentTriangleIndex;
		TwoDTriangle* mTriangleList;
	};

	/// <summary>
	/// Sort TwoDTriangle Points y ascending.
	/// Point1 is TopMost Vector2
	/// </summary>
	/// <param name="triangle"></param>
	inline void SortTriangle(TwoDTriangle& triangle)
	{
		if (triangle.Point1.y < triangle.Point2.y)
		{
			std::swap(triangle.Point1, triangle.Point2);
		}
		if (triangle.Point1.y < triangle.Point3.y)
		{
			std::swap(triangle.Point1, triangle.Point3);
		}
		if (triangle.Point2.y < triangle.Point3.y)
		{
			std::swap(triangle.Point2, triangle.Point3);
		}
	}
}