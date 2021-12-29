#pragma once

#include "Common.h"
#include "Vector.h"

#include "SIMD_Core.h"

namespace culling
{
	struct TwoDTriangle
	{
		Vec2 Points[3];
	};

	struct ThreeDTriangle
	{
		Vec3 Points[3];
	};


	/// <summary>
	/// Sort TwoDTriangle Points y ascending.
	/// Point1 is TopMost Vec2
	/// </summary>
	/// <param name="triangle"></param>

	/// <summary>
	/// Sort TwoDTriangle Points y ascending.
	/// Point1 is TopMost Vec2
	/// </summary>
	/// <param name="triangle"></param>
	FORCE_INLINE extern void SortTriangle(TwoDTriangle& triangle)
	{
		if (triangle.Points[0].y < triangle.Points[1].y)
		{
			culling::SWAP(triangle.Points[0], triangle.Points[1]);
		}
		if (triangle.Points[0].y < triangle.Points[2].y)
		{
			culling::SWAP(triangle.Points[0], triangle.Points[2]);
		}
		if (triangle.Points[1].y < triangle.Points[2].y)
		{
			culling::SWAP(triangle.Points[1], triangle.Points[2]);
		}
	}

	FORCE_INLINE extern void SortTriangle(ThreeDTriangle& triangle)
	{
		if (triangle.Points[0].y < triangle.Points[1].y)
		{
			culling::SWAP(triangle.Points[0], triangle.Points[1]);
		}
		if (triangle.Points[0].y < triangle.Points[2].y)
		{
			culling::SWAP(triangle.Points[0], triangle.Points[2]);
		}
		if (triangle.Points[1].y < triangle.Points[2].y)
		{
			culling::SWAP(triangle.Points[1], triangle.Points[2]);
		}
	}

	FORCE_INLINE extern bool IsFrontFaceOfProjectSpaceTriangle(const TwoDTriangle& triangle)
	{
		return PerpDot(triangle.Points[1] - triangle.Points[0], triangle.Points[2] - triangle.Points[0]) > 0;
		/*
		//triangle should be in projection space
		(v1x - v0x)(v2y - v0y) - (v1y - v0y)(v2x - v0x)
		==
		(v1x - v0x) * (v2y - v0y) - (v0x - v2x) * (v0y - v1y)
			// Perform backface test. 
		__mw triArea1 = _mmw_mul_ps(_mmw_sub_ps(pVtxX[1], pVtxX[0]), _mmw_sub_ps(pVtxY[2], pVtxY[0]));
		__mw triArea2 = _mmw_mul_ps(_mmw_sub_ps(pVtxX[0], pVtxX[2]), _mmw_sub_ps(pVtxY[0], pVtxY[1]));
		__mw triArea = _mmw_sub_ps(triArea1, triArea2);
		__mw ccwMask = _mmw_cmpgt_ps(triArea, _mmw_setzero_ps());
		*/
	}


