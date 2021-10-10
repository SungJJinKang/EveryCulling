#pragma once

#include "../CoverageMask.h"

#include "../../../DataType/Math/Vector.h"
#include "../../../DataType/Math/Triangle.h"

static_assert(TILE_WIDTH % 16 == 0);
static_assert(TILE_HEIGHT % 8 == 0);
static_assert(TILE_WIDTH * TILE_HEIGHT == 128 || TILE_WIDTH * TILE_HEIGHT == 256);

namespace culling
{
	class DOOM_API CoverageRasterizer
	{
	private:

		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		M256I FillBottomFlatTriangle(CoverageMask& coverageMask, const Vec2& LeftBottomPoint, const Vec2& point1, const Vec2& point2, const Vec2& point3);
		
		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		M256I FillTopFlatTriangle(CoverageMask& coverageMask, const Vec2& LeftBottomPoint, const Vec2& point1, const Vec2& point2, const Vec2& point3);
		
		

	public:

		/// <summary>
		/// https://dl.acm.org/doi/pdf/10.1145/800250.807490?casa_token=alcaBmG1OpoAAAAA:a7Wktjv1YCEp-IODF_dRPbZNDVkRQxPGz67vvDGBfseW6UayupLQM8JizEuMdHT22ymouD-ExDVNPmU
		/// 4page
		/// 
		/// 
		/// </summary>
		void FillTriangle(CoverageMask& coverageMask, Vec2 LeftBottomPoint, TwoDTriangle& triangle);

	};
}


