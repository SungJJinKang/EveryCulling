#pragma once

#include "Math/Vector.h"

namespace culling
{
	/// <summary>
	/// this datas isn't used for frustum culling.
	/// some culling will use this data after frustum culling
	/// 
	/// 14 * 4 byte
	/// </summary>
	struct TransformData
	{
		Quat mRotation; // 16byte
		Vec3 mMinAABBPoint; // 12 byte
		Vec3 mMaxAABBPoint; // 12 byte
		Vec3 mScale; // 12 byte
	};
}
