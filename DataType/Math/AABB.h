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

	
}