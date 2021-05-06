#include "Triangle.h"

#include <algorithm>

/// <summary>
/// Sort TwoDTriangle Points y ascending.
/// Point1 is TopMost Vector2
/// </summary>
/// <param name="triangle"></param>
void culling::SortTriangle(TwoDTriangle& triangle)
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
void culling::Sort_8_2DTriangles(M256F* TriPointX, M256F* TriPointY)
{
	M256F MASK;

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
/// Passed M256F* is pointer of 3 size array
/// 
/// </summary>
/// <param name="TriPointX">3 size array M256F</param>
/// <param name="TriPointY">3 size array M256F</param>
/// <param name="TriPointZ">3 size array M256F</param>
void culling::Sort_8_3DTriangles(M256F* TriPointX, M256F* TriPointY, M256F* TriPointZ)
{
	M256F MASK;

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
