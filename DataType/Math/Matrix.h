#pragma once

#include "Vector.h"

namespace culling
{
	/// <summary>
	/// Column major Matrix 4X4
	/// </summary>
	struct alignas(32) Matrix4X4
	{
		culling::Vector4 mColumns[4];

		FORCE_INLINE Vector4& operator[](size_t index)
		{
			return mColumns[index];
		}

		FORCE_INLINE const Vector4& operator[](size_t index) const
		{
			return mColumns[index];
		}
	};
}
