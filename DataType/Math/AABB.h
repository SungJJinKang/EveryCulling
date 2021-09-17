#pragma once

#include "Vector.h"

namespace culling
{

	struct AABB
	{
		Vector4 mMin;
		Vector4 mMax;
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