	/// <summary>
	/// 
	/// verticesX[3]
	/// verticesX[0] : Triangle1_Vertex1_X Triangle2_Vertex1_X Triangle3_Vertex1_X Triangle4_Vertex1_X
	/// verticesX[1] : Triangle1_Vertex2_X Triangle2_Vertex2_X Triangle3_Vertex2_X Triangle4_Vertex2_X
	/// verticesX[2] : Triangle1_Vertex3_X Triangle2_Vertex3_X Triangle3_Vertex3_X Triangle4_Vertex3_X
	///
	/// verticesY[3]
	/// verticesY[0] : Triangle1_Vertex1_Y Triangle2_Vertex1_Y Triangle3_Vertex1_Y Triangle4_Vertex1_Y
	/// verticesY[1] : Triangle1_Vertex2_Y Triangle2_Vertex2_Y Triangle3_Vertex2_Y Triangle4_Vertex2_Y
	/// verticesY[2] : Triangle1_Vertex3_Y Triangle2_Vertex3_Y Triangle3_Vertex3_Y Triangle4_Vertex3_Y
	/// 
	/// </summary>
	/// <param name="vertexX"></param>
	/// <param name="vertexY"></param>
	/// <returns></returns>
	FORCE_INLINE extern std::uint32_t TestTrianglesIsFrontFaceUsingSIMD
	(
		const culling::M256F* const verticesX,
		const culling::M256F* const verticesY
	)
	{
		//I don't know How this Works.........
		//https://stackoverflow.com/questions/67357115/i-found-back-face-culling-code-but-i-cant-know-how-this-works
		const culling::M256F triArea1 = culling::M256F_MUL(culling::M256F_SUB(verticesX[1], verticesX[0]), culling::M256F_SUB(verticesY[2], verticesY[0]));
		const culling::M256F triArea2 = culling::M256F_MUL(culling::M256F_SUB(verticesX[0], verticesX[2]), culling::M256F_SUB(verticesY[0], verticesY[1]));
		const culling::M256F triArea = culling::M256F_SUB(triArea1, triArea2);
		
		//_CMP_GT_OQ vs _CMP_GT_OQ : https://stackoverflow.com/questions/16988199/how-to-choose-avx-compare-predicate-variants
		const culling::M256F ccwMask = _mm256_cmp_ps(triArea, _mm256_set1_ps(0.0f), _CMP_GT_OQ);

		//Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in a.
		//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&expand=2156,4979,4979,1731,4929,951,4979,3869&text=movemask
		//if second triangle is front facing, low second bit of triangleCullMask is 1
		return static_cast<std::uint32_t>(_mm256_movemask_ps(ccwMask));
	}

	

