#pragma once

#include "SIMD_Core.h"

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
	void SortTriangle(TwoDTriangle& triangle);

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
	void Sort_8_2DTriangles(M256F* TriPointX, M256F* TriPointY);

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
	void Sort_8_3DTriangles(M256F* TriPointX, M256F* TriPointY, M256F* TriPointZ);
}