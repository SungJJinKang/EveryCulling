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
	inline void SortTriangle(TwoDTriangle& triangle)
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

	inline bool IsFrontFaceOfProjectSpaceTriangle(const TwoDTriangle& triangle)
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
	inline culling::__M128F IsFrontFaceOfPerspectiveSpaceTrianglesSIMD(const culling::__M128F* verticesX, const culling::__M128F* verticesY)
	{
		/*
		//triangle should be in projection space
		(v1x - v0x)(v2y - v0y) - (v1y - v0y)(v2x - v0x)
		==
		(v1x - v0x) * (v2y - v0y) - (v0x - v2x) * (v0y - v1y)
		// Perform backface test.
		*/
		const culling::__M128F triArea1 = M128F_MUL(M128F_SUB(verticesX[1], verticesX[0]), M128F_SUB(verticesY[2], verticesY[0]));
		const culling::__M128F triArea2 = M128F_MUL(M128F_SUB(verticesX[0], verticesX[2]), M128F_SUB(verticesY[0], verticesY[1]));
		const culling::__M128F triArea = M128F_SUB(triArea1, triArea2);
		const culling::__M128F ccwMask = _mm_cmpgt_ps(triArea, M128F_Zero);
		return ccwMask;
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
	/// Passed culling::__M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX"></param>
	/// <param name="TriPointY"></param>
	inline void Sort_8_2DTriangles(culling::__M256F* TriPointX, culling::__M256F* TriPointY)
	{
		culling::__M256F MASK;

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[1], _CMP_LE_OQ);
		M256F_SWAP(TriPointX[0], TriPointX[1], MASK);
		M256F_SWAP(TriPointY[0], TriPointY[1], MASK);

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[2], _CMP_LE_OQ);
		M256F_SWAP(TriPointX[0], TriPointX[2], MASK);
		M256F_SWAP(TriPointY[0], TriPointY[2], MASK);

		MASK = _mm256_cmp_ps(TriPointY[1], TriPointY[2], _CMP_LE_OQ);
		M256F_SWAP(TriPointX[1], TriPointX[2], MASK);
		M256F_SWAP(TriPointY[1], TriPointY[2], MASK);
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
	/// Passed culling::__M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX">3 size array culling::__M256F</param>
	/// <param name="TriPointY">3 size array culling::__M256F</param>
	/// <param name="TriPointZ">3 size array culling::__M256F</param>
	inline void Sort_8_3DTriangles(culling::__M256F* TriPointX, culling::__M256F* TriPointY, culling::__M256F* TriPointZ)
	{
		culling::__M256F MASK;

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[1], _CMP_LE_OQ);
		M256F_SWAP(TriPointX[0], TriPointX[1], MASK);
		M256F_SWAP(TriPointY[0], TriPointY[1], MASK);
		M256F_SWAP(TriPointZ[0], TriPointZ[1], MASK);

		MASK = _mm256_cmp_ps(TriPointY[0], TriPointY[2], _CMP_LE_OQ);
		M256F_SWAP(TriPointX[0], TriPointX[2], MASK);
		M256F_SWAP(TriPointY[0], TriPointY[2], MASK);
		M256F_SWAP(TriPointZ[0], TriPointZ[2], MASK);

		MASK = _mm256_cmp_ps(TriPointY[1], TriPointY[2], _CMP_LE_OQ);
		M256F_SWAP(TriPointX[1], TriPointX[2], MASK);
		M256F_SWAP(TriPointY[1], TriPointY[2], MASK);
		M256F_SWAP(TriPointZ[1], TriPointZ[2], MASK);
	}

}