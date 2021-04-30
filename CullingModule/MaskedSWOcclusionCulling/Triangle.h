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

	struct Triangle
	{
		Vector3 Point1;
		Vector3 Point2;
		Vector3 Point3;
	};

	/// <summary>
	/// TriangleList
	/// 
	/// Triangle should be reserved after initialized!!!!
	/// </summary>
	struct TriangleList
	{
		size_t mMaxTriangleCount;
		size_t mCurrentTriangleIndex;
		Triangle* mTriangleList;
	};
}