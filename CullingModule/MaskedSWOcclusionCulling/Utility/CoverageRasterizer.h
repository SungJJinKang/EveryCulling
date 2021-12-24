#pragma once

#include "../SWDepthBuffer.h"
#include "../../../DataType/Math/Vector.h"
#include "../../../DataType/Math/Triangle.h"

static_assert(TILE_WIDTH % 16 == 0);
static_assert(TILE_HEIGHT % 8 == 0);
static_assert(TILE_WIDTH * TILE_HEIGHT == 128 || TILE_WIDTH * TILE_HEIGHT == 256);

namespace culling
{
	namespace CoverageRasterizer
	{
		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		extern culling::M256I FillBottomFlatTriangle
		(
			const Vec2& TileLeftBottomOriginPoint, 
			const Vec2& point1, 
			const Vec2& point2, 
			const Vec2& point3
		);
		
		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		extern culling::M256I FillTopFlatTriangle
		(
			const Vec2& TileLeftBottomOriginPoint, 
			const Vec2& point1, 
			const Vec2& point2, 
			const Vec2& point3
		);

		

		/// <summary>
		/// https://dl.acm.org/doi/pdf/10.1145/800250.807490?casa_token=alcaBmG1OpoAAAAA:a7Wktjv1YCEp-IODF_dRPbZNDVkRQxPGz67vvDGBfseW6UayupLQM8JizEuMdHT22ymouD-ExDVNPmU
		/// 4page
		/// 
		/// 
		/// </summary>
		extern culling::M256I FillTriangle
		(
			const Vec2& TileLeftBottomOriginPoint, 
			const culling::Vec2& triangleVertex1,
			const culling::Vec2& triangleVertex2,
			const culling::Vec2& triangleVertex3
		);
		extern culling::M256I FillTriangle
		(
			const Vec2& LeftBottomPoint, 
			TwoDTriangle& triangle
		);
		extern culling::M256I FillTriangle
		(
			const Vec2& LeftBottomPoint, 
			ThreeDTriangle& triangle
		);

		
		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		extern void FillBottomFlatTriangleBatch
		(
			culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& LeftBottomPoint,
			const culling::M256F& TriPointA_X,
			const culling::M256F& TriPointA_Y,

			const culling::M256F& TriPointB_X,
			const culling::M256F& TriPointB_Y,

			const culling::M256F& TriPointC_X,
			const culling::M256F& TriPointC_Y
		);

		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		extern void FillTopFlatTriangleBatch
		(
			culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& LeftBottomPoint,
			const culling::M256F& TriPointA_X,
			const culling::M256F& TriPointA_Y,

			const culling::M256F& TriPointB_X,
			const culling::M256F& TriPointB_Y,

			const culling::M256F& TriPointC_X,
			const culling::M256F& TriPointC_Y
		);
		
		extern void FillTriangleBatch
		(
			culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& LeftBottomPoint,
			culling::M256F& TriPointA_X,
			culling::M256F& TriPointA_Y,

			culling::M256F& TriPointB_X,
			culling::M256F& TriPointB_Y,

			culling::M256F& TriPointC_X,
			culling::M256F& TriPointC_Y
		);

		
	};
}


