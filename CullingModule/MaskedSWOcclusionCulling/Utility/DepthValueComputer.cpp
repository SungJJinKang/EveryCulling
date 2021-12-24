#include "DepthValueComputer.h"

#include "depthUtility.h"


void culling::DepthValueComputer::ComputeFloatBottomDepthValue
(
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
	const culling::M256F& vertexPoint3Z
)
{
#ifdef DEBUG_CULLING
	const culling::M256F LE_12Y = _mm256_cmp_ps(vertexPoint1Y, vertexPoint2Y, _CMP_GE_OQ);
	const culling::M256F LE_13Y = _mm256_cmp_ps(vertexPoint1Y, vertexPoint3Y, _CMP_GE_OQ);

	assert(_mm256_testc_si256(*reinterpret_cast<const culling::M256I*>(&LE_12Y), _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)) == 1);
	assert(_mm256_testc_si256(*reinterpret_cast<const culling::M256I*>(&LE_13Y), _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)) == 1);
#endif

	culling::M256F zPixelDx, zPixelDy;
	culling::depthUtility::ComputeDepthPlane
	(
		vertexPoint1X,
		vertexPoint1Y,
		vertexPoint1Z,

		vertexPoint2X,
		vertexPoint2Y,
		vertexPoint2Z,

		vertexPoint3X,
		vertexPoint3Y,
		vertexPoint3Z,

		zPixelDx,
		zPixelDy
	);

	culling::M256F bbMinXV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginX)), vertexPoint1X);
	culling::M256F bbMinYV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginY)), vertexPoint1Y);
	culling::M256F zPlaneOffset = _mm256_fmadd_ps(zPixelDx, bbMinXV0, _mm256_fmadd_ps(zPixelDy, bbMinYV0, vertexPoint1Z)); // depth value at tile origin
	culling::M256F zTileDx = _mm256_mul_ps(zPixelDx, _mm256_set1_ps((float)TILE_WIDTH));
	culling::M256F zTileDy = _mm256_mul_ps(zPixelDy, _mm256_set1_ps((float)TILE_HEIGHT));

	if (true)
	{
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_max_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDx, _mm256_set1_ps(SUB_TILE_WIDTH))));
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_max_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDy, _mm256_set1_ps(SUB_TILE_HEIGHT))));
	}
	else
	{
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_min_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDx, _mm256_set1_ps(SUB_TILE_WIDTH))));
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_min_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDy, _mm256_set1_ps(SUB_TILE_HEIGHT))));
	}

	// Compute Zmin and Zmax for the triangle (used to narrow the range for difficult tiles)
	culling::M256F zMin = _mm256_min_ps(vertexPoint1Z, _mm256_min_ps(vertexPoint2Z, vertexPoint3Z));
	culling::M256F zMax = _mm256_max_ps(vertexPoint1Z, _mm256_max_ps(vertexPoint2Z, vertexPoint3Z));
}




void culling::DepthValueComputer::ComputeFlatTopDepthValue
(
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
	const culling::M256F& vertexPoint3Z
)
{
#ifdef DEBUG_CULLING
	const culling::M256F LE_13Y = _mm256_cmp_ps(vertexPoint1Y, vertexPoint3Y, _CMP_GE_OQ);
	const culling::M256F LE_23Y = _mm256_cmp_ps(vertexPoint2Y, vertexPoint3Y, _CMP_GE_OQ);

	assert(_mm256_testc_si256(*reinterpret_cast<const culling::M256I*>(&LE_13Y), _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)) == 1);
	assert(_mm256_testc_si256(*reinterpret_cast<const culling::M256I*>(&LE_23Y), _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)) == 1);
#endif

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

	culling::M256F bbMinXV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginX)), vertexPoint3X);
	culling::M256F bbMinYV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginY)), vertexPoint3Y);
	culling::M256F zPlaneOffset = _mm256_fmadd_ps(zPixelDx, bbMinXV0, _mm256_fmadd_ps(zPixelDy, bbMinYV0, vertexPoint3Z)); // depth value at tile origin
	culling::M256F zTileDx = _mm256_mul_ps(zPixelDx, _mm256_set1_ps((float)TILE_WIDTH));
	culling::M256F zTileDy = _mm256_mul_ps(zPixelDy, _mm256_set1_ps((float)TILE_HEIGHT));

	if (true)
	{
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_max_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDx, _mm256_set1_ps(SUB_TILE_WIDTH))));
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_max_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDy, _mm256_set1_ps(SUB_TILE_HEIGHT))));
	}
	else
	{
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_min_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDx, _mm256_set1_ps(SUB_TILE_WIDTH))));
		zPlaneOffset = _mm256_add_ps(zPlaneOffset, _mm256_min_ps(_mm256_setzero_ps(), _mm256_mul_ps(zPixelDy, _mm256_set1_ps(SUB_TILE_HEIGHT))));
	}


	// Compute Zmin and Zmax for the triangle (used to narrow the range for difficult tiles)
	culling::M256F zMin = _mm256_min_ps(vertexPoint1Z, _mm256_min_ps(vertexPoint2Z, vertexPoint3Z));
	culling::M256F zMax = _mm256_max_ps(vertexPoint1Z, _mm256_max_ps(vertexPoint2Z, vertexPoint3Z));
}


