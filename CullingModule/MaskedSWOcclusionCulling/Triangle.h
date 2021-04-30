#pragma once

#include <vector>

#include <Vector2.h>

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
}