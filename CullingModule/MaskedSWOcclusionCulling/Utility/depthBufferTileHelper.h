#pragma once

#include "../../../EveryCullingCore.h"
#include "../SWDepthBuffer.h"

#define FLOAT_OVERFLOW_CHECKER (float)2147483645.0f

namespace culling
{
	namespace depthBufferTileHelper
	{
		EVERYCULLING_FORCE_INLINE extern void ComputeBinBoundingBoxFromThreeVertices
		(
			const culling::M256F& pointAScreenPixelX,
			const culling::M256F& pointAScreenPixelY,

			const culling::M256F& pointBScreenPixelX,
			const culling::M256F& pointBScreenPixelY,

			const culling::M256F& pointCScreenPixelX,
			const culling::M256F& pointCScreenPixelY,

			culling::M256I& outBinBoundingBoxMinX,
			culling::M256I& outBinBoundingBoxMinY,
			culling::M256I& outBinBoundingBoxMaxX,
			culling::M256I& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		)
		{
			static const culling::M256I WIDTH_MASK = _mm256_set1_epi32(~(TILE_WIDTH - 1));
			static const culling::M256I HEIGHT_MASK = _mm256_set1_epi32(~(TILE_HEIGHT - 1));

			// to prevent overflow.
			// when floating point is converted to integer
			const culling::M256F minScreenPixelFloatX = _mm256_max_ps(_mm256_set1_ps(-FLOAT_OVERFLOW_CHECKER), _mm256_min_ps(_mm256_floor_ps(_mm256_min_ps(pointAScreenPixelX, _mm256_min_ps(pointBScreenPixelX, pointCScreenPixelX))), _mm256_set1_ps(FLOAT_OVERFLOW_CHECKER)));
			const culling::M256F minScreenPixelFloatY = _mm256_max_ps(_mm256_set1_ps(-FLOAT_OVERFLOW_CHECKER), _mm256_min_ps(_mm256_floor_ps(_mm256_min_ps(pointAScreenPixelY, _mm256_min_ps(pointBScreenPixelY, pointCScreenPixelY))), _mm256_set1_ps(FLOAT_OVERFLOW_CHECKER)));
			const culling::M256F maxScreenPixelFloatX = _mm256_max_ps(_mm256_set1_ps(-FLOAT_OVERFLOW_CHECKER), _mm256_min_ps(_mm256_ceil_ps(_mm256_max_ps(pointAScreenPixelX, _mm256_max_ps(pointBScreenPixelX, pointCScreenPixelX))), _mm256_set1_ps(FLOAT_OVERFLOW_CHECKER)));
			const culling::M256F maxScreenPixelFloatY = _mm256_max_ps(_mm256_set1_ps(-FLOAT_OVERFLOW_CHECKER), _mm256_min_ps(_mm256_ceil_ps(_mm256_max_ps(pointAScreenPixelY, _mm256_max_ps(pointBScreenPixelY, pointCScreenPixelY))), _mm256_set1_ps(FLOAT_OVERFLOW_CHECKER)));


			const culling::M256I minScreenPixelX = _mm256_cvttps_epi32(minScreenPixelFloatX);
			const culling::M256I minScreenPixelY = _mm256_cvttps_epi32(minScreenPixelFloatY);
			const culling::M256I maxScreenPixelX = _mm256_cvttps_epi32(maxScreenPixelFloatX);
			const culling::M256I maxScreenPixelY = _mm256_cvttps_epi32(maxScreenPixelFloatY);
			

			// How "and" works?
			// 0000 0000 0110 0011 <- 96 = 32 * 3 + 3
			//        AND
			// 1111 1111 1110 0000 <- WIDTH_MASK
			//         |
			//         | Masking low bits to make coordinate multiple of tile size
			//         V
			// 0000 0000 0110 0000 <- 92 = 32 * 3 ( multiple of tile size )
			//		

			outBinBoundingBoxMinX = _mm256_and_si256(minScreenPixelX, WIDTH_MASK);
			outBinBoundingBoxMinY = _mm256_and_si256(minScreenPixelY, HEIGHT_MASK);
			outBinBoundingBoxMaxX = _mm256_and_si256(maxScreenPixelX, WIDTH_MASK);
			outBinBoundingBoxMaxY = _mm256_and_si256(maxScreenPixelY, HEIGHT_MASK);

			outBinBoundingBoxMinX = _mm256_min_epi32(_mm256_set1_epi32(depthBuffer.mResolution.mRightTopTileOrginX), _mm256_max_epi32(outBinBoundingBoxMinX, _mm256_set1_epi32(depthBuffer.mResolution.mLeftBottomTileOrginX)));
			outBinBoundingBoxMinY = _mm256_min_epi32(_mm256_set1_epi32(depthBuffer.mResolution.mRightTopTileOrginY), _mm256_max_epi32(outBinBoundingBoxMinY, _mm256_set1_epi32(depthBuffer.mResolution.mLeftBottomTileOrginY)));
			outBinBoundingBoxMaxX = _mm256_max_epi32(_mm256_set1_epi32(depthBuffer.mResolution.mLeftBottomTileOrginX), _mm256_min_epi32(outBinBoundingBoxMaxX, _mm256_set1_epi32(depthBuffer.mResolution.mRightTopTileOrginX)));
			outBinBoundingBoxMaxY = _mm256_max_epi32(_mm256_set1_epi32(depthBuffer.mResolution.mLeftBottomTileOrginY), _mm256_min_epi32(outBinBoundingBoxMaxY, _mm256_set1_epi32(depthBuffer.mResolution.mRightTopTileOrginY)));
			

		}

		

		
	};
}


