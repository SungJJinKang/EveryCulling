#pragma once

#include <Quaternion.h>
#include <Vector3.h>

namespace doom
{
	namespace graphics
	{
		/// <summary>
		/// this datas isn't used for frustum culling.
		/// some culling will use this data after frustum culling
		/// 
		/// 14 * 4 byte
		/// </summary>
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