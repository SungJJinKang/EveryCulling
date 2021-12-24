#pragma once
// references :
// https://stackoverflow.com/questions/66743623/what-is-difference-between-m128a-and-m128a?noredirect=1#comment117984269_66743623
// https://stackoverflow.com/questions/52112605/is-reinterpret-casting-between-hardware-simd-vector-pointer-and-the-correspond
// https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
// Unreal Engine Source Code  
//

#include "SIMD.h"

#ifdef SIMD_ENABLED



#include <immintrin.h>

namespace culling
{


#if defined(__GNUC__)  || defined( __clang__)

	union __M128F {
		__m128 raw;    // SSE 4 x float vector
		float m128_f32[4];  // scalar array of 4 floats

		FORCE_INLINE operator __m128() { return raw; }
		FORCE_INLINE operator __m128() const { return raw; }
		FORCE_INLINE operator __m128* () { return &raw; }
		FORCE_INLINE operator const __m128* () const { return &raw; }
		FORCE_INLINE __m128* operator& () { return &raw; }
		FORCE_INLINE const __m128* operator& () const { return &raw; }

		FORCE_INLINE __M128F() {}
		FORCE_INLINE __M128F(const __m128& _raw) : raw(_raw) {}
	};

	using M128F = __M128F;

	union __M128D {
		__m128d raw;    // SSE 4 x float vector
		double m128_d32[2];  // scalar array of 4 floats

		FORCE_INLINE operator __m128d() { return raw; }
		FORCE_INLINE operator __m128d() const { return raw; }
		FORCE_INLINE operator __m128d* () { return &raw; }
		FORCE_INLINE operator const __m128d* () const { return &raw; }
		FORCE_INLINE __m128d* operator& () { return &raw; }
		FORCE_INLINE const __m128d* operator& () const { return &raw; }

		FORCE_INLINE __M128D() {}
		FORCE_INLINE __M128D(const __m128d& _raw) : raw(_raw) {}
	};

	using M128D = __M128D;

	union __M128I {
		__m128i raw;    // SSE 4 x float vector
		INT32 m128_i32[4];  // scalar array of 4 floats

		FORCE_INLINE operator __m128i() { return raw; }
		FORCE_INLINE operator __m128i() const { return raw; }
		FORCE_INLINE operator __m128i* () { return &raw; }
		FORCE_INLINE operator const __m128i* () const { return &raw; }
		FORCE_INLINE __m128i* operator& () { return &raw; }
		FORCE_INLINE const __m128i* operator& () const { return &raw; }

		FORCE_INLINE __M128I() {}
		FORCE_INLINE __M128I(const __m128i& _raw) : raw(_raw) {}
	};

	using M128I = __M128I;

	union __M256F {
		__m256 raw;    // SSE 4 x float vector
		float m128_f32[8];  // scalar array of 4 floats

		FORCE_INLINE operator __m256() { return raw; }
		FORCE_INLINE operator __m256() const { return raw; }
		FORCE_INLINE operator __m256* () { return &raw; }
		FORCE_INLINE operator const __m256* () const { return &raw; }
		FORCE_INLINE __m256* operator& () { return &raw; }
		FORCE_INLINE const __m256* operator& () const { return &raw; }

		FORCE_INLINE __M256F() {}
		FORCE_INLINE __M256F(const __m256& _raw) : raw(_raw) {}
	};

	using M256F = __M256F;

	union __M256D {
		__m256d raw;    // SSE 4 x float vector
		double m128_d32[4];  // scalar array of 4 floats

		FORCE_INLINE operator __m256d() { return raw; }
		FORCE_INLINE operator __m256d() const { return raw; }
		FORCE_INLINE operator __m256d* () { return &raw; }
		FORCE_INLINE operator const __m256d* () const { return &raw; }
		FORCE_INLINE __m256d* operator& () { return &raw; }
		FORCE_INLINE const __m256d* operator& () const { return &raw; }

