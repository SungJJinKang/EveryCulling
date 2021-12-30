#pragma once

#include "../../../EveryCullingCore.h"

#include "depthUtility.h"

namespace culling
{
	namespace DepthValueComputer
	{

		/*
		enum class eDepthType
		{
			MinDepth,
			MaxDepth
		};
		*/
		

		FORCE_INLINE extern void ComputeFlatTriangleMaxDepthValue
		(
			const size_t triangleCount,
			culling::M256F* const subTileMaxValues,
			const std::uint32_t tileOriginX, // 32x8 tile
			const std::uint32_t tileOriginY, // 32x8 tile

			const culling::M256F& vertexPoint1X, 
			const culling::M256F& vertexPoint1Y,
			const culling::M256F& vertexPoint1Z,

			const culling::M256F& vertexPoint2X,
			const culling::M256F& vertexPoint2Y,
			const culling::M256F& vertexPoint2Z,

			const culling::M256F& vertexPoint3X,
			const culling::M256F& vertexPoint3Y,
			const culling::M256F& vertexPoint3Z,

			const culling::M256I* const leftFaceEventOfTriangles, // eight _mm256i
			const culling::M256I* const rightFaceEventOfTriangles, // eight _mm256i

			const culling::M256F& minYOfTriangle,
			const culling::M256F& maxYOfTriangle,

			const std::uint32_t triangleMask
		)
		{

#ifdef DEBUG_CULLING
			for(int i = 0 ; i < triangleCount ; i++)
			{
				assert(reinterpret_cast<const float*>(&maxYOfTriangle)[i] >= reinterpret_cast<const float*>(&minYOfTriangle)[i]);
			}
#endif
			//const culling::M256I minYIntOfTriangles = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_add_ps(minYOfTriangle, _mm256_set1_ps(0.5f))));
			//const culling::M256I maxYIntOfTriangles = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_add_ps(maxYOfTriangle, _mm256_set1_ps(0.5f))));

			//const culling::M256I tileStartRowIndex = _mm256_max_epi32(_mm256_sub_epi32(minYIntOfTriangles, _mm256_set1_epi32(tileOriginY)), _mm256_set1_epi32(0));
			//const culling::M256I tileEndRowIndex = _mm256_sub_epi32(_mm256_set1_epi32(TILE_HEIGHT), _mm256_max_epi32(_mm256_sub_epi32(_mm256_set1_epi32(tileOriginY + TILE_HEIGHT), maxYIntOfTriangles), _mm256_set1_epi32(0)));

			culling::M256F zPixelDxOfTriangles, zPixelDyOfTriangles;
			culling::depthUtility::ComputeDepthPlane
			(
				vertexPoint3X,
				vertexPoint3Y,
				vertexPoint3Z,

				vertexPoint1X,
				vertexPoint1Y,
				vertexPoint1Z,

				vertexPoint2X,
				vertexPoint2Y,
				vertexPoint2Z,

				zPixelDxOfTriangles,
				zPixelDyOfTriangles
			);

			const culling::M256F bbMinXV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginX + 0.5f)), vertexPoint3X);
			const culling::M256F bbMinYV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginY + 0.5f)), vertexPoint3Y);

			// depth value at tile origin ( 0, 0 )
			culling::M256F depthValueAtTileOriginPoint = _mm256_fmadd_ps(zPixelDxOfTriangles, bbMinXV0, _mm256_fmadd_ps(zPixelDyOfTriangles, bbMinYV0, vertexPoint3Z)); 
			
			// Compute Zmin and Zmax for the triangle (used to narrow the range for difficult tiles)
			const culling::M256F zMinOfTriangle = _mm256_min_ps(vertexPoint1Z, _mm256_min_ps(vertexPoint2Z, vertexPoint3Z));
			const culling::M256F zMaxOfTriangle = _mm256_max_ps(vertexPoint1Z, _mm256_max_ps(vertexPoint2Z, vertexPoint3Z));


			for (size_t triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
			{
				if ((triangleMask & (1 << triangleIndex)) != 0x00000000)
				{
					const culling::M256I& leftFaceEvent = leftFaceEventOfTriangles[triangleIndex];
					const culling::M256I& rightFaceEvent = rightFaceEventOfTriangles[triangleIndex];

					const float zPixelDx = reinterpret_cast<const float*>(&zPixelDxOfTriangles)[triangleIndex];
					const float zPixelDy = reinterpret_cast<const float*>(&zPixelDyOfTriangles)[triangleIndex];

					//const culling::M256F zTriMax = _mm256_set1_ps((reinterpret_cast<const float*>(&zMaxOfTriangle))[triangleIndex]);
					//const culling::M256F zTriMin = _mm256_set1_ps((reinterpret_cast<const float*>(&zMinOfTriangle))[triangleIndex]);

					//
					// 4 5 6 7   <-- _mm256i
					// 0 1 2 3
					// depth value at (0, 0) of subtiles
					culling::M256F zValueAtOriginPointOfSubTiles = _mm256_fmadd_ps(_mm256_set1_ps(zPixelDx), _mm256_setr_ps(0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3, 0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3),
						_mm256_fmadd_ps(_mm256_set1_ps(zPixelDy), _mm256_setr_ps(0, 0, 0, 0, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT), _mm256_set1_ps((reinterpret_cast<const float*>(&depthValueAtTileOriginPoint))[triangleIndex])));

					//
					// 4 5 6 7   <-- _mm256i
					// 0 1 2 3
					// FaceEventOfSubTiles
					//
					// face event in each subtile
					for (int rowIndexInSubtiles = 0; rowIndexInSubtiles < SUB_TILE_HEIGHT; rowIndexInSubtiles++)
					{
						const culling::M256I leftFaceEventOfRowIndexInSubTiles = _mm256_setr_epi32(reinterpret_cast<const int*>(&leftFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&leftFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&leftFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&leftFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&leftFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles], reinterpret_cast<const int*>(&leftFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles], reinterpret_cast<const int*>(&leftFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles], reinterpret_cast<const int*>(&leftFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles]);
						const culling::M256I leftFaceEventInSubTiles = _mm256_sub_epi32(leftFaceEventOfRowIndexInSubTiles, _mm256_setr_epi32(SUB_TILE_WIDTH * 0, SUB_TILE_WIDTH * 1, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3, SUB_TILE_WIDTH * 0, SUB_TILE_WIDTH * 1, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3));
						const culling::M256I clampedLeftFaceEventInSubTiles = _mm256_min_epi32(_mm256_max_epi32(leftFaceEventInSubTiles, _mm256_set1_epi32(0)), _mm256_set1_epi32(SUB_TILE_WIDTH));

						const culling::M256I rightFaceEventOfRowIndexInSubTiles = _mm256_setr_epi32(reinterpret_cast<const int*>(&rightFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&rightFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&rightFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&rightFaceEvent)[rowIndexInSubtiles], reinterpret_cast<const int*>(&rightFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles], reinterpret_cast<const int*>(&rightFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles], reinterpret_cast<const int*>(&rightFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles], reinterpret_cast<const int*>(&rightFaceEvent)[SUB_TILE_HEIGHT + rowIndexInSubtiles]);
						const culling::M256I rightFaceEventInSubTiles = _mm256_sub_epi32(rightFaceEventOfRowIndexInSubTiles, _mm256_setr_epi32(SUB_TILE_WIDTH * 0, SUB_TILE_WIDTH * 1, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3, SUB_TILE_WIDTH * 0, SUB_TILE_WIDTH * 1, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3));
						const culling::M256I clampedRightFaceEventInSubTiles = _mm256_min_epi32(_mm256_max_epi32(rightFaceEventInSubTiles, _mm256_set1_epi32(0)), _mm256_set1_epi32(SUB_TILE_WIDTH));

						const culling::M256F leftFaceDepthValueOfRowIndexInSubTiles =
							_mm256_add_ps(_mm256_add_ps(_mm256_set1_ps(zPixelDy * rowIndexInSubtiles), zValueAtOriginPointOfSubTiles), _mm256_mul_ps(_mm256_cvtepi32_ps(clampedLeftFaceEventInSubTiles), _mm256_set1_ps(zPixelDx)));

						const culling::M256F rightFaceDepthValueOfRowIndexInSubTiles =
							_mm256_add_ps(_mm256_add_ps(_mm256_set1_ps(zPixelDy * rowIndexInSubtiles), zValueAtOriginPointOfSubTiles), _mm256_mul_ps(_mm256_cvtepi32_ps(clampedRightFaceEventInSubTiles), _mm256_set1_ps(zPixelDx)));
						

						culling::M256F maxZValueAtRowOfSubTiles = _mm256_max_ps(leftFaceDepthValueOfRowIndexInSubTiles, rightFaceDepthValueOfRowIndexInSubTiles);

						// if left, right event is both 0 or both SUB_TILE_WIDTH, exclude the row
						const culling::M256I mashWhenLeftRightSlopeIsLocatedAtLeftOfSubTiles = _mm256_and_si256(_mm256_cmpeq_epi32(clampedLeftFaceEventInSubTiles, _mm256_set1_epi32(0)), _mm256_cmpeq_epi32(clampedRightFaceEventInSubTiles, _mm256_set1_epi32(0)));
						const culling::M256I mashWhenLeftRightSlopeIsLocatedAtRightOfSubTiles = _mm256_and_si256(_mm256_cmpeq_epi32(clampedLeftFaceEventInSubTiles, _mm256_set1_epi32(SUB_TILE_WIDTH)), _mm256_cmpeq_epi32(clampedRightFaceEventInSubTiles, _mm256_set1_epi32(SUB_TILE_WIDTH)));
						const culling::M256I maskWhenLeftRightSlopeIsOutOfSubTiles = _mm256_or_si256(mashWhenLeftRightSlopeIsLocatedAtLeftOfSubTiles, mashWhenLeftRightSlopeIsLocatedAtRightOfSubTiles);

						// when row is greater than max y or less than min y, row is invalid row
						const culling::M256I screenYOfRowInSubTiles = _mm256_setr_epi32(tileOriginY + 0.5f + rowIndexInSubtiles, tileOriginY + 0.5f + rowIndexInSubtiles, tileOriginY + 0.5f + rowIndexInSubtiles, tileOriginY + 0.5f + rowIndexInSubtiles, tileOriginY + 0.5f + rowIndexInSubtiles + SUB_TILE_HEIGHT, tileOriginY + 0.5f + rowIndexInSubtiles + SUB_TILE_HEIGHT, tileOriginY + 0.5f + rowIndexInSubtiles + SUB_TILE_HEIGHT, tileOriginY + 0.5f + rowIndexInSubtiles + SUB_TILE_HEIGHT);
						const culling::M256I maskWhenRowIsOutOfMinMaxY = _mm256_or_si256(_mm256_cmpgt_epi32(screenYOfRowInSubTiles, _mm256_set1_epi32(static_cast<int>(reinterpret_cast<const float*>(&maxYOfTriangle)[triangleIndex]))), _mm256_cmpgt_epi32(_mm256_set1_epi32(static_cast<int>(reinterpret_cast<const float*>(&minYOfTriangle)[triangleIndex])), screenYOfRowInSubTiles));

						maxZValueAtRowOfSubTiles = _mm256_blendv_ps(maxZValueAtRowOfSubTiles, _mm256_set1_ps((float)MIN_DEPTH_VALUE), *reinterpret_cast<const culling::M256F*>(&maskWhenLeftRightSlopeIsOutOfSubTiles));
						maxZValueAtRowOfSubTiles = _mm256_blendv_ps(maxZValueAtRowOfSubTiles, _mm256_set1_ps((float)MIN_DEPTH_VALUE), *reinterpret_cast<const culling::M256F*>(&maskWhenRowIsOutOfMinMaxY));

						subTileMaxValues[triangleIndex] = _mm256_max_ps(subTileMaxValues[triangleIndex], maxZValueAtRowOfSubTiles);
					}
				}
				

			}

		}
		

	};
}


