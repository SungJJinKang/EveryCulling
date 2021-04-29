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


typedef __m128	M128F;
typedef __m128d M128D;
typedef __m128i M128I;

typedef __m256	M256F;
typedef __m256d M256D;
typedef __m256i M256I;

/*
#ifndef M128F
#define M128F(VECTOR4FLOAT) *reinterpret_cast<M128F*>(&VECTOR4FLOAT)
#endif

#ifndef M128D
#define M128D(VECTOR4DOUBLE) *reinterpret_cast<M128D*>(&VECTOR4DOUBLE)
#endif

#ifndef M128I
#define M128I(VECTOR4INT) *reinterpret_cast<M128I*>(&VECTOR4INT)
#endif
*/

//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=69,124,3928,5197&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&text=_mm_shuffle_ps
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )

#define M128F_REPLICATE(M128F, ElementIndex) _mm_permute_ps(M128F, SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 

#define M128F_SWIZZLE(M128F, X, Y, Z, W) _mm_permute_ps(M128F, SHUFFLEMASK(X, Y, Z, W)) 

#define M256F_REPLICATE(M256F, ElementIndex) _mm256_permute_ps(M256F, SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 

#define M256F_SWIZZLE(M256F, X, Y, Z, W) _mm256_permute_ps(M256F, SHUFFLEMASK(X, Y, Z, W)) 

inline M128F M128F_Zero{ _mm_castsi128_ps(_mm_set1_epi16(0)) };
inline M128F M128F_EVERY_BITS_ONE{ _mm_castsi128_ps(_mm_set1_epi16(-1)) };

FORCE_INLINE M128F M128F_ADD(const M128F& M128_A, const M128F& M128_B)
{
	return _mm_add_ps(M128_A, M128_B);
}

FORCE_INLINE M256F M256F_ADD(const M256F& M256_A, const M256F& M256_B)
{
	return _mm256_add_ps(M256_A, M256_B);
}

FORCE_INLINE M128F M128F_SUB(const M128F& M128_A, const M128F& M128_B)
{
	return _mm_sub_ps(M128_A, M128_B);
}

FORCE_INLINE M256F M256F_SUB(const M256F& M256_A, const M256F& M256_B)
{
	return _mm256_sub_ps(M256_A, M256_B);
}

FORCE_INLINE M128F M128F_MUL(const M128F& M128_A, const M128F& M128_B)
{
	return _mm_mul_ps(M128_A, M128_B);
}

FORCE_INLINE M256F M256F_MUL(const M256F& M256_A, const M256F& M256_B)
{
	return _mm256_mul_ps(M256_A, M256_B);
}

FORCE_INLINE M128F M128F_DIV(const M128F& M128_A, const M128F& M128_B)
{
	return _mm_div_ps(M128_A, M128_B);
}

FORCE_INLINE M256F M256F_DIV(const M256F& M256_A, const M256F& M256_B)
{
	return _mm256_div_ps(M256_A, M256_B);
}

FORCE_INLINE M128F M128F_MUL_AND_ADD(const M128F& M128_A, const M128F& M128_B, const M128F& M128_C)
{
	return M128F_ADD(M128F_MUL(M128_A, M128_B), M128_C);
}

FORCE_INLINE M256F M256F_MUL_AND_ADD(const M256F& M256_A, const M256F& M256_B, const M256F& M256_C)
{
	return M256F_ADD(M256F_MUL(M256_A, M256_B), M256_C);
}

FORCE_INLINE M128F M128F_CROSS(const M128F& M128_A, const M128F& M128_B)
{
	M128F A_YZXW = _mm_shuffle_ps(M128_A, M128_A, SHUFFLEMASK(1, 2, 0, 3));
	M128F B_ZXYW = _mm_shuffle_ps(M128_B, M128_B, SHUFFLEMASK(2, 0, 1, 3));
	M128F A_ZXYW = _mm_shuffle_ps(M128_A, M128_A, SHUFFLEMASK(2, 0, 1, 3));
	M128F B_YZXW = _mm_shuffle_ps(M128_B, M128_B, SHUFFLEMASK(1, 2, 0, 3));
	return M128F_SUB(M128F_MUL(A_YZXW, B_ZXYW), M128F_MUL(A_ZXYW, B_YZXW));
}

FORCE_INLINE M256F M256F_CROSS(const M256F& M256_A, const M256F& M256_B)
{
	M256F A_YZXW = _mm256_shuffle_ps(M256_A, M256_A, SHUFFLEMASK(1, 2, 0, 3));
	M256F B_ZXYW = _mm256_shuffle_ps(M256_B, M256_B, SHUFFLEMASK(2, 0, 1, 3));
	M256F A_ZXYW = _mm256_shuffle_ps(M256_A, M256_A, SHUFFLEMASK(2, 0, 1, 3));
	M256F B_YZXW = _mm256_shuffle_ps(M256_B, M256_B, SHUFFLEMASK(1, 2, 0, 3));
	return M256F_SUB(M256F_MUL(A_YZXW, B_ZXYW), M256F_MUL(A_ZXYW, B_YZXW));
}










#endif


