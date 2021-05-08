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


//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=69,124,3928,5197&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&text=_mm_shuffle_ps
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )

#define M128F_REPLICATE(M128F, ElementIndex) _mm_permute_ps(M128F, SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 

#define M128F_SWIZZLE(M128F, X, Y, Z, W) _mm_permute_ps(M128F, SHUFFLEMASK(X, Y, Z, W)) 

#define M256F_REPLICATE(M256F, ElementIndex) _mm256_permute_ps(M256F, SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 

#define M256F_SWIZZLE(M256F, X, Y, Z, W) _mm256_permute_ps(M256F, SHUFFLEMASK(X, Y, Z, W)) 

namespace culling
{

	inline M128F M128F_Zero{ _mm_castsi128_ps(_mm_set1_epi32(0)) };
	inline M128F M128F_EVERY_BITS_ONE{ _mm_castsi128_ps(_mm_set1_epi32(-1)) };

	inline M256F M256F_Zero{ _mm256_castsi256_ps(_mm256_set1_epi32(0)) };
	inline M256F M256F_EVERY_BITS_ONE{ _mm256_castsi256_ps(_mm256_set1_epi32(-1)) };

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
		return culling::M128F_ADD(culling::M128F_MUL(M128_A, M128_B), M128_C);
	}

	FORCE_INLINE M256F M256F_MUL_AND_ADD(const M256F& M256_A, const M256F& M256_B, const M256F& M256_C)
	{
		return culling::M256F_ADD(culling::M256F_MUL(M256_A, M256_B), M256_C);
	}

	FORCE_INLINE M128F M128F_CROSS(const M128F& M128_A, const M128F& M128_B)
	{
		M128F A_YZXW = _mm_shuffle_ps(M128_A, M128_A, SHUFFLEMASK(1, 2, 0, 3));
		M128F B_ZXYW = _mm_shuffle_ps(M128_B, M128_B, SHUFFLEMASK(2, 0, 1, 3));
		M128F A_ZXYW = _mm_shuffle_ps(M128_A, M128_A, SHUFFLEMASK(2, 0, 1, 3));
		M128F B_YZXW = _mm_shuffle_ps(M128_B, M128_B, SHUFFLEMASK(1, 2, 0, 3));
		return culling::M128F_SUB(culling::M128F_MUL(A_YZXW, B_ZXYW), culling::M128F_MUL(A_ZXYW, B_YZXW));
	}

	FORCE_INLINE M256F M256F_CROSS(const M256F& M256_A, const M256F& M256_B)
	{
		M256F A_YZXW = _mm256_shuffle_ps(M256_A, M256_A, SHUFFLEMASK(1, 2, 0, 3));
		M256F B_ZXYW = _mm256_shuffle_ps(M256_B, M256_B, SHUFFLEMASK(2, 0, 1, 3));
		M256F A_ZXYW = _mm256_shuffle_ps(M256_A, M256_A, SHUFFLEMASK(2, 0, 1, 3));
		M256F B_YZXW = _mm256_shuffle_ps(M256_B, M256_B, SHUFFLEMASK(1, 2, 0, 3));
		return culling::M256F_SUB(culling::M256F_MUL(A_YZXW, B_ZXYW), culling::M256F_MUL(A_ZXYW, B_YZXW));
	}


	FORCE_INLINE void M256F_SWAP(M128F& M128_A, M128F& M128_B, const M128F& MASK)
	{
		M128F TEMP = M128_A;
		M128_A = _mm_blendv_ps(M128_A, M128_B, MASK);
		M128_B = _mm_blendv_ps(M128_B, TEMP, MASK);
	}

	/// <summary>
	///
	/// FOR j := 0 to 7
	///		i: = j * 32
	///		IF MASK[i + 31]
	///			dst[i + 31:i] : = M256_B[i + 31:i]
	///		ELSE
	///			dst[i + 31:i] : = M256_A[i + 31:i]
	///		FI
	/// ENDFOR
	/// dst[MAX:256] : = 0
	/// 
	/// </summary>
	/// <param name="M256_A"></param>
	/// <param name="M256_B"></param>
	/// <param name="MASK"></param>
	/// <returns></returns>
	FORCE_INLINE void M256F_SWAP(M256F& M256_A, M256F& M256_B, const M256F& MASK)
	{
		M256F TEMP = M256_A;
		M256_A = _mm256_blendv_ps(M256_A, M256_B, MASK);
		M256_B = _mm256_blendv_ps(M256_B, TEMP, MASK);
	}

}


#endif


