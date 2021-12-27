#include "BinTrianglesStage.h"

#include <vector>

#include "../MaskedSWOcclusionCulling.h"
#include "../SWDepthBuffer.h"

#include <Rendering/Renderer/Renderer.h>
#include <Rendering/Renderer/RendererStaticIterator.h>

FORCE_INLINE void culling::BinTrianglesStage::ConvertClipSpaceToNDCSpace
(
	culling::M256F* outClipVertexX, 
	culling::M256F* outClipVertexY, 
	culling::M256F* outClipVertexZ,
	const culling::M256F* oneDividedByW,
	std::uint32_t& triangleCullMask
)
{
	for (size_t i = 0; i < 3; i++)
	{
		//Why Do This??
		//compute 1/w in advance 

		outClipVertexX[i] = culling::M256F_MUL(outClipVertexX[i], oneDividedByW[i]);
		outClipVertexY[i] = culling::M256F_MUL(outClipVertexY[i], oneDividedByW[i]);
		outClipVertexZ[i] = culling::M256F_MUL(outClipVertexZ[i], oneDividedByW[i]);

		//This code is useless
		//outClipVertexW[i] = culling::M256F_MUL(outClipVertexW[i], outClipVertexW[i]);
	}


	
	/*
	for (size_t i = 0 ; i < 8; i++)
	{
		math::Vector3 vertexA{ (reinterpret_cast<const float*>(outClipVertexX + 0))[i], (reinterpret_cast<const float*>(outClipVertexY + 0))[i], (reinterpret_cast<const float*>(outClipVertexZ + 0))[i] };
		math::Vector3 vertexB{ (reinterpret_cast<const float*>(outClipVertexX + 1))[i], (reinterpret_cast<const float*>(outClipVertexY + 1))[i], (reinterpret_cast<const float*>(outClipVertexZ + 1))[i] };
		math::Vector3 vertexC{ (reinterpret_cast<const float*>(outClipVertexX + 2))[i], (reinterpret_cast<const float*>(outClipVertexY + 2))[i], (reinterpret_cast<const float*>(outClipVertexZ + 2))[i] };

		dooms::graphics::DebugDrawer::GetSingleton()->DebugDraw2DTriangle(vertexA, vertexB, vertexC, eColor::Green);
	}
	*/

}

#define CONVERT_TO_M256I(_M256F) *reinterpret_cast<const culling::M256I*>(&_M256F)

FORCE_INLINE void culling::BinTrianglesStage::FrustumCulling
(
	const culling::M256F* const clipspaceVertexX,
	const culling::M256F* const clipspaceVertexY,
	const culling::M256F* const clipspaceVertexZ,
	const culling::M256F* const clipspaceVertexW,
	std::uint32_t& triangleCullMask
)
{
	const culling::M256F pointANdcW = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexW[0]);
	const culling::M256F pointBNdcW = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexW[1]);
	const culling::M256F pointCNdcW = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexW[2]);

	const culling::M256F pointANdcX = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexX[0]), pointANdcW, _CMP_LE_OQ); // make positive values ( https://stackoverflow.com/questions/23847377/how-does-this-function-compute-the-absolute-value-of-a-float-through-a-not-and-a )
	const culling::M256F pointBNdcX = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexX[1]), pointBNdcW, _CMP_LE_OQ);
	const culling::M256F pointCNdcX = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexX[2]), pointCNdcW, _CMP_LE_OQ);
	const culling::M256F pointANdcY = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexY[0]), pointANdcW, _CMP_LE_OQ);
	const culling::M256F pointBNdcY = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexY[1]), pointBNdcW, _CMP_LE_OQ);
	const culling::M256F pointCNdcY = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexY[2]), pointCNdcW, _CMP_LE_OQ);
	const culling::M256F pointANdcZ = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexZ[0]), pointANdcW, _CMP_LE_OQ);
	const culling::M256F pointBNdcZ = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexZ[1]), pointBNdcW, _CMP_LE_OQ);
	const culling::M256F pointCNdcZ = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexZ[2]), pointCNdcW, _CMP_LE_OQ);
	
	const culling::M256I pointAInFrustum = _mm256_and_si256(_mm256_and_si256(*reinterpret_cast<const culling::M256I*>(&pointANdcX), *reinterpret_cast<const culling::M256I*>(&pointANdcY)), *reinterpret_cast<const culling::M256I*>(&pointANdcZ));
	const culling::M256I pointBInFrustum = _mm256_and_si256(_mm256_and_si256(*reinterpret_cast<const culling::M256I*>(&pointBNdcX), *reinterpret_cast<const culling::M256I*>(&pointBNdcY)), *reinterpret_cast<const culling::M256I*>(&pointBNdcZ));
	const culling::M256I pointCInFrustum = _mm256_and_si256(_mm256_and_si256(*reinterpret_cast<const culling::M256I*>(&pointCNdcX), *reinterpret_cast<const culling::M256I*>(&pointCNdcY)), *reinterpret_cast<const culling::M256I*>(&pointCNdcZ));

	const culling::M256I verticesInFrustum = _mm256_or_si256(_mm256_or_si256(*reinterpret_cast<const culling::M256I*>(&pointAInFrustum), *reinterpret_cast<const culling::M256I*>(&pointBInFrustum)), *reinterpret_cast<const culling::M256I*>(&pointCInFrustum));

	triangleCullMask &= _mm256_movemask_ps(*reinterpret_cast<const culling::M256F*>(&verticesInFrustum));
}

