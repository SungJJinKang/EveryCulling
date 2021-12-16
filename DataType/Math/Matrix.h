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

		inline Vec4& operator[](size_t index)
		{
			return mColumns[index];
		}

		inline const Vec4& operator[](size_t index) const
		{
			return mColumns[index];
		}

		inline float* data()
		{
			return mColumns[0].values;
		}

		inline const float* data() const
		{
			return mColumns[0].values;
		}
	};
}
