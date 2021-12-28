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

			//A grid square, including its (x, y) window coordinates, z (depth), and associated data which may be added by fragment shaders, is called a fragment. A
			//fragment is located by its lower left corner, which lies on integer grid coordinates.
			//Rasterization operations also refer to a fragment��s center, which is offset by ( 1/2, 1/2 )
			//from its lower left corner(and so lies on half - integer coordinates).

			static const culling::M256I WIDTH_MASK = _mm256_set1_epi32(~(TILE_WIDTH - 1));
			static const culling::M256I HEIGHT_MASK = _mm256_set1_epi32(~(TILE_HEIGHT - 1));

			minScreenPixelX = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_min_ps(pointAScreenPixelX, _mm256_min_ps(pointBScreenPixelX, pointCScreenPixelX))));
			minScreenPixelY = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_min_ps(pointAScreenPixelY, _mm256_min_ps(pointBScreenPixelY, pointCScreenPixelY))));
			maxScreenPixelX = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_max_ps(pointAScreenPixelX, _mm256_max_ps(pointBScreenPixelX, pointCScreenPixelX))));
			maxScreenPixelY = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_max_ps(pointAScreenPixelY, _mm256_max_ps(pointBScreenPixelY, pointCScreenPixelY))));

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

		FORCE_INLINE extern float MinFloatFromM256F(const culling::M256F& data)
		{
			float min = FLT_MAX;
			for (size_t i = 0; i < 8; i++)
			{
				min = MIN(min, reinterpret_cast<const float*>(&data)[i]);
			}
			return min;
		}

		FORCE_INLINE extern float MaxFloatFromM256F(const culling::M256F& data)
		{
			float max = -FLT_MAX;
			for (size_t i = 0; i < 8; i++)
			{
				max = MAX(max, reinterpret_cast<const float*>(&data)[i]);
			}
			return max;
		}

		FORCE_INLINE extern void ComputeBinBoundingBoxFromVertex
		(
			const culling::M256F& screenPixelX,
			const culling::M256F& screenPixelY,
			int& outBinBoundingBoxMinX,
			int& outBinBoundingBoxMinY,
			int& outBinBoundingBoxMaxX,
			int& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		)
		{
			int minScreenPixelX, minScreenPixelY, maxScreenPixelX, maxScreenPixelY;
			
			static const int WIDTH_MASK = ~(TILE_WIDTH - 1);
			static const int HEIGHT_MASK = ~(TILE_HEIGHT - 1);

			minScreenPixelX = MAX(0, (int)MinFloatFromM256F(screenPixelX));
			minScreenPixelY = MAX(0, (int)MinFloatFromM256F(screenPixelY));
			maxScreenPixelX = MAX(0, (int)MaxFloatFromM256F(screenPixelX));
			maxScreenPixelY = MAX(0, (int)MaxFloatFromM256F(screenPixelY));
			
			outBinBoundingBoxMinX = minScreenPixelX & WIDTH_MASK;
			outBinBoundingBoxMinY = minScreenPixelY & HEIGHT_MASK;
			outBinBoundingBoxMaxX = maxScreenPixelX & WIDTH_MASK;
			outBinBoundingBoxMaxY = maxScreenPixelY & HEIGHT_MASK;

			outBinBoundingBoxMinX = MIN(depthBuffer.mResolution.mRightTopTileOrginX, MAX(outBinBoundingBoxMinX, depthBuffer.mResolution.mLeftBottomTileOrginX));
			outBinBoundingBoxMinY = MIN(depthBuffer.mResolution.mRightTopTileOrginY, MAX(outBinBoundingBoxMinY, depthBuffer.mResolution.mLeftBottomTileOrginY));
			outBinBoundingBoxMaxX = MAX(depthBuffer.mResolution.mLeftBottomTileOrginX, MIN(outBinBoundingBoxMaxX, depthBuffer.mResolution.mRightTopTileOrginX));
			outBinBoundingBoxMaxY = MAX(depthBuffer.mResolution.mLeftBottomTileOrginY, MIN(outBinBoundingBoxMaxY, depthBuffer.mResolution.mRightTopTileOrginY));

			assert(outBinBoundingBoxMinX <= outBinBoundingBoxMaxX);
			assert(outBinBoundingBoxMinY <= outBinBoundingBoxMaxY);
		}
	};
}