FORCE_INLINE void culling::BinTrianglesStage::ConvertNDCSpaceToScreenPixelSpace
(
	const culling::M256F* ndcSpaceVertexX, 
	const culling::M256F* ndcSpaceVertexY,
	culling::M256F* outScreenPixelSpaceX, 
	culling::M256F* outScreenPixelSpaceY, 
	std::uint32_t& triangleCullMask
)
{
	for (size_t i = 0; i < 3; i++)
	{
		//Convert NDC Space Coordinates To Screen Space Coordinates 
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
		outScreenPixelSpaceX[i] = culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexX[i], _mm256_set1_ps(1.0f)), mMaskedOcclusionCulling->mDepthBuffer.mResolution.mReplicatedScreenHalfWidth);
		outScreenPixelSpaceY[i] = culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexY[i], _mm256_set1_ps(1.0f)), mMaskedOcclusionCulling->mDepthBuffer.mResolution.mReplicatedScreenHalfHeight);
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
		outScreenPixelSpaceX[i] = culling::M256F_MUL(ndcSpaceVertexX[i], mDepthBuffer.mResolution.mReplicatedScreenWidth);
		outScreenPixelSpaceY[i] = culling::M256F_MUL(ndcSpaceVertexY[i], mDepthBuffer.mResolution.mReplicatedScreenHeight);
#else 
		assert(0); //NEVER HAPPEN
#endif



		/* Never snap screen pixel at here.
		outScreenPixelSpaceX[i] = _mm256_floor_ps(outScreenPixelSpaceX[i]);
		outScreenPixelSpaceY[i] = _mm256_floor_ps(outScreenPixelSpaceY[i]);

		outScreenPixelSpaceX[i] = culling::M256F_ADD(outScreenPixelSpaceX[i], _mm256_set1_ps(0.5f));
		outScreenPixelSpaceY[i] = culling::M256F_ADD(outScreenPixelSpaceY[i], _mm256_set1_ps(0.5f));
		*/
		
	}

}

FORCE_INLINE void culling::BinTrianglesStage::TransformVertexsToClipSpace
(
	culling::M256F* outClipVertexX, 
	culling::M256F* outClipVertexY, 
	culling::M256F* outClipVertexZ, 
	culling::M256F* outClipVertexW, 
	const float* const toClipspaceMatrix, 
	std::uint32_t& triangleCullMask
)
{
	if (toClipspaceMatrix != nullptr)
	{
		//Actually triangleCullMask should be considered.
		//But Just Compute it all.
		//TODO : Consider Trimask
		for (size_t i = 0; i < 3; ++i)
		{
			const culling::M256F tmpX = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[0]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[4]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
			const culling::M256F tmpY = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[1]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[5]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
			const culling::M256F tmpZ = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[2]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[6]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));
			const culling::M256F tmpW = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[3]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[7]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[11]), _mm256_set1_ps(toClipspaceMatrix[15]))));

			outClipVertexX[i] = tmpX;
			outClipVertexY[i] = tmpY;
			outClipVertexZ[i] = tmpZ;
			outClipVertexW[i] = tmpW;

		}
	}
}

