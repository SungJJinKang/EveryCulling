#pragma once	

namespace culling
{
	struct Vec2
	{
		FLOAT32 x, y;

		inline Vec2() {}
		inline Vec2(const FLOAT32 _x, const FLOAT32 _y)
			:x(_x), y(_y)
		{

		}
	};

	struct Vec3
	{
		FLOAT32 x, y, z;

		inline Vec3() {}
		inline Vec3(const FLOAT32 _x, const FLOAT32 _y, const FLOAT32 _z)
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

	inline FLOAT32 Dot(const culling::Vec2& a, const culling::Vec2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline FLOAT32 Dot(const culling::Vec3& a, const culling::Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline FLOAT32 PerpDot(const culling::Vec2& lhs, const culling::Vec2& rhs)
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
		FLOAT32 values[4];

		inline FLOAT32& operator[](SIZE_T index)
		{
			return values[index];
		}

		inline FLOAT32 operator[](SIZE_T index) const
		{
			return values[index];
		}
	};

	struct alignas(16) Quat
	{
		FLOAT32 values[4];

		inline FLOAT32& operator[](SIZE_T index)
		{
			return values[index];
		}

		inline FLOAT32 operator[](SIZE_T index) const
		{
			return values[index];
		}
	};
}