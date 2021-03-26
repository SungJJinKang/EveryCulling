#pragma once

#include <Quaternion.h>
#include <Vector3.h>

namespace doom
{
	namespace graphics
	{
		struct TransformData
		{
			math::Quaternion rotation;
			math::Vector3 minAABB;
			float pad[1]; // for aligned to 8byte
			math::Vector3 maxAABB;
			math::Vector3 scale;
		};
	}
}