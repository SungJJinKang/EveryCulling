#pragma once
#include "../../EveryCullingCore.h"

#if defined(__AVX2__)

#include <immintrin.h>

namespace culling
{


#if defined(__GNUC__)  || defined( __clang__)

	union EVERYCULLING_UNION_M128F {
		__m128 raw;    // SSE 4 x float vector
		float m128_f32[4];  // scalar array of 4 floats

		EVERYCULLING_FORCE_INLINE operator __m128() { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m128() const { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m128* () { return &raw; }
		EVERYCULLING_FORCE_INLINE operator const __m128* () const { return &raw; }
		EVERYCULLING_FORCE_INLINE __m128* operator& () { return &raw; }
		EVERYCULLING_FORCE_INLINE const __m128* operator& () const { return &raw; }

		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M128F() {}
		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M128F(const __m128& _raw) : raw(_raw) {}
	};

	using EVERYCULLING_M128F = EVERYCULLING_UNION_M128F;

	union EVERYCULLING_UNION_M128D {
		__m128d raw;    // SSE 4 x float vector
		double m128_d32[2];  // scalar array of 4 floats

		EVERYCULLING_FORCE_INLINE operator __m128d() { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m128d() const { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m128d* () { return &raw; }
		EVERYCULLING_FORCE_INLINE operator const __m128d* () const { return &raw; }
		EVERYCULLING_FORCE_INLINE __m128d* operator& () { return &raw; }
		EVERYCULLING_FORCE_INLINE const __m128d* operator& () const { return &raw; }

		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M128D() {}
		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M128D(const __m128d& _raw) : raw(_raw) {}
	};

	using EVERYCULLING_M128D = EVERYCULLING_UNION_M128D;

	union EVERYCULLING_UNION_M128I {
		__m128i raw;    // SSE 4 x float vector
		int m128_i32[4];  // scalar array of 4 floats

		EVERYCULLING_FORCE_INLINE operator __m128i() { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m128i() const { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m128i* () { return &raw; }
		EVERYCULLING_FORCE_INLINE operator const __m128i* () const { return &raw; }
		EVERYCULLING_FORCE_INLINE __m128i* operator& () { return &raw; }
		EVERYCULLING_FORCE_INLINE const __m128i* operator& () const { return &raw; }

		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M128I() {}
		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M128I(const __m128i& _raw) : raw(_raw) {}
	};

	using EVERYCULLING_M128I = EVERYCULLING_UNION_M128I;

	union EVERYCULLING_UNION_M256F {
		__m256 raw;    // SSE 4 x float vector
		float m128_f32[8];  // scalar array of 4 floats

		EVERYCULLING_FORCE_INLINE operator __m256() { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m256() const { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m256* () { return &raw; }
		EVERYCULLING_FORCE_INLINE operator const __m256* () const { return &raw; }
		EVERYCULLING_FORCE_INLINE __m256* operator& () { return &raw; }
		EVERYCULLING_FORCE_INLINE const __m256* operator& () const { return &raw; }

		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M256F() {}
		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M256F(const __m256& _raw) : raw(_raw) {}
	};

	using EVERYCULLING_M256F = EVERYCULLING_UNION_M256F;

	union EVERYCULLING_UNION_M256D {
		__m256d raw;    // SSE 4 x float vector
		double m128_d32[4];  // scalar array of 4 floats

		EVERYCULLING_FORCE_INLINE operator __m256d() { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m256d() const { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m256d* () { return &raw; }
		EVERYCULLING_FORCE_INLINE operator const __m256d* () const { return &raw; }
		EVERYCULLING_FORCE_INLINE __m256d* operator& () { return &raw; }
		EVERYCULLING_FORCE_INLINE const __m256d* operator& () const { return &raw; }

		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M256D() {}
		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M256D(const __m256d& _raw) : raw(_raw) {}
	};

	using EVERYCULLING_M256D = EVERYCULLING_UNION_M256D;

	union EVERYCULLING_UNION_M256I {
		__m256i raw;    // SSE 4 x float vector
		int m128_i32[8];  // scalar array of 4 floats

		EVERYCULLING_FORCE_INLINE operator __m256i() { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m256i() const { return raw; }
		EVERYCULLING_FORCE_INLINE operator __m256i* () { return &raw; }
		EVERYCULLING_FORCE_INLINE operator const __m256i* () const { return &raw; }
		EVERYCULLING_FORCE_INLINE __m256i* operator& () { return &raw; }
		EVERYCULLING_FORCE_INLINE const __m256i* operator& () const { return &raw; }

		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M256I() {}
		EVERYCULLING_FORCE_INLINE EVERYCULLING_UNION_M256I(const __m256i& _raw) : raw(_raw) {}
	};

	using EVERYCULLING_M256I = EVERYCULLING_UNION_M256I;


#elif defined(_MSC_VER)

	using EVERYCULLING_M128F = __m128;
	using EVERYCULLING_M128D = __m128d;
	using EVERYCULLING_M128I = __m128i;



	using EVERYCULLING_M256F = __m256;
	using EVERYCULLING_M256D = __m256d;
	using EVERYCULLING_M256I = __m256i;

#endif



	/*
	#ifndef culling::EVERYCULLING_M128F
	#define culling::EVERYCULLING_M128F(VECTOR4FLOAT) *reinterpret_cast<culling::EVERYCULLING_M128F*>(&VECTOR4FLOAT)
	#endif

	#ifndef EVERYCULLING_M128D
	#define EVERYCULLING_M128D(VECTOR4DOUBLE) *reinterpret_cast<EVERYCULLING_M128D*>(&VECTOR4DOUBLE)
	#endif

	#ifndef EVERYCULLING_M128I
	#define EVERYCULLING_M128I(VECTOR4INT) *reinterpret_cast<EVERYCULLING_M128I*>(&VECTOR4INT)
	#endif
	*/

	//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=69,124,3928,5197&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&text=_mm_shuffle_ps

#ifndef EVERYCULLING_SHUFFLEMASK
#define EVERYCULLING_SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )
#endif

#ifndef EVERYCULLING_M128F_REPLICATE
#define EVERYCULLING_M128F_REPLICATE(_M128F, ElementIndex) _mm_permute_ps(_M128F, EVERYCULLING_SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 
#endif

#ifndef EVERYCULLING_M128F_SWIZZLE
#define EVERYCULLING_M128F_SWIZZLE(_M128F, X, Y, Z, W) _mm_permute_ps(_M128F, EVERYCULLING_SHUFFLEMASK(X, Y, Z, W)) 
#endif

#ifndef EVERYCULLING_M256F_REPLICATE
#define EVERYCULLING_M256F_REPLICATE(_M256F, ElementIndex) _mm256_permute_ps(_M256F, EVERYCULLING_SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 
#endif

#ifndef EVERYCULLING_M256F_SWIZZLE
#define EVERYCULLING_M256F_SWIZZLE(_M256F, X, Y, Z, W) _mm256_permute_ps(_M256F, EVERYCULLING_SHUFFLEMASK(X, Y, Z, W)) 
#endif

	extern const culling::EVERYCULLING_M128F M128F_Zero;
	extern const culling::EVERYCULLING_M128F M128F_HALF_ONE;
	extern const culling::EVERYCULLING_M128F M128F_EVERY_BITS_ONE;

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M128F EVERYCULLING_M128F_ADD(const culling::EVERYCULLING_M128F& M128_A, const culling::EVERYCULLING_M128F& M128_B)
	{
		return _mm_add_ps(M128_A, M128_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_ADD(const culling::EVERYCULLING_M256F& M256_A, const culling::EVERYCULLING_M256F& M256_B)
	{
		return _mm256_add_ps(M256_A, M256_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M128F EVERYCULLING_M128F_SUB(const culling::EVERYCULLING_M128F& M128_A, const culling::EVERYCULLING_M128F& M128_B)
	{
		return _mm_sub_ps(M128_A, M128_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_SUB(const culling::EVERYCULLING_M256F& M256_A, const culling::EVERYCULLING_M256F& M256_B)
	{
		return _mm256_sub_ps(M256_A, M256_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M128F EVERYCULLING_M128F_MUL(const culling::EVERYCULLING_M128F& M128_A, const culling::EVERYCULLING_M128F& M128_B)
	{
		return _mm_mul_ps(M128_A, M128_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_MUL(const culling::EVERYCULLING_M256F& M256_A, const culling::EVERYCULLING_M256F& M256_B)
	{
		return _mm256_mul_ps(M256_A, M256_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M128F EVERYCULLING_M128F_DIV(const culling::EVERYCULLING_M128F& M128_A, const culling::EVERYCULLING_M128F& M128_B)
	{
		return _mm_div_ps(M128_A, M128_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_DIV(const culling::EVERYCULLING_M256F& M256_A, const culling::EVERYCULLING_M256F& M256_B)
	{
		return _mm256_div_ps(M256_A, M256_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M128F EVERYCULLING_M128F_MUL_AND_ADD(const culling::EVERYCULLING_M128F& M128_A, const culling::EVERYCULLING_M128F& M128_B, const culling::EVERYCULLING_M128F& M128_C)
	{
		return culling::EVERYCULLING_M128F_ADD(culling::EVERYCULLING_M128F_MUL(M128_A, M128_B), M128_C);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_MUL_AND_ADD(const culling::EVERYCULLING_M256F& M256_A, const culling::EVERYCULLING_M256F& M256_B, const culling::EVERYCULLING_M256F& M256_C)
	{
		return _mm256_fmadd_ps(M256_A, M256_B, M256_C);
	}

	/*EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M128F EVERYCULLING_M128F_CROSS(const culling::EVERYCULLING_M128F& M128_A, const culling::EVERYCULLING_M128F& M128_B)
	{
		culling::EVERYCULLING_M128F A_YZXW = _mm_shuffle_ps(M128_A.raw, M128_A.raw, EVERYCULLING_SHUFFLEMASK(1, 2, 0, 3));
		culling::EVERYCULLING_M128F B_ZXYW = _mm_shuffle_ps(M128_B, M128_B, EVERYCULLING_SHUFFLEMASK(2, 0, 1, 3));
		culling::EVERYCULLING_M128F A_ZXYW = _mm_shuffle_ps(M128_A, M128_A, EVERYCULLING_SHUFFLEMASK(2, 0, 1, 3));
		culling::EVERYCULLING_M128F B_YZXW = _mm_shuffle_ps(M128_B, M128_B, EVERYCULLING_SHUFFLEMASK(1, 2, 0, 3));
		return EVERYCULLING_M128F_SUB(EVERYCULLING_M128F_MUL(A_YZXW, B_ZXYW), EVERYCULLING_M128F_MUL(A_ZXYW, B_YZXW));
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_CROSS(const culling::EVERYCULLING_M256F& M256_A, const culling::EVERYCULLING_M256F& M256_B)
	{
		culling::EVERYCULLING_M256F A_YZXW = _mm256_shuffle_ps(M256_A, M256_A, EVERYCULLING_SHUFFLEMASK(1, 2, 0, 3));
		culling::EVERYCULLING_M256F B_ZXYW = _mm256_shuffle_ps(M256_B, M256_B, EVERYCULLING_SHUFFLEMASK(2, 0, 1, 3));
		culling::EVERYCULLING_M256F A_ZXYW = _mm256_shuffle_ps(M256_A, M256_A, EVERYCULLING_SHUFFLEMASK(2, 0, 1, 3));
		culling::EVERYCULLING_M256F B_YZXW = _mm256_shuffle_ps(M256_B, M256_B, EVERYCULLING_SHUFFLEMASK(1, 2, 0, 3));
		return EVERYCULLING_M256F_SUB(EVERYCULLING_M256F_MUL(A_YZXW, B_ZXYW), EVERYCULLING_M256F_MUL(A_ZXYW, B_YZXW));
	}*/


	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_SELECT(const culling::EVERYCULLING_M256F& M256_A, const culling::EVERYCULLING_M256F& M256_B, const culling::EVERYCULLING_M256F& MASK)
	{
		return _mm256_blendv_ps(M256_A, M256_B, MASK);
	}

	EVERYCULLING_FORCE_INLINE void EVERYCULLING_M256F_SWAP(culling::EVERYCULLING_M256F& M256_A, culling::EVERYCULLING_M256F& M256_B, const culling::EVERYCULLING_M256F& MASK)
	{
		const culling::EVERYCULLING_M256F TEMP_A = M256_A;
		const culling::EVERYCULLING_M256F TEMP_B = M256_B;
		M256_A = culling::EVERYCULLING_M256F_SELECT(TEMP_A, TEMP_B, MASK);
		M256_B = culling::EVERYCULLING_M256F_SELECT(TEMP_B, TEMP_A, MASK);
	}

	/*
	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_MIN(culling::EVERYCULLING_M256F& M256_A, culling::EVERYCULLING_M256F& M256_B)
	{
		return _mm256_min_ps(M256_A, M256_B);
	}

	EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F EVERYCULLING_M256F_MAX(culling::EVERYCULLING_M256F& M256_A, culling::EVERYCULLING_M256F& M256_B)
	{
		return _mm256_max_ps(M256_A, M256_B);
	}
	*/
	
}

#else

#error "Please enable AVX2"

#endif


