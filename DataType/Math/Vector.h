#pragma once

#include "../../EveryCullingCore.h"

namespace culling
{
	struct Vector2
	{
		float x, y;

		FORCE_INLINE Vector2() {};
		FORCE_INLINE Vector2(const float _x, const float _y)
			:x(_x), y(_y)
		{

		}
	};

	struct Vector3
	{
		float x, y, z;

		FORCE_INLINE Vector3() {};
		FORCE_INLINE Vector3(const float _x, const float _y, const float _z)
			:x(_x), y(_y), z(_z)
		{

		}

		
	};

	FORCE_INLINE culling::Vector2 operator+(const culling::Vector2& a, const culling::Vector2& b)
	{
		return culling::Vector2(a.x + b.x, a.y + b.y);
	}

	FORCE_INLINE culling::Vector2 operator-(const culling::Vector2& a, const culling::Vector2& b)
	{
		return culling::Vector2(a.x - b.x, a.y - b.y);

	}

	FORCE_INLINE culling::Vector3 operator+(const culling::Vector3& a, const culling::Vector3& b)
	{
		return culling::Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	FORCE_INLINE culling::Vector3 operator-(const culling::Vector3& a, const culling::Vector3& b)
	{
		return culling::Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	FORCE_INLINE float Dot(const culling::Vector2& a, const culling::Vector2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	FORCE_INLINE float Dot(const culling::Vector3& a, const culling::Vector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	FORCE_INLINE float PerpDot(const culling::Vector2& lhs, const culling::Vector2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	FORCE_INLINE culling::Vector3 Cross(const culling::Vector3& lhs, const culling::Vector3& rhs)
	{
		return culling::Vector3(
			lhs.y * rhs.z - rhs.y * lhs.z,
			lhs.z * rhs.x - rhs.z * lhs.x,
			lhs.x * rhs.y - rhs.x * lhs.y);
	}


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