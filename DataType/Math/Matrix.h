#pragma once

#include "Vector.h"

namespace culling
{
	/// <summary>
	/// Column major Matrix 4X4
	/// </summary>
	struct alignas(32) Mat4x4
	{
		culling::Vec4 mColumns[4];

		EVERYCULLING_FORCE_INLINE Vec4& operator[](size_t index)
		{
			return mColumns[index];
		}

		EVERYCULLING_FORCE_INLINE const Vec4& operator[](size_t index) const
		{
			return mColumns[index];
		}

		EVERYCULLING_FORCE_INLINE float* data()
		{
			return mColumns[0].values;
		}

		EVERYCULLING_FORCE_INLINE const float* data() const
		{
			return mColumns[0].values;
		}
	};
}
