#pragma once

#include "../../EveryCullingCore.h"

namespace culling
{
	struct Vector2
	{
		float x, y;
	};

	struct Vector3
	{
		float x, y, z;
	};

	/// <summary>
	/// Why alignas(16)? : FOR SIMD!!
	/// </summary>
	struct alignas(16) Vector4
	{
		float values[4];

		FORCE_INLINE float& operator[](size_t index)
		{
			return values[index];
		}

		FORCE_INLINE float operator[](size_t index) const
		{
			return values[index];
		}
	};

	struct alignas(16) Quaternion
	{
		float values[4];

		FORCE_INLINE float& operator[](size_t index)
		{
			return values[index];
		}

		FORCE_INLINE float operator[](size_t index) const
		{
			return values[index];
		}
	};
}