FORCE_INLINE void culling::BinTrianglesStage::BackfaceCulling
(
	culling::M256F* const screenPixelX,
	culling::M256F* const screenPixelY,
	culling::M256F* const ndcSpaceVertexZ,
	std::uint32_t& triangleCullMask
)
{
	culling::M256F triArea1 = _mm256_mul_ps(_mm256_sub_ps(screenPixelX[1], screenPixelX[0]), _mm256_sub_ps(screenPixelY[2], screenPixelY[0]));
	culling::M256F triArea2 = _mm256_mul_ps(_mm256_sub_ps(screenPixelX[0], screenPixelX[2]), _mm256_sub_ps(screenPixelY[0], screenPixelY[1]));
	culling::M256F triArea = _mm256_sub_ps(triArea1, triArea2);
	culling::M256F ccwMask = _mm256_cmp_ps(triArea, _mm256_setzero_ps(), _CMP_GT_OQ);
	
	// Return a lane mask with all front faces set
	triangleCullMask &= _mm256_movemask_ps(ccwMask);
}

void culling::BinTrianglesStage::ComputeBinBoundingBox
(
	const culling::M256F* screenPixelX, 
	const culling::M256F* screenPixelY, 
	culling::M256I& outBinBoundingBoxMinX, 
	culling::M256I& outBinBoundingBoxMinY, 
	culling::M256I& outBinBoundingBoxMaxX, 
	culling::M256I& outBinBoundingBoxMaxY
)
{
	culling::M256I minScreenPixelX, minScreenPixelY, maxScreenPixelX, maxScreenPixelY;

	//A grid square, including its (x, y) window coordinates, z (depth), and associated data which may be added by fragment shaders, is called a fragment. A
	//fragment is located by its lower left corner, which lies on integer grid coordinates.
	//Rasterization operations also refer to a fragment��s center, which is offset by ( 1/2, 1/2 )
	//from its lower left corner(and so lies on half - integer coordinates).

	static const culling::M256I WIDTH_MASK = _mm256_set1_epi32(~(TILE_WIDTH - 1));
	static const culling::M256I HEIGHT_MASK = _mm256_set1_epi32(~(TILE_HEIGHT - 1));

	minScreenPixelX = _mm256_cvttps_epi32( _mm256_floor_ps(_mm256_min_ps(screenPixelX[0], _mm256_min_ps(screenPixelX[1], screenPixelX[2]))) );
	minScreenPixelY = _mm256_cvttps_epi32( _mm256_floor_ps(_mm256_min_ps(screenPixelY[0], _mm256_min_ps(screenPixelY[1], screenPixelY[2]))) );
	maxScreenPixelX = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_max_ps(screenPixelX[0], _mm256_max_ps(screenPixelX[1], screenPixelX[2]))) );
	maxScreenPixelY = _mm256_cvttps_epi32(_mm256_floor_ps(_mm256_max_ps(screenPixelY[0], _mm256_max_ps(screenPixelY[1], screenPixelY[2]))) );

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
	
	outBinBoundingBoxMinX = _mm256_min_epi32(_mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRightTopTileOrginX), _mm256_max_epi32(outBinBoundingBoxMinX, _mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mLeftBottomTileOrginX)));
	outBinBoundingBoxMinY = _mm256_min_epi32(_mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRightTopTileOrginY), _mm256_max_epi32(outBinBoundingBoxMinY, _mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mLeftBottomTileOrginY)));
	outBinBoundingBoxMaxX = _mm256_max_epi32(_mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mLeftBottomTileOrginX), _mm256_min_epi32(outBinBoundingBoxMaxX, _mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRightTopTileOrginX)));
	outBinBoundingBoxMaxY = _mm256_max_epi32(_mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mLeftBottomTileOrginY), _mm256_min_epi32(outBinBoundingBoxMaxY, _mm256_set1_epi32(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRightTopTileOrginY)));

	assert(_mm256_testc_si256(_mm256_cmpgt_epi32(outBinBoundingBoxMinX, outBinBoundingBoxMaxX), _mm256_set1_epi64x(0x0000000000000000)) == 1) ;
	assert(_mm256_testc_si256(_mm256_cmpgt_epi32(outBinBoundingBoxMinY, outBinBoundingBoxMaxY), _mm256_set1_epi64x(0x0000000000000000)) == 1) ;

}


