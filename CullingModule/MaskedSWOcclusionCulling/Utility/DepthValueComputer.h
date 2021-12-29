#pragma once

#include "../../../EveryCullingCore.h"

#include "depthUtility.h"

namespace culling
{
	namespace DepthValueComputer
	{
		enum class eDepthType
		{
			MinDepth,
			MaxDepth
		};
		

		FORCE_INLINE extern void ComputeFlatTriangleMaxDepthValue
		(
			const size_t triangleCount,
			const eDepthType targetDepthType,
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

			const culling::M256I* const leftFaceEvent, // eight _mm256i
			const culling::M256I* const rightFaceEvent, // eight _mm256i

			const std::uint32_t triangleMask
		)
		{
			culling::M256F zPixelDx, zPixelDy;
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

				zPixelDx,
				zPixelDy
			);

			const culling::M256F bbMinXV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginX)), vertexPoint3X);
			const culling::M256F bbMinYV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginY)), vertexPoint3Y);
			culling::M256F zPlaneOffset = _mm256_fmadd_ps(zPixelDx, bbMinXV0, _mm256_fmadd_ps(zPixelDy, bbMinYV0, vertexPoint3Z)); // depth value at tile origin + ( 0.5f, 0.5f )
			const culling::M256F zTileDx = _mm256_mul_ps(zPixelDx, _mm256_set1_ps((float)TILE_WIDTH));
			const culling::M256F zTileDy = _mm256_mul_ps(zPixelDy, _mm256_set1_ps((float)TILE_HEIGHT));

			zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_max_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDx, _mm256_set1_ps(SUB_TILE_WIDTH))));
			zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_max_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDy, _mm256_set1_ps(SUB_TILE_HEIGHT))));
			

			// Compute Zmin and Zmax for the triangle (used to narrow the range for difficult tiles)
			const culling::M256F zMinOfTriangle = _mm256_min_ps(vertexPoint1Z, _mm256_min_ps(vertexPoint2Z, vertexPoint3Z));
			const culling::M256F zMaxOfTriangle = _mm256_max_ps(vertexPoint1Z, _mm256_max_ps(vertexPoint2Z, vertexPoint3Z));


			for (size_t triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
			{
				if ((triangleMask & (1 << triangleIndex)) != 0x00000000)
				{
					const culling::M256F zTriMax = _mm256_set1_ps((reinterpret_cast<const float*>(&zMaxOfTriangle))[triangleIndex]);
					const culling::M256F zTriMin = _mm256_set1_ps((reinterpret_cast<const float*>(&zMinOfTriangle))[triangleIndex]);

					// depth value at subtiles
					culling::M256F z0 = _mm256_fmadd_ps(_mm256_set1_ps((reinterpret_cast<const float*>(&zPixelDx))[triangleIndex]), _mm256_setr_ps(0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3, 0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3),
						_mm256_fmadd_ps(_mm256_set1_ps((reinterpret_cast<const float*>(&zPixelDy))[triangleIndex]), _mm256_setr_ps(0, 0, 0, 0, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT), _mm256_set1_ps((reinterpret_cast<const float*>(&zPlaneOffset))[triangleIndex])));

					z0 = _mm256_max_ps(_mm256_min_ps(z0, zTriMax), zTriMin);
					subTileMaxValues[triangleIndex] = z0;


					// TODO : ScanLine DepthValue based on left, right event.
					//leftFaceEvent[triangleIndex]
					//rightFaceEvent[triangleIndex]
				}
				

			}

		}
		

	};
}


