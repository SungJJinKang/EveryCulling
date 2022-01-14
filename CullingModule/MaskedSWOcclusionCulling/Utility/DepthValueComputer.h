#pragma once

#include "../../../EveryCullingCore.h"

#include "../../../DataType/Math/SIMD_Core.h"
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
		

		EVERYCULLING_FORCE_INLINE extern void ComputeFlatTriangleMaxDepthValue
		(
			culling::M256F& subTileMaxValues,
			const std::uint32_t tileOriginX, // 32x8 tile
			const std::uint32_t tileOriginY, // 32x8 tile

			const float vertexPoint1X, 
			const float vertexPoint1Y,
			const float vertexPoint1Z,

			const float vertexPoint2X,
			const float vertexPoint2Y,
			const float vertexPoint2Z,

			const float vertexPoint3X,
			const float vertexPoint3Y,
			const float vertexPoint3Z,

			const culling::M256I& leftFaceEventOfTriangles, // eight _mm256i
			const culling::M256I& rightFaceEventOfTriangles, // eight _mm256i

			const float minYOfTriangle,
			const float maxYOfTriangle
		)
		{
			
			//const culling::M256I minYIntOfTriangles = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_add_ps(minYOfTriangle, _mm256_set1_ps(0.5f))));
			//const culling::M256I maxYIntOfTriangles = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_add_ps(maxYOfTriangle, _mm256_set1_ps(0.5f))));

			//const culling::M256I tileStartRowIndex = _mm256_max_epi32(_mm256_sub_epi32(minYIntOfTriangles, _mm256_set1_epi32(tileOriginY)), _mm256_set1_epi32(0));
			//const culling::M256I tileEndRowIndex = _mm256_sub_epi32(_mm256_set1_epi32(TILE_HEIGHT), _mm256_max_epi32(_mm256_sub_epi32(_mm256_set1_epi32(tileOriginY + TILE_HEIGHT), maxYIntOfTriangles), _mm256_set1_epi32(0)));

			float zPixelDxOfTriangles, zPixelDyOfTriangles;
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

			const float bbMinXV0 = (int)((float)tileOriginX + 0.5f) - vertexPoint3X;
			const float bbMinYV0 = (int)((float)tileOriginY + 0.5f) - vertexPoint3Y;

			// depth value at tile origin ( 0, 0 )
			const float depthValueAtTileOriginPoint = (zPixelDxOfTriangles * bbMinXV0) + (zPixelDyOfTriangles * bbMinYV0) + vertexPoint3Z;
			
			const culling::M256I& leftFaceEvent = leftFaceEventOfTriangles;
			const culling::M256I& rightFaceEvent = rightFaceEventOfTriangles;

			const float zPixelDx = zPixelDxOfTriangles;
			const float zPixelDy = zPixelDyOfTriangles;

			//const culling::M256F zTriMax = _mm256_set1_ps((reinterpret_cast<const float*>(&zMaxOfTriangle)));
			//const culling::M256F zTriMin = _mm256_set1_ps((reinterpret_cast<const float*>(&zMinOfTriangle)));

			//
			// 4 5 6 7   <-- _mm256i
			// 0 1 2 3
			// depth value at (0, 0) of subtiles
			culling::M256F zValueAtOriginPointOfSubTiles = _mm256_fmadd_ps(_mm256_set1_ps(zPixelDx), _mm256_setr_ps(0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3, 0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3),
				_mm256_fmadd_ps(_mm256_set1_ps(zPixelDy), _mm256_setr_ps(0, 0, 0, 0, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT), _mm256_set1_ps(depthValueAtTileOriginPoint)));

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
				const culling::M256I maskWhenLeftRightSlopeIsLocatedAtLeftOfSubTiles = _mm256_and_si256(_mm256_cmpeq_epi32(clampedLeftFaceEventInSubTiles, _mm256_set1_epi32(0)), _mm256_cmpeq_epi32(clampedRightFaceEventInSubTiles, _mm256_set1_epi32(0)));
				const culling::M256I maskWhenLeftRightSlopeIsLocatedAtRightOfSubTiles = _mm256_and_si256(_mm256_cmpeq_epi32(clampedLeftFaceEventInSubTiles, _mm256_set1_epi32(SUB_TILE_WIDTH)), _mm256_cmpeq_epi32(clampedRightFaceEventInSubTiles, _mm256_set1_epi32(SUB_TILE_WIDTH)));
				const culling::M256I maskWhenLeftRightSlopeIsOutOfSubTiles = _mm256_or_si256(maskWhenLeftRightSlopeIsLocatedAtLeftOfSubTiles, maskWhenLeftRightSlopeIsLocatedAtRightOfSubTiles);

				// when row is greater than max y or less than min y, row is invalid row
				const culling::M256I screenYOfRowInSubTiles = _mm256_setr_epi32((int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles), (int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles), (int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles), (int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles), (int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles + (float)SUB_TILE_HEIGHT), (int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles + SUB_TILE_HEIGHT), (int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles + SUB_TILE_HEIGHT), (int)((float)tileOriginY + 0.5f + (float)rowIndexInSubtiles + SUB_TILE_HEIGHT));
				const culling::M256I maskWhenRowIsOutOfMinMaxY = _mm256_or_si256(_mm256_cmpgt_epi32(screenYOfRowInSubTiles, _mm256_set1_epi32(static_cast<int>(maxYOfTriangle))), _mm256_cmpgt_epi32(_mm256_set1_epi32(static_cast<int>(minYOfTriangle)), screenYOfRowInSubTiles));

				maxZValueAtRowOfSubTiles = _mm256_blendv_ps(maxZValueAtRowOfSubTiles, _mm256_set1_ps((float)MIN_DEPTH_VALUE), *reinterpret_cast<const culling::M256F*>(&maskWhenLeftRightSlopeIsOutOfSubTiles));
				maxZValueAtRowOfSubTiles = _mm256_blendv_ps(maxZValueAtRowOfSubTiles, _mm256_set1_ps((float)MIN_DEPTH_VALUE), *reinterpret_cast<const culling::M256F*>(&maskWhenRowIsOutOfMinMaxY));

				subTileMaxValues = _mm256_max_ps(subTileMaxValues, maxZValueAtRowOfSubTiles);
			}

		}
		

	};
}