	/// <summary>
	/// 
	/// Sort TwoDTriangle Points y ascending.
	/// TriPoint[0] is TopMost
	/// 
	/// TriPoint[0] : First Points Of Triangle
	/// TriPoint[1] : Second Points Of Triangle
	/// TriPoint[2] : Third Points Of Triangle
	/// 
	/// Passed culling::M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX"></param>
	/// <param name="TriPointY"></param>
	FORCE_INLINE extern void Sort_8_2DTriangles(culling::M256F* TriPointX, culling::M256F* TriPointY)
	{
		culling::M256F MASK;

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[1], _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[1], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[1], MASK);

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[2], _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[2], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[2], MASK);

		MASK = _mm256_cmp_ps(TriPointY[1], TriPointY[2], _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointX[1], TriPointX[2], MASK);
		culling::M256F_SWAP(TriPointY[1], TriPointY[2], MASK);
	}

	/// <summary>
	/// 
	/// Sort TwoDTriangle Points y ascending.
	/// TriPoint[0] is TopMost
	/// 
	/// TriPoint[0] : First Points Of Triangle
	/// TriPoint[1] : Second Points Of Triangle
	/// TriPoint[2] : Third Points Of Triangle
	/// 
	/// Passed culling::M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX">3 size array culling::M256F</param>
	/// <param name="TriPointY">3 size array culling::M256F</param>
	/// <param name="TriPointZ">3 size array culling::M256F</param>
	FORCE_INLINE extern void Sort_8_3DTriangles(culling::M256F* TriPointX, culling::M256F* TriPointY, culling::M256F* TriPointZ)
	{
		culling::M256F MASK;

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[1], _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[1], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[1], MASK);
		culling::M256F_SWAP(TriPointZ[0], TriPointZ[1], MASK);

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[2], _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[2], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[2], MASK);
		culling::M256F_SWAP(TriPointZ[0], TriPointZ[2], MASK);

		MASK = _mm256_cmp_ps(TriPointY[1], TriPointY[2], _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointX[1], TriPointX[2], MASK);
		culling::M256F_SWAP(TriPointY[1], TriPointY[2], MASK);
		culling::M256F_SWAP(TriPointZ[1], TriPointZ[2], MASK);
	}

	FORCE_INLINE extern void Sort_8_2DTriangles
	(
		culling::M256F& TriPointA_X,
		culling::M256F& TriPointA_Y,

		culling::M256F& TriPointB_X,
		culling::M256F& TriPointB_Y,

		culling::M256F& TriPointC_X,
		culling::M256F& TriPointC_Y
	)
	{
		culling::M256F MASK;
		
		MASK = _mm256_cmp_ps(TriPointA_Y, TriPointB_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointA_X, TriPointB_X, MASK);
		culling::M256F_SWAP(TriPointA_Y, TriPointB_Y, MASK);

		MASK = _mm256_cmp_ps(TriPointA_Y, TriPointC_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointA_X, TriPointC_X, MASK);
		culling::M256F_SWAP(TriPointA_Y, TriPointC_Y, MASK);

		MASK = _mm256_cmp_ps(TriPointB_Y, TriPointC_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointB_X, TriPointC_X, MASK);
		culling::M256F_SWAP(TriPointB_Y, TriPointC_Y, MASK);
	}

	FORCE_INLINE extern void Sort_8_3DTriangles
	(
		culling::M256F& TriPointA_X,
		culling::M256F& TriPointA_Y,
		culling::M256F& TriPointA_Z,

		culling::M256F& TriPointB_X,
		culling::M256F& TriPointB_Y,
		culling::M256F& TriPointB_Z,

		culling::M256F& TriPointC_X,
		culling::M256F& TriPointC_Y,
		culling::M256F& TriPointC_Z
	)
	{
		culling::M256F MASK;

		MASK = _mm256_cmp_ps(TriPointA_Y, TriPointB_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointA_X, TriPointB_X, MASK);
		culling::M256F_SWAP(TriPointA_Y, TriPointB_Y, MASK);
		culling::M256F_SWAP(TriPointA_Z, TriPointB_Z, MASK);

		MASK = _mm256_cmp_ps(TriPointA_Y, TriPointC_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointA_X, TriPointC_X, MASK);
		culling::M256F_SWAP(TriPointA_Y, TriPointC_Y, MASK);
		culling::M256F_SWAP(TriPointA_Z, TriPointC_Z, MASK);

		MASK = _mm256_cmp_ps(TriPointB_Y, TriPointC_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointB_X, TriPointC_X, MASK);
		culling::M256F_SWAP(TriPointB_Y, TriPointC_Y, MASK);
		culling::M256F_SWAP(TriPointB_Z, TriPointC_Z, MASK);
	}

	FORCE_INLINE extern void Sort_8_3DTriangles
	(
		culling::M256F& TriPointA_X,
		culling::M256F& TriPointA_Y,
		culling::M256F& TriPointA_Z,
		
		culling::M256F& TriPointB_X,
		culling::M256F& TriPointB_Y,
		culling::M256F& TriPointB_Z,
		
		culling::M256F& TriPointC_X,
		culling::M256F& TriPointC_Y,
		culling::M256F& TriPointC_Z,

		culling::M256I& mask
	)
	{
		culling::M256F MASK;

		MASK = _mm256_cmp_ps(TriPointA_Y, TriPointB_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointA_X, TriPointB_X, MASK);
		culling::M256F_SWAP(TriPointA_Y, TriPointB_Y, MASK);
		culling::M256F_SWAP(TriPointA_Z, TriPointB_Z, MASK);
		
		MASK = _mm256_cmp_ps(TriPointA_Y, TriPointC_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointA_X, TriPointC_X, MASK);
		culling::M256F_SWAP(TriPointA_Y, TriPointC_Y, MASK);
		culling::M256F_SWAP(TriPointA_Z, TriPointC_Z, MASK);

		MASK = _mm256_cmp_ps(TriPointB_Y, TriPointC_Y, _CMP_LT_OQ);
		culling::M256F_SWAP(TriPointB_X, TriPointC_X, MASK);
		culling::M256F_SWAP(TriPointB_Y, TriPointC_Y, MASK);
		culling::M256F_SWAP(TriPointB_Z, TriPointC_Z, MASK);
	}
}