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
		FORCE_INLINE extern void FillBottomFlatTriangleBatch
		(
			const size_t triangleCount,
			culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& TileLeftBottomOriginPoint,

			const culling::M256I* const leftFaceEvent,
			const culling::M256I* const rightFaceEvent,

			const culling::M256F& bottomEdgeY
		)
		{
			culling::M256I Mask1[8];
			culling::M256I Mask2[8];

			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				Mask1[triIndex] = _mm256_sllv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent[triIndex]);
				Mask2[triIndex] = _mm256_sllv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent[triIndex]);
			}
			culling::M256I Result[8];

			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				const culling::M256F blend = _mm256_andnot_ps(*reinterpret_cast<const culling::M256F*>(Mask2 + triIndex), *reinterpret_cast<const culling::M256F*>(Mask1 + triIndex));
				Result[triIndex] = *reinterpret_cast<const culling::M256I*>(&blend);
			}

			culling::M256F aboveFlatBottomTriangleFace[8];
			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				aboveFlatBottomTriangleFace[triIndex] = _mm256_cmp_ps(_mm256_setr_ps(TileLeftBottomOriginPoint.y + 0.5f, TileLeftBottomOriginPoint.y + 1.5f, TileLeftBottomOriginPoint.y + 2.5f, TileLeftBottomOriginPoint.y + 3.5f, TileLeftBottomOriginPoint.y + 4.5f, TileLeftBottomOriginPoint.y + 5.5f, TileLeftBottomOriginPoint.y + 6.5f, TileLeftBottomOriginPoint.y + 7.5f), _mm256_set1_ps(reinterpret_cast<const float*>(&bottomEdgeY)[triIndex]), _CMP_GE_OQ);
			}

			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				outCoverageMask[triIndex] = _mm256_and_si256(Result[triIndex], *reinterpret_cast<const culling::M256I*>(aboveFlatBottomTriangleFace + triIndex));
			}
		}

		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		FORCE_INLINE extern void FillTopFlatTriangleBatch
		(
			const size_t triangleCount,
			culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& TileLeftBottomOriginPoint,

			const culling::M256I* const leftFaceEvent,
			const culling::M256I* const rightFaceEvent,

			const culling::M256F& topEdgeY
		)
		{
			culling::M256I Mask1[8];
			culling::M256I Mask2[8];

			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				Mask1[triIndex] = _mm256_sllv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent[triIndex]);
				Mask2[triIndex] = _mm256_sllv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent[triIndex]);
			}
			culling::M256I Result[8];

			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				const culling::M256F blend = _mm256_andnot_ps(*reinterpret_cast<const culling::M256F*>(Mask2 + triIndex), *reinterpret_cast<const culling::M256F*>(Mask1 + triIndex));
				Result[triIndex] = *reinterpret_cast<const culling::M256I*>(&blend);
			}

			culling::M256F belowFlatTopTriangleFace[8];
			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				belowFlatTopTriangleFace[triIndex] = _mm256_cmp_ps(_mm256_setr_ps(TileLeftBottomOriginPoint.y + 0.5f, TileLeftBottomOriginPoint.y + 1.5f, TileLeftBottomOriginPoint.y + 2.5f, TileLeftBottomOriginPoint.y + 3.5f, TileLeftBottomOriginPoint.y + 4.5f, TileLeftBottomOriginPoint.y + 5.5f, TileLeftBottomOriginPoint.y + 6.5f, TileLeftBottomOriginPoint.y + 7.5f), _mm256_set1_ps(reinterpret_cast<const float*>(&topEdgeY)[triIndex]), _CMP_LE_OQ);
			}

			for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
			{
				outCoverageMask[triIndex] = _mm256_and_si256(Result[triIndex], *reinterpret_cast<const culling::M256I*>(belowFlatTopTriangleFace + triIndex));
			}
		}


		
		/*
		extern void FillTriangleBatch
		(
			const size_t triangleCount,
			culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& LeftBottomPoint,
			culling::M256F& TriPointA_X,
			culling::M256F& TriPointA_Y,

			culling::M256F& TriPointB_X,
			culling::M256F& TriPointB_Y,

			culling::M256F& TriPointC_X,
			culling::M256F& TriPointC_Y
		);
		*/

		
	};
}


