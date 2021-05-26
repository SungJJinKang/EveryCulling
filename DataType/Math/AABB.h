#pragma once

#include "Vector.h"

namespace culling
{

	struct AABB
	{
		Vector3 mMin;
		Vector3 mMax;
	};

	struct AABBPoints
	{
		Vector3 mPoints[8];
	};
}