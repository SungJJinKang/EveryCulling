#pragma once	

#include "../../EveryCullingCore.h"

namespace culling
{
	struct Vec2
	{
		float x, y;

		FORCE_INLINE Vec2() {}
		FORCE_INLINE Vec2(const float _x, const float _y)
			:x(_x), y(_y)
		{

		}

		FORCE_INLINE float* data()
		{
			return &x;
		}

		FORCE_INLINE const float* data() const
		{
			return &x;
		}
	};

	struct Vec3
	{
		float x, y, z;

		FORCE_INLINE Vec3() {}
		FORCE_INLINE Vec3(const float _x, const float _y, const float _z)
			:x(_x), y(_y), z(_z)
		{

		}

		FORCE_INLINE float* data()
		{
			return &x;
		}

		FORCE_INLINE const float* data() const
		{
			return &x;
		}
	};

	FORCE_INLINE culling::Vec2 operator+(const culling::Vec2& a, const culling::Vec2& b)
	{
		return culling::Vec2(a.x + b.x, a.y + b.y);
	}

	FORCE_INLINE culling::Vec2 operator-(const culling::Vec2& a, const culling::Vec2& b)
	{
		return culling::Vec2(a.x - b.x, a.y - b.y);

	}

	FORCE_INLINE culling::Vec3 operator+(const culling::Vec3& a, const culling::Vec3& b)
	{
		return culling::Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	FORCE_INLINE culling::Vec3 operator-(const culling::Vec3& a, const culling::Vec3& b)
	{
		return culling::Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	FORCE_INLINE float Dot(const culling::Vec2& a, const culling::Vec2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	FORCE_INLINE float Dot(const culling::Vec3& a, const culling::Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	FORCE_INLINE float PerpDot(const culling::Vec2& lhs, const culling::Vec2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	FORCE_INLINE culling::Vec3 Cross(const culling::Vec3& lhs, const culling::Vec3& rhs)
	{
		return culling::Vec3(
			lhs.y * rhs.z - rhs.y * lhs.z,
			lhs.z * rhs.x - rhs.z * lhs.x,
			lhs.x * rhs.y - rhs.x * lhs.y);
	}


	/// <summary>
	/// Why alignas(16)? : FOR SIMD!!
	/// </summary>
	struct alignas(16) Vec4
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

		FORCE_INLINE float* data()
		{
			return values;
		}

		FORCE_INLINE const float* data() const
		{
			return values;
		}
	};

	struct alignas(16) Quat
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

		FORCE_INLINE float* data()
		{
			return values;
		}

		FORCE_INLINE const float* data() const
		{
			return values;
		}
	};
}