#pragma once

#include "../../../EveryCullingCore.h"
#include "../SWDepthBuffer.h"

namespace culling
{
	namespace depthBufferTileHelper
	{
		FORCE_INLINE extern void ComputeBinBoundingBoxFromThreeVertices
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
			culling::M256I minScreenPixelX, minScreenPixelY, maxScreenPixelX, maxScreenPixelY;
			
			static const culling::M256I WIDTH_MASK = _mm256_set1_epi32(~(TILE_WIDTH - 1));
			static const culling::M256I HEIGHT_MASK = _mm256_set1_epi32(~(TILE_HEIGHT - 1));

			minScreenPixelX = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_min_ps(pointAScreenPixelX, _mm256_min_ps(pointBScreenPixelX, pointCScreenPixelX))));
			minScreenPixelY = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_min_ps(pointAScreenPixelY, _mm256_min_ps(pointBScreenPixelY, pointCScreenPixelY))));
			maxScreenPixelX = _mm256_cvttps_epi32(_mm256_ceil_ps(_mm256_max_ps(pointAScreenPixelX, _mm256_max_ps(pointBScreenPixelX, pointCScreenPixelX))));
			maxScreenPixelY = _mm256_cvttps_epi32(_mm256_ceil_ps(_mm256_max_ps(pointAScreenPixelY, _mm256_max_ps(pointBScreenPixelY, pointCScreenPixelY))));

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