		FORCE_INLINE __M256D() {}
		FORCE_INLINE __M256D(const __m256d& _raw) : raw(_raw) {}
	};

	using M256D = __M256D;

	union __M256I {
		__m256i raw;    // SSE 4 x float vector
		INT32 m128_i32[8];  // scalar array of 4 floats

		FORCE_INLINE operator __m256i() { return raw; }
		FORCE_INLINE operator __m256i() const { return raw; }
		FORCE_INLINE operator __m256i* () { return &raw; }
		FORCE_INLINE operator const __m256i* () const { return &raw; }
		FORCE_INLINE __m256i* operator& () { return &raw; }
		FORCE_INLINE const __m256i* operator& () const { return &raw; }

		FORCE_INLINE __M256I() {}
		FORCE_INLINE __M256I(const __m256i& _raw) : raw(_raw) {}
	};

	using M256I = __M256I;


#elif defined(_MSC_VER)

	using M128F = __m128;
	using M128D = __m128d;
	using M128I = __m128i;



	using M256F = __m256;
	using M256D = __m256d;
	using M256I = __m256i;

#endif



	/*
	#ifndef culling::M128F
	#define culling::M128F(VECTOR4FLOAT) *reinterpret_cast<culling::M128F*>(&VECTOR4FLOAT)
	#endif

	#ifndef M128D
	#define M128D(VECTOR4DOUBLE) *reinterpret_cast<M128D*>(&VECTOR4DOUBLE)
	#endif

	#ifndef M128I
	#define M128I(VECTOR4INT) *reinterpret_cast<M128I*>(&VECTOR4INT)
	#endif
	*/

	//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=69,124,3928,5197&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&text=_mm_shuffle_ps

#ifndef SHUFFLEMASK
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )
#endif

#ifndef M128F_REPLICATE
#define M128F_REPLICATE(_M128F, ElementIndex) _mm_permute_ps(_M128F, SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 
#endif

#ifndef M128F_SWIZZLE
#define M128F_SWIZZLE(_M128F, X, Y, Z, W) _mm_permute_ps(_M128F, SHUFFLEMASK(X, Y, Z, W)) 
#endif

#ifndef M256F_REPLICATE
#define M256F_REPLICATE(_M256F, ElementIndex) _mm256_permute_ps(_M256F, SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 
#endif

