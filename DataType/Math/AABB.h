#pragma once

#include "Vector.h"

namespace culling
{

	struct AABB
	{
		Vector3 mMin;
		Vector3 mMax;
	};

	struct Face
	{
		Vector3 Vertices[4];
	};

	struct AABBVertices
	{
		Face mFaces[6];
	};

	inline constexpr unsigned int AABB_INDEX_DATA[] = {
			0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9,10,10, 9,11,
		   12,13,14,14,13,15,16,17,18,18,17,19,20,21,22,22,21,23,
	};
}