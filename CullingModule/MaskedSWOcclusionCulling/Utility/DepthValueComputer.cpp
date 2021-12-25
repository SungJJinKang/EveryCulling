#include "DepthValueComputer.h"

#include "depthUtility.h"


void culling::DepthValueComputer::ComputeFlatBottomDepthValue
(
	const size_t triangleCount,
	const eDepthType targetDepthType,
	culling::M256F* const subTileMaxValues,
	const std::uint32_t tileOriginX, // 32x8 tile
	const std::uint32_t tileOriginY, // 32x8 tile

	const culling::M256F& vertexPoint1X, // top point
	const culling::M256F& vertexPoint1Y,
	const culling::M256F& vertexPoint1Z,

	const culling::M256F& vertexPoint2X, // bottom flat left point 
	const culling::M256F& vertexPoint2Y,
	const culling::M256F& vertexPoint2Z,

	const culling::M256F& vertexPoint3X, // bottom flat right point
	const culling::M256F& vertexPoint3Y,
	const culling::M256F& vertexPoint3Z,

	const culling::M256I* const leftFaceEvent,
	const culling::M256I* const rightFaceEvent
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

	const culling::M256F bbMinXV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginX + 0.5f)), vertexPoint1X);
	const culling::M256F bbMinYV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginY + 0.5f)), vertexPoint1Y);
	culling::M256F zPlaneOffset = _mm256_fmadd_ps(zPixelDx, bbMinXV0, _mm256_fmadd_ps(zPixelDy, bbMinYV0, vertexPoint1Z)); // depth value at tile origin
	const culling::M256F zTileDx = _mm256_mul_ps(zPixelDx, _mm256_set1_ps((float)TILE_WIDTH)); // Z value variance to tile index x
	const culling::M256F zTileDy = _mm256_mul_ps(zPixelDy, _mm256_set1_ps((float)TILE_HEIGHT)); // Z value variance to tile index y

	if (targetDepthType == eDepthType::MaxDepth)
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
	const culling::M256F zMinOfTriangle = _mm256_min_ps(vertexPoint1Z, _mm256_min_ps(vertexPoint2Z, vertexPoint3Z));
	const culling::M256F zMaxOfTriangle = _mm256_max_ps(vertexPoint1Z, _mm256_max_ps(vertexPoint2Z, vertexPoint3Z));


	for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
	{
		const culling::M256F zTriMax = _mm256_set1_ps((reinterpret_cast<const float*>(&zMaxOfTriangle))[triIndex]);
		const culling::M256F zTriMin = _mm256_set1_ps((reinterpret_cast<const float*>(&zMinOfTriangle))[triIndex]);

		// depth value at subtiles
		culling::M256F z0 = _mm256_fmadd_ps(_mm256_set1_ps((reinterpret_cast<const float*>(&zPixelDx))[triIndex]), _mm256_setr_ps(0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3, 0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3),
			_mm256_fmadd_ps(_mm256_set1_ps((reinterpret_cast<const float*>(&zPixelDy))[triIndex]), _mm256_setr_ps(0, 0, 0, 0, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT), _mm256_set1_ps((reinterpret_cast<const float*>(&zPlaneOffset))[triIndex])));

		z0 = _mm256_max_ps(_mm256_min_ps(z0, zTriMax), zTriMin);
		subTileMaxValues[triIndex] = z0;
		//const float zx = (reinterpret_cast<const float*>(&zTileDx))[triIndex];
		//const float zy = (reinterpret_cast<const float*>(&zTileDy))[triIndex];

		// Get dimension of bounding box bottom, mid & top segments
		//int bbWidth = (reinterpret_cast<const float*>(&bbTileSizeX))[triIndex];
		//int bbHeight = (reinterpret_cast<const float*>(&bbTileSizeY))[triIndex];
		//int tileRowIdx = (reinterpret_cast<const float*>(&bbBottomIdx))[triIndex];
	}
}