FORCE_INLINE void culling::BinTrianglesStage::PassTrianglesToTileBin
(
	const culling::M256F* screenPixelPosX,
	const culling::M256F* screenPixelPosY,
	const culling::M256F* ndcSpaceVertexZ,
	std::uint32_t& triangleCullMask, 
	const size_t triangleCountPerLoop,
	const culling::M256I& outBinBoundingBoxMinX, 
	const culling::M256I& outBinBoundingBoxMinY,
	const culling::M256I& outBinBoundingBoxMaxX,
	const culling::M256I& outBinBoundingBoxMaxY
)
{
	for (size_t triangleIndex = 0; triangleIndex < triangleCountPerLoop; triangleIndex++)
	{
		if ((triangleCullMask & (1 << triangleIndex)) != 0x00000000)
		{
			const int intersectingMinBoxX = (reinterpret_cast<const int*>(&outBinBoundingBoxMinX))[triangleIndex]; // this is screen space coordinate
			const int intersectingMinBoxY = (reinterpret_cast<const int*>(&outBinBoundingBoxMinY))[triangleIndex];
			const int intersectingMaxBoxX = (reinterpret_cast<const int*>(&outBinBoundingBoxMaxX))[triangleIndex];
			const int intersectingMaxBoxY = (reinterpret_cast<const int*>(&outBinBoundingBoxMaxY))[triangleIndex];

			assert(intersectingMinBoxX <= intersectingMaxBoxX);
			assert(intersectingMinBoxY <= intersectingMaxBoxY);

			const int startBoxIndexX = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMinBoxX / TILE_WIDTH);
			const int startBoxIndexY = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMinBoxY / TILE_HEIGHT);
			const int endBoxIndexX = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMaxBoxX / TILE_WIDTH);
			const int endBoxIndexY = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMaxBoxY / TILE_HEIGHT);

			assert(startBoxIndexX >= 0 && startBoxIndexX < (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(startBoxIndexY >= 0 && startBoxIndexY < (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));
			
			assert(endBoxIndexX >= 0 && endBoxIndexX <= (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(endBoxIndexY >= 0 && endBoxIndexY <= (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));

			for (size_t y = startBoxIndexY; y <= endBoxIndexY; y++)
			{
				for (size_t x = startBoxIndexX; x <= endBoxIndexX; x++)
				{
					Tile* const targetTile = mMaskedOcclusionCulling->mDepthBuffer.GetTile(y, x);

					const size_t triListIndex = targetTile->mBinnedTriangles.mCurrentTriangleCount++;

					//assert(triListIndex < BIN_TRIANGLE_CAPACITY_PER_TILE);
					if(triListIndex >= BIN_TRIANGLE_CAPACITY_PER_TILE)
					{
						return;
					}

					for(size_t pointIndex = 0 ; pointIndex < 3 ; pointIndex++)
					{
						targetTile->mBinnedTriangles.VertexX[pointIndex][triListIndex] = (reinterpret_cast<const float*>(screenPixelPosX + pointIndex))[triangleIndex];
						targetTile->mBinnedTriangles.VertexY[pointIndex][triListIndex] = (reinterpret_cast<const float*>(screenPixelPosY + pointIndex))[triangleIndex];
						targetTile->mBinnedTriangles.VertexZ[pointIndex][triListIndex] = (reinterpret_cast<const float*>(ndcSpaceVertexZ + pointIndex))[triangleIndex];
					}
					
				}
			}
		}
	}
}




FORCE_INLINE void culling::BinTrianglesStage::GatherVertices
(
	const float* const vertices,
	const size_t verticeCount,
	const std::uint32_t* const vertexIndices, 
	const size_t indiceCount, 
	const size_t currentIndiceIndex, 
	const size_t vertexStrideByte, 
	const size_t fetchTriangleCount,
	culling::M256F* outVerticesX, 
	culling::M256F* outVerticesY, 
	culling::M256F* outVerticesZ, 
	std::uint32_t& triangleCullMask
)
{
	assert(indiceCount % 3 == 0);
	assert(currentIndiceIndex % 3 == 0);
	assert(indiceCount != 0); // TODO : implement gatherVertices when there is no indiceCount
	
	//Gather Indices
	const std::uint32_t* currentVertexIndices = vertexIndices + currentIndiceIndex;
	const culling::M256I indiceIndexs = _mm256_set_epi32(21, 18, 15, 12, 9, 6, 3, 0);
	static const culling::M256I SIMD_LANE_MASK[9] = {
		_mm256_setr_epi32(0,  0,  0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0,  0,  0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0,  0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, ~0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0)
	};

	// Compute per-lane index list offset that guards against out of bounds memory accesses
	const culling::M256I safeIndiceIndexs = _mm256_and_si256(indiceIndexs, SIMD_LANE_MASK[fetchTriangleCount]);

	culling::M256I m256i_indices[3];
	//If stride is 7
	//Current Value 
	//m256i_indices[0] : 0 ( first vertex index ), 3, 6,  9, 12, 15, 18, 21
	//m256i_indices[1] : 1 ( second vertex index ), 4, 7, 10, 13, 16, 19, 22
	//m256i_indices[2] : 2, 5, 8, 11, 14, 17, 20, 23
	//Point1 indices of Triangles
	m256i_indices[0] = _mm256_i32gather_epi32(reinterpret_cast<const INT32*>(currentVertexIndices + 0), safeIndiceIndexs, 4); // why 4? -> vertexIndices is std::uint32_t( 4byte )
	//Point2 indices of Triangles
	m256i_indices[1] = _mm256_i32gather_epi32(reinterpret_cast<const INT32*>(currentVertexIndices + 1), safeIndiceIndexs, 4);
	//Point3 indices of Triangles
	m256i_indices[2] = _mm256_i32gather_epi32(reinterpret_cast<const INT32*>(currentVertexIndices + 2), safeIndiceIndexs, 4);

	if(vertexStrideByte > 0)
	{
		//Consider Stride
		//If StrideByte is 28
		//m256i_indices[0] : 0 * 28, 3 * 28, 6 * 28,  9 * 28, 12 * 28, 15 * 28, 18 * 28, 21 * 28
		//m256i_indices[1] : 1 * 28, 4 * 28, 7 * 28, 10 * 28, 13 * 28, 16 * 28, 19 * 28, 22 * 28
		//m256i_indices[2] : 2 * 28, 5 * 28, 8 * 28, 11 * 28, 14 * 28, 17 * 28, 20 * 28, 23 * 28
		const culling::M256I m256i_stride = _mm256_set1_epi32(static_cast<INT32>(vertexStrideByte));
		m256i_indices[0] = _mm256_mullo_epi32(m256i_indices[0], m256i_stride);
		m256i_indices[1] = _mm256_mullo_epi32(m256i_indices[1], m256i_stride);
		m256i_indices[2] = _mm256_mullo_epi32(m256i_indices[2], m256i_stride);
	}
	

	//Gather vertexs
	//Should consider vertexStride(vertices isn't stored contiguously because generally vertex datas is stored with uv, normal datas...) 
	//And Should consider z value
	for (size_t i = 0; i < 3; i++)
	{
		outVerticesX[i] = _mm256_i32gather_ps((float*)vertices, m256i_indices[i], 1);
		outVerticesY[i] = _mm256_i32gather_ps((float*)vertices + 1, m256i_indices[i], 1);
		outVerticesZ[i] = _mm256_i32gather_ps((float*)vertices + 2, m256i_indices[i], 1);
	}
}

void culling::BinTrianglesStage::ConvertToPlatformDepth(culling::M256F* const depth)
{

#if (NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE)
	for (size_t i = 0; i < 3; i++)
	{
		//depth[i]
	}
#endif
	
}

void culling::BinTrianglesStage::BinTriangleThreadJob(const size_t cameraIndex)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex, false);

		if (nextEntityBlock != nullptr)
		{
			for (size_t entityIndex = 0; entityIndex < nextEntityBlock->mCurrentEntityCount; entityIndex++)
			{
				if
					(
						(nextEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false) &&
						(nextEntityBlock->GetIsOccluder(entityIndex, cameraIndex) == true)
						)
				{
					const culling::Mat4x4 modelToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex) * (*reinterpret_cast<const culling::Mat4x4*>(nextEntityBlock->GetModelMatrix(entityIndex)));
					const VertexData& vertexData = nextEntityBlock->mVertexDatas[entityIndex];

					BinTriangles
					(
						reinterpret_cast<const float*>(vertexData.mVertices),
						vertexData.mVerticeCount,
						vertexData.mIndices,
						vertexData.mIndiceCount,
						vertexData.mVertexStride,
						modelToClipSpaceMatrix.data()
					);
				}
			}

		}
		else
		{
			break;
		}
	}
}

