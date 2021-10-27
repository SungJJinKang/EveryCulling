#pragma once	

namespace culling
{
	struct Vec2
	{
		float x, y;

		inline Vec2() {}
		inline Vec2(const float _x, const float _y)
			:x(_x), y(_y)
		{

		}
	};

	struct Vec3
	{
		float x, y, z;

		inline Vec3() {}
		inline Vec3(const float _x, const float _y, const float _z)
			:x(_x), y(_y), z(_z)
		{

		}

		
	};

	inline culling::Vec2 operator+(const culling::Vec2& a, const culling::Vec2& b)
	{
		return culling::Vec2(a.x + b.x, a.y + b.y);
	}

	inline culling::Vec2 operator-(const culling::Vec2& a, const culling::Vec2& b)
	{
		return culling::Vec2(a.x - b.x, a.y - b.y);

	}

	inline culling::Vec3 operator+(const culling::Vec3& a, const culling::Vec3& b)
	{
		return culling::Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	inline culling::Vec3 operator-(const culling::Vec3& a, const culling::Vec3& b)
	{
		return culling::Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	inline float Dot(const culling::Vec2& a, const culling::Vec2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline float Dot(const culling::Vec3& a, const culling::Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float PerpDot(const culling::Vec2& lhs, const culling::Vec2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	inline culling::Vec3 Cross(const culling::Vec3& lhs, const culling::Vec3& rhs)
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

		inline float& operator[](size_t index)
		{
			return values[index];
		}

		inline float operator[](size_t index) const
		{
			return values[index];
		}
	};

	struct alignas(16) Quat
	{
		float values[4];

		inline float& operator[](size_t index)
		{
			return values[index];
		}

		inline float operator[](size_t index) const
		{
			return values[index];
		}
	};
}