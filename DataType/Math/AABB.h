#pragma once

#include "Vector.h"

namespace culling
{

	struct AABB
	{
		Vec4 mMin;
		Vec4 mMax;
	};

	struct Face
	{
		Vec3 Vertices[4];
	};

	struct AABBVertices
	{
		Face mFaces[6];
	};

	
}