#ifndef M256F_SWIZZLE
#define M256F_SWIZZLE(_M256F, X, Y, Z, W) _mm256_permute_ps(_M256F, SHUFFLEMASK(X, Y, Z, W)) 
#endif

	inline const culling::M128F M128F_Zero{ _mm_castsi128_ps(_mm_set1_epi16(0)) };
	inline const culling::M128F M128F_HALF_ONE{ _mm_set1_ps(0.5f) };
	inline const culling::M128F M128F_EVERY_BITS_ONE{ _mm_castsi128_ps(_mm_set1_epi16(-1)) };

	FORCE_INLINE extern culling::M128F M128F_ADD(const culling::M128F& M128_A, const culling::M128F& M128_B)
	{
		return _mm_add_ps(M128_A, M128_B);
	}

	FORCE_INLINE extern culling::M256F M256F_ADD(const culling::M256F& M256_A, const culling::M256F& M256_B)
	{
		return _mm256_add_ps(M256_A, M256_B);
	}

	FORCE_INLINE extern culling::M128F M128F_SUB(const culling::M128F& M128_A, const culling::M128F& M128_B)
	{
		return _mm_sub_ps(M128_A, M128_B);
	}

	FORCE_INLINE extern culling::M256F M256F_SUB(const culling::M256F& M256_A, const culling::M256F& M256_B)
	{
		return _mm256_sub_ps(M256_A, M256_B);
	}

	FORCE_INLINE extern culling::M128F M128F_MUL(const culling::M128F& M128_A, const culling::M128F& M128_B)
	{
		return _mm_mul_ps(M128_A, M128_B);
	}

	FORCE_INLINE extern culling::M256F M256F_MUL(const culling::M256F& M256_A, const culling::M256F& M256_B)
	{
		return _mm256_mul_ps(M256_A, M256_B);
	}

	FORCE_INLINE extern culling::M128F M128F_DIV(const culling::M128F& M128_A, const culling::M128F& M128_B)
	{
		return _mm_div_ps(M128_A, M128_B);
	}

	FORCE_INLINE extern culling::M256F M256F_DIV(const culling::M256F& M256_A, const culling::M256F& M256_B)
	{
		return _mm256_div_ps(M256_A, M256_B);
	}

	FORCE_INLINE extern culling::M128F M128F_MUL_AND_ADD(const culling::M128F& M128_A, const culling::M128F& M128_B, const culling::M128F& M128_C)
	{
		return culling::M128F_ADD(culling::M128F_MUL(M128_A, M128_B), M128_C);
	}

	FORCE_INLINE extern culling::M256F M256F_MUL_AND_ADD(const culling::M256F& M256_A, const culling::M256F& M256_B, const culling::M256F& M256_C)
	{
		return culling::M256F_ADD(culling::M256F_MUL(M256_A, M256_B), M256_C);
	}

	/*FORCE_INLINE extern culling::M128F M128F_CROSS(const culling::M128F& M128_A, const culling::M128F& M128_B)
	{
		culling::M128F A_YZXW = _mm_shuffle_ps(M128_A.raw, M128_A.raw, SHUFFLEMASK(1, 2, 0, 3));
		culling::M128F B_ZXYW = _mm_shuffle_ps(M128_B, M128_B, SHUFFLEMASK(2, 0, 1, 3));
		culling::M128F A_ZXYW = _mm_shuffle_ps(M128_A, M128_A, SHUFFLEMASK(2, 0, 1, 3));
		culling::M128F B_YZXW = _mm_shuffle_ps(M128_B, M128_B, SHUFFLEMASK(1, 2, 0, 3));
		return M128F_SUB(M128F_MUL(A_YZXW, B_ZXYW), M128F_MUL(A_ZXYW, B_YZXW));
	}

	FORCE_INLINE extern culling::M256F M256F_CROSS(const culling::M256F& M256_A, const culling::M256F& M256_B)
	{
		culling::M256F A_YZXW = _mm256_shuffle_ps(M256_A, M256_A, SHUFFLEMASK(1, 2, 0, 3));
		culling::M256F B_ZXYW = _mm256_shuffle_ps(M256_B, M256_B, SHUFFLEMASK(2, 0, 1, 3));
		culling::M256F A_ZXYW = _mm256_shuffle_ps(M256_A, M256_A, SHUFFLEMASK(2, 0, 1, 3));
		culling::M256F B_YZXW = _mm256_shuffle_ps(M256_B, M256_B, SHUFFLEMASK(1, 2, 0, 3));
		return M256F_SUB(M256F_MUL(A_YZXW, B_ZXYW), M256F_MUL(A_ZXYW, B_YZXW));
	}*/


	FORCE_INLINE extern culling::M256F M256F_SELECT(const culling::M256F& M256_A, const culling::M256F& M256_B, const culling::M256F& MASK)
	{
		return _mm256_blendv_ps(M256_B, M256_A, MASK);
	}

	FORCE_INLINE extern void M256F_SWAP(culling::M256F& M256_A, culling::M256F& M256_B, const culling::M256F& MASK)
	{
		culling::M256F TEMP = M256_A;
		M256_A = culling::M256F_SELECT(M256_A, M256_B, MASK);
		M256_B = culling::M256F_SELECT(M256_A, TEMP, MASK);
	}

	/*
	FORCE_INLINE extern culling::M256F M256F_MIN(culling::M256F& M256_A, culling::M256F& M256_B)
	{
		return _mm256_min_ps(M256_A, M256_B);
	}

	FORCE_INLINE extern culling::M256F M256F_MAX(culling::M256F& M256_A, culling::M256F& M256_B)
	{
		return _mm256_max_ps(M256_A, M256_B);
	}
	*/
	
}





#endif


