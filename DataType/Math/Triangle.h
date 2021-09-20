#pragma once

#include "../../SIMD_Core.h"

#include "Vector.h"

namespace culling
{
	struct TwoDTriangle
	{
		Vector2 Points[3];
	};

	struct ThreeDTriangle
	{
		Vector3 Points[3];
	};

	

	/// <summary>
	/// Sort TwoDTriangle Points y ascending.
	/// Point1 is TopMost Vector2
	/// </summary>
	/// <param name="triangle"></param>

	/// <summary>
	/// Sort TwoDTriangle Points y ascending.
	/// Point1 is TopMost Vector2
	/// </summary>
	/// <param name="triangle"></param>
	inline void SortTriangle(TwoDTriangle& triangle)
	{
		if (triangle.Points[0].y < triangle.Points[1].y)
		{
			std::swap(triangle.Points[0], triangle.Points[1]);
		}
		if (triangle.Points[0].y < triangle.Points[2].y)
		{
			std::swap(triangle.Points[0], triangle.Points[2]);
		}
		if (triangle.Points[1].y < triangle.Points[2].y)
		{
			std::swap(triangle.Points[1], triangle.Points[2]);
		}
	}

	inline bool IsFrontFace(const ThreeDTriangle& triangle)
	{
		//triangle should be in projection space
		(v1x - v0x)(v2y - v0y) - (v1y - v0y)(v2x - v0x)
		==
		(v1x - v0x) * (v2y - v0y) - (v0x - v2x) * (v0y - v1y)
			// Perform backface test. 
		__mw triArea1 = _mmw_mul_ps(_mmw_sub_ps(pVtxX[1], pVtxX[0]), _mmw_sub_ps(pVtxY[2], pVtxY[0]));
		__mw triArea2 = _mmw_mul_ps(_mmw_sub_ps(pVtxX[0], pVtxX[2]), _mmw_sub_ps(pVtxY[0], pVtxY[1]));
		__mw triArea = _mmw_sub_ps(triArea1, triArea2);
		__mw ccwMask = _mmw_cmpgt_ps(triArea, _mmw_setzero_ps());
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
	/// Passed M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX"></param>
	/// <param name="TriPointY"></param>
	inline void Sort_8_2DTriangles(M256F* TriPointX, M256F* TriPointY)
	{
		M256F MASK;

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[1], _CMP_LE_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[1], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[1], MASK);

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[2], _CMP_LE_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[2], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[2], MASK);

		MASK = _mm256_cmp_ps(TriPointY[1], TriPointY[2], _CMP_LE_OQ);
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
	/// Passed M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX">3 size array M256F</param>
	/// <param name="TriPointY">3 size array M256F</param>
	/// <param name="TriPointZ">3 size array M256F</param>
	inline void Sort_8_3DTriangles(M256F* TriPointX, M256F* TriPointY, M256F* TriPointZ)
	{
		M256F MASK;

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[1], _CMP_LE_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[1], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[1], MASK);
		culling::M256F_SWAP(TriPointZ[0], TriPointZ[1], MASK);

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[2], _CMP_LE_OQ);
		culling::M256F_SWAP(TriPointX[0], TriPointX[2], MASK);
		culling::M256F_SWAP(TriPointY[0], TriPointY[2], MASK);
		culling::M256F_SWAP(TriPointZ[0], TriPointZ[2], MASK);

		MASK = _mm256_cmp_ps(TriPointY[1], TriPointY[2], _CMP_LE_OQ);
		culling::M256F_SWAP(TriPointX[1], TriPointX[2], MASK);
		culling::M256F_SWAP(TriPointY[1], TriPointY[2], MASK);
		culling::M256F_SWAP(TriPointZ[1], TriPointZ[2], MASK);
	}

}