void culling::DepthValueComputer::ComputeFlatTopDepthValue
(
	const size_t triangleCount,
	const eDepthType targetDepthType,
	culling::M256F* const subTileMaxValues,
	const std::uint32_t tileOriginX, // 32x8 tile
	const std::uint32_t tileOriginY, // 32x8 tile

	const culling::M256F& vertexPoint1X, // top flat left point 
	const culling::M256F& vertexPoint1Y,
	const culling::M256F& vertexPoint1Z,

	const culling::M256F& vertexPoint2X, // top flat right point 
	const culling::M256F& vertexPoint2Y,
	const culling::M256F& vertexPoint2Z,

	const culling::M256F& vertexPoint3X, // bottom point
	const culling::M256F& vertexPoint3Y,
	const culling::M256F& vertexPoint3Z,

	const culling::M256I* const leftFaceEvent,
	const culling::M256I* const rightFaceEvent
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

	const culling::M256F bbMinXV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginX)), vertexPoint3X);
	const culling::M256F bbMinYV0 = _mm256_sub_ps(_mm256_cvtepi32_ps(_mm256_set1_epi32(tileOriginY)), vertexPoint3Y);
	culling::M256F zPlaneOffset = _mm256_fmadd_ps(zPixelDx, bbMinXV0, _mm256_fmadd_ps(zPixelDy, bbMinYV0, vertexPoint3Z)); // depth value at tile origin + ( 0.5f, 0.5f )
	const culling::M256F zTileDx = _mm256_mul_ps(zPixelDx, _mm256_set1_ps((float)TILE_WIDTH));
	const culling::M256F zTileDy = _mm256_mul_ps(zPixelDy, _mm256_set1_ps((float)TILE_HEIGHT));
	
	if (targetDepthType == eDepthType::MaxDepth)
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
	const culling::M256F zMinOfTriangle = _mm256_min_ps(vertexPoint1Z, _mm256_min_ps(vertexPoint2Z, vertexPoint3Z));
	const culling::M256F zMaxOfTriangle = _mm256_max_ps(vertexPoint1Z, _mm256_max_ps(vertexPoint2Z, vertexPoint3Z));


	for(size_t triIndex = 0 ; triIndex < triangleCount ; triIndex++)
	{
		const culling::M256F zTriMax = _mm256_set1_ps((reinterpret_cast<const float*>(&zMaxOfTriangle))[triIndex]);
		const culling::M256F zTriMin = _mm256_set1_ps((reinterpret_cast<const float*>(&zMinOfTriangle))[triIndex]);

		// depth value at subtiles
		culling::M256F z0 = _mm256_fmadd_ps(_mm256_set1_ps((reinterpret_cast<const float*>(&zPixelDx))[triIndex]), _mm256_setr_ps(0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3, 0, SUB_TILE_WIDTH, SUB_TILE_WIDTH * 2, SUB_TILE_WIDTH * 3),
			_mm256_fmadd_ps(_mm256_set1_ps((reinterpret_cast<const float*>(&zPixelDy))[triIndex]), _mm256_setr_ps(0, 0, 0, 0, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT, SUB_TILE_HEIGHT), _mm256_set1_ps((reinterpret_cast<const float*>(&zPlaneOffset))[triIndex])));

		z0 = _mm256_max_ps(_mm256_min_ps(z0, zTriMax), zTriMin);
		subTileMaxValues[triIndex] = z0;
		//const float zx = (reinterpret_cast<const float*>(&zTileDx))[triIndex];
		//const float zy = (reinterpret_cast<const float*>(&zTileDy))[triIndex];

		// Get dimension of bounding box bottom, mid & top segments
		//int bbWidth = (reinterpret_cast<const float*>(&bbTileSizeX))[triIndex];
		//int bbHeight = (reinterpret_cast<const float*>(&bbTileSizeY))[triIndex];
		//int tileRowIdx = (reinterpret_cast<const float*>(&bbBottomIdx))[triIndex];
	}
	
}


