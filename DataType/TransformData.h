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
	struct alignas(64) TransformData
	{
		Quaternion mRotation; // 16byte
		Vector3 mMinAABBPoint; // 12 byte
		Vector3 mMaxAABBPoint; // 12 byte
		Vector3 mScale; // 12 byte
	};
}
