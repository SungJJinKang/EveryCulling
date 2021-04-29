#pragma once

#include "../CoverageMask.h"
#include "../Triangle.h"

#include "../../../SIMD_Core.h"

static_assert(TILE_WIDTH % 16 == 0);
static_assert(TILE_HEIGHT % 8 == 0);
static_assert(TILE_WIDTH * TILE_HEIGHT == 128 || TILE_WIDTH * TILE_HEIGHT == 256);

namespace culling
{
	class CoverageRasterizer
	{
	private:

		/// <summary>
		/// Assume Triangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		M128I FillBottomFlatTriangle(CoverageMask& coverageMask, const Point& LeftBottomPoint, const Point& point1, const Point& point2, const Point& point3);
		/// <summary>
		/// Assume Triangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		M128I FillTopFlatTriangle(CoverageMask& coverageMask, const Point& LeftBottomPoint, const Point& point1, const Point& point2, const Point& point3);
		
		/// <summary>
		/// Sort Triangle Points y ascending.
		/// Point1 is TopMost Point
		/// </summary>
		/// <param name="triangle"></param>
		void SortTriangle(Triangle& triangle);

	public:

		/// <summary>
		/// https://dl.acm.org/doi/pdf/10.1145/800250.807490?casa_token=alcaBmG1OpoAAAAA:a7Wktjv1YCEp-IODF_dRPbZNDVkRQxPGz67vvDGBfseW6UayupLQM8JizEuMdHT22ymouD-ExDVNPmU
		/// 4page
		/// 
		/// 
		/// </summary>
		void FillTriangle(CoverageMask& coverageMask, Point LeftBottomPoint, Triangle& triangle);

	};
}