void culling::BinTrianglesStage::BinTriangleThreadJobByObjectOrder(const size_t cameraIndex)
{
	for (UINT32 layerIndex = 0; layerIndex < MAX_LAYER_COUNT; layerIndex++)
	{
		const std::vector<dooms::Renderer*>& renderersInLayer = dooms::RendererComponentStaticIterator::GetSingleton()->GetWorkingRendererInLayer(cameraIndex, layerIndex);

		for(INT64 rendererIndex = 0 ; rendererIndex < renderersInLayer.size() ; rendererIndex++)
		{
			dooms::Renderer* const renderer = renderersInLayer[rendererIndex];
			if
			(
				dooms::IsValid(renderer) == true &&
				renderer->mCullingEntityBlockViewer.GetIsCulled(cameraIndex) == false &&
				renderer->mCullingEntityBlockViewer.GetIsOccluder(cameraIndex) == true
			)
			{
				
				const culling::Mat4x4 modelToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex) * (*reinterpret_cast<const culling::Mat4x4*>(renderer->mCullingEntityBlockViewer.GetModelMatrix()));
				const VertexData& vertexData = renderer->mCullingEntityBlockViewer.GetVertexData();

				BinTriangles
				(
					reinterpret_cast<const float*>(vertexData.mVertices),
					vertexData.mVerticeCount,
					vertexData.mIndices,
					vertexData.mIndiceCount,
					vertexData.mVertexStride,
					modelToClipSpaceMatrix.data()
				);
			}
		}
	}
}

