#pragma once

#include <Vector2.h>

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
		M128I FillBottomFlatTriangle(CoverageMask& coverageMask, const math::Vector2& LeftBottomPoint, const math::Vector2& point1, const math::Vector2& point2, const math::Vector2& point3);
		/// <summary>
		/// Assume Triangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		M128I FillTopFlatTriangle(CoverageMask& coverageMask, const math::Vector2& LeftBottomPoint, const math::Vector2& point1, const math::Vector2& point2, const math::Vector2& point3);
		
		/// <summary>
		/// Sort Triangle Points y ascending.
		/// Point1 is TopMost math::Vector2
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
		void FillTriangle(CoverageMask& coverageMask, math::Vector2 LeftBottomPoint, Triangle& triangle);

	};
}


