#pragma once

#include <Vector3.h>

namespace culling
{

	struct AABB
	{
		math::Vector4 mMin;
		math::Vector4 mMax;
	};

}