culling::BinTrianglesStage::BinTrianglesStage(MaskedSWOcclusionCulling* mMOcclusionCulling)
	: MaskedSWOcclusionCullingStage{ mMOcclusionCulling }
{
}

void culling::BinTrianglesStage::ResetCullingModule()
{
	MaskedSWOcclusionCullingStage::ResetCullingModule();

	IsWorkingByAThread.store(false, std::memory_order_seq_cst);
}

void culling::BinTrianglesStage::CullBlockEntityJob(const size_t cameraIndex)
{
	const bool canWorkable = !(IsWorkingByAThread.exchange(true, std::memory_order_seq_cst));

	// Only one thread can work on this stage
	if(canWorkable == true)
	{
#ifdef FETCH_OBJECT_SORT_FROM_DOOMS_ENGINE_IN_BIN_TRIANGLE_STAGE
		BinTriangleThreadJobByObjectOrder(cameraIndex);
#else
		BinTriangleThreadJob(cameraIndex);
#endif
			
	}
}

FORCE_INLINE void culling::BinTrianglesStage::BinTriangles
(
	const float* const vertices, 
	const size_t verticeCount,
	const std::uint32_t* const vertexIndices, 
	const size_t indiceCount, 
	const size_t vertexStrideByte, 
	const float* const modelToClipspaceMatrix
)
{
	static constexpr std::int32_t triangleCountPerLoop = 8;

	std::int32_t currentIndiceIndex = -(triangleCountPerLoop * 3);

	assert(indiceCount != 0); // check GatherVertices function

	while (currentIndiceIndex < (std::int32_t)indiceCount)
	{
		currentIndiceIndex += (triangleCountPerLoop * 3);

		if(currentIndiceIndex >= (std::int32_t)indiceCount)
		{
			break;
		}
		const size_t fetchTriangleCount = MIN(8, (indiceCount - currentIndiceIndex) / 3);
		assert(fetchTriangleCount != 0);

		// First 4 bits show if traingle is valid
		// Current Value : 00000000 00000000 00000000 11111111
		std::uint32_t triangleCullMask = (1 << fetchTriangleCount) - 1;


		//Why Size of array is 3?
		//A culling::M256F can have 8 floating-point
		//A TwoDTriangle have 3 point
		//So If you have just one culling::M256F variable, a floating-point is unused.
		//Not to make unused space, 12 floating point is required per axis
		// culling::M256F * 3 -> 8 TwoDTriangle with no unused space

		//We don't need z value in Binning stage!!!
		// Triangle's First Vertex X is in ndcSpaceVertexX[0][0]
		// Triangle's Second Vertex X is in ndcSpaceVertexX[0][1]
		// Triangle's Third Vertex X is in ndcSpaceVertexX[0][2]
		culling::M256F ndcSpaceVertexX[3], ndcSpaceVertexY[3], ndcSpaceVertexZ[3], oneDividedByW[3];


		//TODO : Why use 1/w for depth test, not z -> Check this : https://github.com/GameTechDev/MaskedOcclusionCulling/issues/23

		//Gather Vertex with indice
		//WE ARRIVE AT MODEL SPACE COORDINATE!
		GatherVertices(vertices, verticeCount, vertexIndices, indiceCount, currentIndiceIndex, vertexStrideByte, fetchTriangleCount, ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, triangleCullMask);

		if (triangleCullMask == 0x00000000)
		{
			continue;
		}

		//Convert Model space Vertex To Clip space Vertex
		//WE ARRIVE AT CLIP SPACE COORDINATE. W IS NOT 1
		TransformVertexsToClipSpace(ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, oneDividedByW, modelToClipspaceMatrix, triangleCullMask);

		

		FrustumCulling(ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, oneDividedByW, triangleCullMask);

		if (triangleCullMask == 0x00000000)
		{
			continue;
		}

		for (size_t i = 0; i < 3; i++)
		{
			//oneDividedByW finally become oneDividedByW
			oneDividedByW[i] = culling::M256F_DIV(_mm256_set1_ps(1.0f), oneDividedByW[i]);
		}

		//WE ARRIVE AT NDC SPACE COORDINATE. 
		//If you use Opengl, Vertexs have value from -1 to 1
		//if you use DirectX, Vertexs have value from 0 to 1 
		//W BECOME USELESS, IGNORE IT
		ConvertClipSpaceToNDCSpace(ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, oneDividedByW, triangleCullMask);

		// we don't linearize depth value
		ConvertToPlatformDepth(ndcSpaceVertexZ);

		// TODO : Set triangleCullMask about NDC x, y, z is in -1 ~ 1

		//ScreenPixelPos : 0 ~ mDepthBuffer.Width, Height
		culling::M256F screenPixelPosX[3], screenPixelPosY[3];
		ConvertNDCSpaceToScreenPixelSpace(ndcSpaceVertexX, ndcSpaceVertexY, screenPixelPosX, screenPixelPosY, triangleCullMask);
		

		//BackFace Cull
		// 
		BackfaceCulling(screenPixelPosX, screenPixelPosY, ndcSpaceVertexZ, triangleCullMask);

		//Do not Sort Triangle in binning stage
		//because Culled triangles is also sorted
		//Sort triangle in drawing depth stage/
		//In that stage, all triangles is valid

		if (triangleCullMask == 0x00000000)
		{
			continue;
		}

		// TODO : Early split triangle at here
		
		

		culling::M256I outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY;
		//Bin Triangles to tiles

		//Compute Bin Bounding Box
		//Get Intersecting Bin List
		ComputeBinBoundingBox
		(
			screenPixelPosX, 
			screenPixelPosY, 
			outBinBoundingBoxMinX, 
			outBinBoundingBoxMinY, 
			outBinBoundingBoxMaxX, 
			outBinBoundingBoxMaxY
		);


		PassTrianglesToTileBin
		(
			screenPixelPosX,
			screenPixelPosY,
			ndcSpaceVertexZ,
			triangleCullMask,
			triangleCountPerLoop,
			outBinBoundingBoxMinX,
			outBinBoundingBoxMinY,
			outBinBoundingBoxMaxX,
			outBinBoundingBoxMaxY
		);

		
	}
}

