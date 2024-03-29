#include "BinTrianglesStage.h"

#include <vector>

#include "../MaskedSWOcclusionCulling.h"
#include "../SWDepthBuffer.h"
#include "../Utility/vertexTransformationHelper.h"
#include "../Utility/depthBufferTileHelper.h"


#include "../Utility/RasterizerHelper.h"

#define DEFAULT_BINNED_TRIANGLE_COUNT_PER_LOOP 8

#define CONVERT_TO_M256I(_M256F) *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&_M256F)

EVERYCULLING_FORCE_INLINE void culling::BinTrianglesStage::Clipping
(
	const culling::EVERYCULLING_M256F* const clipspaceVertexX,
	const culling::EVERYCULLING_M256F* const clipspaceVertexY,
	const culling::EVERYCULLING_M256F* const clipspaceVertexZ,
	const culling::EVERYCULLING_M256F* const clipspaceVertexW,
	std::uint32_t& triangleCullMask
)
{
	const culling::EVERYCULLING_M256F pointANdcPositiveW = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexW[0]);
	const culling::EVERYCULLING_M256F pointBNdcPositiveW = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexW[1]);
	const culling::EVERYCULLING_M256F pointCNdcPositiveW = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexW[2]);

	const culling::EVERYCULLING_M256F pointANdcX = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexX[0]), pointANdcPositiveW, _CMP_LE_OQ); // make positive values ( https://stackoverflow.com/questions/23847377/how-does-this-function-compute-the-absolute-value-of-a-float-through-a-not-and-a )
	const culling::EVERYCULLING_M256F pointBNdcX = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexX[1]), pointBNdcPositiveW, _CMP_LE_OQ);
	const culling::EVERYCULLING_M256F pointCNdcX = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexX[2]), pointCNdcPositiveW, _CMP_LE_OQ);
	const culling::EVERYCULLING_M256F pointANdcY = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexY[0]), pointANdcPositiveW, _CMP_LE_OQ);
	const culling::EVERYCULLING_M256F pointBNdcY = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexY[1]), pointBNdcPositiveW, _CMP_LE_OQ);
	const culling::EVERYCULLING_M256F pointCNdcY = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexY[2]), pointCNdcPositiveW, _CMP_LE_OQ);
	const culling::EVERYCULLING_M256F pointANdcZ = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexZ[0]), pointANdcPositiveW, _CMP_LE_OQ);
	const culling::EVERYCULLING_M256F pointBNdcZ = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexZ[1]), pointBNdcPositiveW, _CMP_LE_OQ);
	const culling::EVERYCULLING_M256F pointCNdcZ = _mm256_cmp_ps(_mm256_andnot_ps(_mm256_set1_ps(-0.0f), clipspaceVertexZ[2]), pointCNdcPositiveW, _CMP_LE_OQ);
	
	culling::EVERYCULLING_M256I pointAInFrustum = _mm256_and_si256(_mm256_and_si256(*reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointANdcX), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointANdcY)), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointANdcZ));
	culling::EVERYCULLING_M256I pointBInFrustum = _mm256_and_si256(_mm256_and_si256(*reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointBNdcX), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointBNdcY)), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointBNdcZ));
	culling::EVERYCULLING_M256I pointCInFrustum = _mm256_and_si256(_mm256_and_si256(*reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointCNdcX), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointCNdcY)), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointCNdcZ));
	
	// if All vertices of triangle is out of volume, cull the triangle
	const culling::EVERYCULLING_M256I verticesInFrustum = _mm256_or_si256(_mm256_or_si256(*reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointAInFrustum), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointBInFrustum)), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&pointCInFrustum));

	triangleCullMask &= _mm256_movemask_ps(*reinterpret_cast<const culling::EVERYCULLING_M256F*>(&verticesInFrustum));
}

EVERYCULLING_FORCE_INLINE culling::EVERYCULLING_M256F culling::BinTrianglesStage::ComputePositiveWMask
(
	const culling::EVERYCULLING_M256F* const clipspaceVertexW
)
{
	const culling::EVERYCULLING_M256F pointA_W_IsNegativeValue = _mm256_cmp_ps(clipspaceVertexW[0], _mm256_set1_ps(std::numeric_limits<float>::epsilon()), _CMP_GE_OQ);
	const culling::EVERYCULLING_M256F pointB_W_IsNegativeValue = _mm256_cmp_ps(clipspaceVertexW[1], _mm256_set1_ps(std::numeric_limits<float>::epsilon()), _CMP_GE_OQ);
	const culling::EVERYCULLING_M256F pointC_W_IsNegativeValue = _mm256_cmp_ps(clipspaceVertexW[2], _mm256_set1_ps(std::numeric_limits<float>::epsilon()), _CMP_GE_OQ);

	return _mm256_and_ps(pointA_W_IsNegativeValue, _mm256_and_ps(pointB_W_IsNegativeValue, pointC_W_IsNegativeValue));
	
}



EVERYCULLING_FORCE_INLINE void culling::BinTrianglesStage::BackfaceCulling
(
	culling::EVERYCULLING_M256F* const screenPixelX,
	culling::EVERYCULLING_M256F* const screenPixelY,
	std::uint32_t& triangleCullMask
)
{
	culling::EVERYCULLING_M256F triArea1 = _mm256_mul_ps(_mm256_sub_ps(screenPixelX[1], screenPixelX[0]), _mm256_sub_ps(screenPixelY[2], screenPixelY[0]));
	culling::EVERYCULLING_M256F triArea2 = _mm256_mul_ps(_mm256_sub_ps(screenPixelX[0], screenPixelX[2]), _mm256_sub_ps(screenPixelY[0], screenPixelY[1]));
	culling::EVERYCULLING_M256F triArea = _mm256_sub_ps(triArea1, triArea2);
	culling::EVERYCULLING_M256F ccwMask = _mm256_cmp_ps(triArea, _mm256_set1_ps(std::numeric_limits<float>::epsilon()), _CMP_GT_OQ);
	
	// Return a lane mask with all front faces set
	triangleCullMask &= _mm256_movemask_ps(ccwMask);
}



EVERYCULLING_FORCE_INLINE void culling::BinTrianglesStage::PassTrianglesToTileBin
(
	const culling::EVERYCULLING_M256F& pointAScreenPixelPosX,
	const culling::EVERYCULLING_M256F& pointAScreenPixelPosY,
	const culling::EVERYCULLING_M256F& pointANdcSpaceVertexZ,

	const culling::EVERYCULLING_M256F& pointBScreenPixelPosX,
	const culling::EVERYCULLING_M256F& pointBScreenPixelPosY,
	const culling::EVERYCULLING_M256F& pointBNdcSpaceVertexZ,

	const culling::EVERYCULLING_M256F& pointCScreenPixelPosX,
	const culling::EVERYCULLING_M256F& pointCScreenPixelPosY,
	const culling::EVERYCULLING_M256F& pointCNdcSpaceVertexZ,

	const std::uint32_t& triangleCullMask, 
	const size_t triangleCountPerLoop,
	const culling::EVERYCULLING_M256I& outBinBoundingBoxMinX, 
	const culling::EVERYCULLING_M256I& outBinBoundingBoxMinY,
	const culling::EVERYCULLING_M256I& outBinBoundingBoxMaxX,
	const culling::EVERYCULLING_M256I& outBinBoundingBoxMaxY
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

			const int startBoxIndexX = EVERYCULLING_MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMinBoxX / EVERYCULLING_TILE_WIDTH);
			const int startBoxIndexY = EVERYCULLING_MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMinBoxY / EVERYCULLING_TILE_HEIGHT);
			const int endBoxIndexX = EVERYCULLING_MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMaxBoxX / EVERYCULLING_TILE_WIDTH);
			const int endBoxIndexY = EVERYCULLING_MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMaxBoxY / EVERYCULLING_TILE_HEIGHT);

			assert(startBoxIndexX >= 0 && startBoxIndexX < (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(startBoxIndexY >= 0 && startBoxIndexY < (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));
			
			assert(endBoxIndexX >= 0 && endBoxIndexX <= (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(endBoxIndexY >= 0 && endBoxIndexY <= (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));

			for (int y = startBoxIndexY; y <= endBoxIndexY; y++)
			{
				for (int x = startBoxIndexX; x <= endBoxIndexX; x++)
				{
					Tile* const targetTile = mMaskedOcclusionCulling->mDepthBuffer.GetTile(static_cast<std::uint32_t>(y), static_cast<std::uint32_t>(x));

					//assert(targetTile->mBinnedTriangleList.GetIsBinFull() == false);

					const size_t triListIndex = targetTile->mBinnedTriangleCount++;

					if(triListIndex < BIN_TRIANGLE_CAPACITY_PER_TILE)
					{
						targetTile->mBinnedTriangleList[triListIndex].PointAVertexX = (reinterpret_cast<const float*>(&pointAScreenPixelPosX))[triangleIndex];
						targetTile->mBinnedTriangleList[triListIndex].PointAVertexY = (reinterpret_cast<const float*>(&pointAScreenPixelPosY))[triangleIndex];
						targetTile->mBinnedTriangleList[triListIndex].PointAVertexZ = (reinterpret_cast<const float*>(&pointANdcSpaceVertexZ))[triangleIndex];

						targetTile->mBinnedTriangleList[triListIndex].PointBVertexX = (reinterpret_cast<const float*>(&pointBScreenPixelPosX))[triangleIndex];
						targetTile->mBinnedTriangleList[triListIndex].PointBVertexY = (reinterpret_cast<const float*>(&pointBScreenPixelPosY))[triangleIndex];
						targetTile->mBinnedTriangleList[triListIndex].PointBVertexZ = (reinterpret_cast<const float*>(&pointBNdcSpaceVertexZ))[triangleIndex];

						targetTile->mBinnedTriangleList[triListIndex].PointCVertexX = (reinterpret_cast<const float*>(&pointCScreenPixelPosX))[triangleIndex];
						targetTile->mBinnedTriangleList[triListIndex].PointCVertexY = (reinterpret_cast<const float*>(&pointCScreenPixelPosY))[triangleIndex];
						targetTile->mBinnedTriangleList[triListIndex].PointCVertexZ = (reinterpret_cast<const float*>(&pointCNdcSpaceVertexZ))[triangleIndex];
					}
				}
			}
		}
	}
}




EVERYCULLING_FORCE_INLINE void culling::BinTrianglesStage::GatherVertices
(
	const float* const vertices,
	const size_t verticeCount,
	const std::uint32_t* const vertexIndices, 
	const size_t indiceCount, 
	const size_t currentIndiceIndex, 
	const size_t vertexStrideByte, 
	const size_t fetchTriangleCount,
	culling::EVERYCULLING_M256F* outVerticesX, 
	culling::EVERYCULLING_M256F* outVerticesY, 
	culling::EVERYCULLING_M256F* outVerticesZ
)
{
	assert(indiceCount % 3 == 0);
	assert(currentIndiceIndex % 3 == 0);
	assert(indiceCount != 0); // TODO : implement gatherVertices when there is no indiceCount
	
	//Gather Indices
	const std::uint32_t* currentVertexIndices = vertexIndices + currentIndiceIndex;
	const culling::EVERYCULLING_M256I indiceIndexs = _mm256_set_epi32(21, 18, 15, 12, 9, 6, 3, 0);
	static const culling::EVERYCULLING_M256I SIMD_LANE_MASK[9] = {
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
	const culling::EVERYCULLING_M256I safeIndiceIndexs = _mm256_and_si256(indiceIndexs, SIMD_LANE_MASK[fetchTriangleCount]);

	culling::EVERYCULLING_M256I m256i_indices[3];
	//If stride is 7
	//Current Value 
	//m256i_indices[0] : 0 ( first vertex index ), 3, 6,  9, 12, 15, 18, 21
	//m256i_indices[1] : 1 ( second vertex index ), 4, 7, 10, 13, 16, 19, 22
	//m256i_indices[2] : 2, 5, 8, 11, 14, 17, 20, 23
	//Point1 indices of Triangles
	m256i_indices[0] = _mm256_i32gather_epi32(reinterpret_cast<const int*>(currentVertexIndices + 0), safeIndiceIndexs, 4); // why 4? -> vertexIndices is std::uint32_t( 4byte )
	//Point2 indices of Triangles
	m256i_indices[1] = _mm256_i32gather_epi32(reinterpret_cast<const int*>(currentVertexIndices + 1), safeIndiceIndexs, 4);
	//Point3 indices of Triangles
	m256i_indices[2] = _mm256_i32gather_epi32(reinterpret_cast<const int*>(currentVertexIndices + 2), safeIndiceIndexs, 4);

	if(vertexStrideByte > 0)
	{
		//Consider Stride
		//If StrideByte is 28
		//m256i_indices[0] : 0 * 28, 3 * 28, 6 * 28,  9 * 28, 12 * 28, 15 * 28, 18 * 28, 21 * 28
		//m256i_indices[1] : 1 * 28, 4 * 28, 7 * 28, 10 * 28, 13 * 28, 16 * 28, 19 * 28, 22 * 28
		//m256i_indices[2] : 2 * 28, 5 * 28, 8 * 28, 11 * 28, 14 * 28, 17 * 28, 20 * 28, 23 * 28
		const culling::EVERYCULLING_M256I m256i_stride = _mm256_set1_epi32(static_cast<int>(vertexStrideByte));
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

void culling::BinTrianglesStage::ConvertToPlatformDepth(culling::EVERYCULLING_M256F* const depth)
{

#if (EVERYCULLING_NDC_RANGE == EVERYCULLING_MINUS_ONE_TO_POSITIVE_ONE)
	for (size_t i = 0; i < 3; i++)
	{
		//depth[i]
	}
#endif
	
}

/*
void culling::BinTrianglesStage::BinTriangleThreadJob(const size_t cameraIndex)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex, false);

		if (nextEntityBlock != nullptr)
		{
			for (size_t entityIndex = 0; entityIndex < nextEntityBlock->mCurrentEntityCount; entityIndex++)
			{
				const size_t binnedTriangleListIndex = mMaskedOcclusionCulling->IncreamentBinnedOccluderCountIfPossible();
				if (binnedTriangleListIndex != 0)
				{
					if
					(
						(nextEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false) &&
						(nextEntityBlock->GetIsOccluder(entityIndex) == true)
					)
					{
						const culling::Mat4x4 modelToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex) * nextEntityBlock->GetModelMatrix(entityIndex);
						const VertexData& vertexData = nextEntityBlock->mVertexDatas[entityIndex];

						BinTriangles
						(
							binnedTriangleListIndex,
							reinterpret_cast<const float*>(vertexData.mVertices),
							vertexData.mVerticeCount,
							vertexData.mIndices,
							vertexData.mIndiceCount,
							vertexData.mVertexStride,
							modelToClipSpaceMatrix.data()
						);
					}
				}
				else
				{
					return;
				}
			}

		}
		else
		{
			return;
		}
	}
}
*/

void culling::BinTrianglesStage::BinTriangleThreadJobByObjectOrder(const size_t cameraIndex)
{
	std::vector<OccluderData> sortedOccluderList = mMaskedOcclusionCulling->mOccluderListManager.GetSortedOccluderList(mCullingSystem->GetCameraWorldPosition(cameraIndex));

	std::uint64_t totalBinnedIndiceCount = 0;
	
	for (size_t entityInfoIndex = 0; entityInfoIndex < sortedOccluderList.size() && totalBinnedIndiceCount < EVERYCULLING_MAX_BINNED_INDICE_COUNT ; entityInfoIndex++)
	{
		culling::OccluderData& occluderInfo = sortedOccluderList[entityInfoIndex];

		culling::EntityBlock* const entityBlock = occluderInfo.mEntityBlock;
		const size_t entityIndexInEntityBlock = occluderInfo.mEntityIndexInEntityBlock;
		
		assert(entityBlock->GetIsCulled(entityIndexInEntityBlock, cameraIndex) == false);
		
		std::atomic<std::uint64_t>& atomic_binnedIndiceCountOfCurrentEntity = entityBlock->mVertexDatas[entityIndexInEntityBlock].mBinnedIndiceCount;

		const culling::Vec3* const vertices = entityBlock->mVertexDatas[entityIndexInEntityBlock].mVertices;
		const std::uint64_t verticeCount = entityBlock->mVertexDatas[entityIndexInEntityBlock].mVerticeCount;
		const std::uint32_t* const indices = entityBlock->mVertexDatas[entityIndexInEntityBlock].mIndices;
		const std::uint64_t totalIndiceCount = entityBlock->mVertexDatas[entityIndexInEntityBlock].mIndiceCount;
		const std::uint64_t vertexStride = entityBlock->mVertexDatas[entityIndexInEntityBlock].mVertexStride;

		std::uint64_t currentBinnedIndiceCountOfCurrentEntity = 0;

		while (totalBinnedIndiceCount + currentBinnedIndiceCountOfCurrentEntity < EVERYCULLING_MAX_BINNED_INDICE_COUNT)
		{
			static_assert((DEFAULT_BINNED_TRIANGLE_COUNT_PER_LOOP * 3) % 3 == 0);
			currentBinnedIndiceCountOfCurrentEntity = atomic_binnedIndiceCountOfCurrentEntity.fetch_add(DEFAULT_BINNED_TRIANGLE_COUNT_PER_LOOP * 3, std::memory_order_seq_cst);
			
			if (currentBinnedIndiceCountOfCurrentEntity < totalIndiceCount)
			{
				const culling::Mat4x4 modelToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex) * entityBlock->GetModelMatrix(entityIndexInEntityBlock);

				const std::uint32_t* const startIndicePtr = indices + currentBinnedIndiceCountOfCurrentEntity;
				const std::uint64_t indiceCount = EVERYCULLING_MIN(DEFAULT_BINNED_TRIANGLE_COUNT_PER_LOOP * 3, totalIndiceCount - currentBinnedIndiceCountOfCurrentEntity);

				BinTriangles
				(
					reinterpret_cast<const float*>(vertices),
					verticeCount,
					startIndicePtr,
					indiceCount,
					vertexStride,
					modelToClipSpaceMatrix.data()
				);
			}
			else
			{
				break;
			}
		}

		totalBinnedIndiceCount += EVERYCULLING_MIN(totalIndiceCount, currentBinnedIndiceCountOfCurrentEntity);
	}
}

culling::BinTrianglesStage::BinTrianglesStage(MaskedSWOcclusionCulling* mMOcclusionCulling)
	: MaskedSWOcclusionCullingStage{ mMOcclusionCulling }
{
}

void culling::BinTrianglesStage::ResetCullingModule(const unsigned long long currentTickCount)
{
	MaskedSWOcclusionCullingStage::ResetCullingModule(currentTickCount);
}

void culling::BinTrianglesStage::CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount)
{
	if(EVERYCULLING_WHEN_TO_BIN_TRIANGLE(currentTickCount))
	{
#ifdef EVERYCULLING_FETCH_OBJECT_SORT_FROM_DOOMS_ENGINE_IN_BIN_TRIANGLE_STAGE
		BinTriangleThreadJobByObjectOrder(cameraIndex);
#else
		BinTriangleThreadJob(cameraIndex);
#endif
	}
}

const char* culling::BinTrianglesStage::GetCullingModuleName() const
{
	return "BinTrianglesStage";
}

EVERYCULLING_FORCE_INLINE void culling::BinTrianglesStage::BinTriangles
(
	const float* const vertices,
	const uint64_t verticeCount,
	const std::uint32_t* const vertexIndices,
	const uint64_t indiceCount,
	const uint64_t vertexStrideByte,
	const float* const modelToClipspaceMatrix
)
{
	assert(indiceCount > 0);

	const uint64_t fetchTriangleCount = EVERYCULLING_MIN(8, indiceCount / 3);
	assert(fetchTriangleCount != 0);

	// First 4 bits show if traingle is valid
	// Current Value : 00000000 00000000 00000000 11111111
	std::uint32_t triangleCullMask = (1 << fetchTriangleCount) - 1;


	//Why Size of array is 3?
	//A culling::EVERYCULLING_M256F can have 8 floating-point
	//A TwoDTriangle have 3 point
	//So If you have just one culling::EVERYCULLING_M256F variable, a floating-point is unused.
	//Not to make unused space, 12 floating point is required per axis
	// culling::EVERYCULLING_M256F * 3 -> 8 TwoDTriangle with no unused space

	//We don't need z value in Binning stage!!!
	// Triangle's First Vertex X is in ndcSpaceVertexX[0][0]
	// Triangle's Second Vertex X is in ndcSpaceVertexX[0][1]
	// Triangle's Third Vertex X is in ndcSpaceVertexX[0][2]
	culling::EVERYCULLING_M256F ndcSpaceVertexX[3], ndcSpaceVertexY[3], ndcSpaceVertexZ[3], oneDividedByW[3];

	//Gather Vertex with indice
	//WE ARRIVE AT MODEL SPACE COORDINATE!
	GatherVertices(vertices, verticeCount, vertexIndices, indiceCount, 0, vertexStrideByte, fetchTriangleCount, ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ);


	//////////////////////////////////////////////////


	for (int i = 0; i < 3; i++)
	{
		oneDividedByW[i] = culling::EVERYCULLING_M256F_MUL_AND_ADD(ndcSpaceVertexX[i], _mm256_set1_ps(modelToClipspaceMatrix[3]), culling::EVERYCULLING_M256F_MUL_AND_ADD(ndcSpaceVertexY[i], _mm256_set1_ps(modelToClipspaceMatrix[7]), culling::EVERYCULLING_M256F_MUL_AND_ADD(ndcSpaceVertexZ[i], _mm256_set1_ps(modelToClipspaceMatrix[11]), _mm256_set1_ps(modelToClipspaceMatrix[15]))));
	}

	const culling::EVERYCULLING_M256F positiveWMask = ComputePositiveWMask(oneDividedByW);

	const culling::EVERYCULLING_M256I allOne = _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF);
	const culling::EVERYCULLING_M256F negativeWMask = _mm256_xor_ps(positiveWMask, *reinterpret_cast<const __m256*>(&allOne));
	triangleCullMask &= _mm256_movemask_ps(*reinterpret_cast<const culling::EVERYCULLING_M256F*>(&positiveWMask));
	if (triangleCullMask == 0x00000000)
	{
		return;
	}

	/*
	for (int i = 0; i < 3; i++)
	{
		const culling::EVERYCULLING_M256F point_W_IsNegativeValue = _mm256_cmp_ps(oneDividedByW[i], _mm256_set1_ps(std::numeric_limits<float>::epsilon()), _CMP_LT_OQ);
		oneDividedByW[i] = _mm256_blendv_ps(oneDividedByW[i], _mm256_set1_ps(1.0f), point_W_IsNegativeValue);
	}
	*/


	//////////////////////////////////////////////////


	//Convert Model space Vertex To Clip space Vertex
	//WE ARRIVE AT CLIP SPACE COORDINATE. W IS NOT 1
	culling::vertexTransformationHelper::TransformThreeVerticesToClipSpace
	(
		ndcSpaceVertexX,
		ndcSpaceVertexY,
		ndcSpaceVertexZ,
		modelToClipspaceMatrix
	);



	Clipping(ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, oneDividedByW, triangleCullMask);
	if (triangleCullMask == 0x00000000)
	{
		return;
	}



	for (int i = 0; i < 3; i++)
	{
		oneDividedByW[i] = culling::EVERYCULLING_M256F_DIV(_mm256_set1_ps(1.0f), oneDividedByW[i]);
	}

	//////////////////////////////////////////////////

	// Now Z value is on NDC coordinate
	for (int i = 0; i < 3; i++)
	{
		ndcSpaceVertexZ[i] = culling::EVERYCULLING_M256F_MUL(ndcSpaceVertexZ[i], oneDividedByW[i]);
	}

	//////////////////////////////////////////////////

	culling::EVERYCULLING_M256F screenPixelPosX[3], screenPixelPosY[3];
	culling::vertexTransformationHelper::ConvertClipSpaceThreeVerticesToScreenPixelSpace(ndcSpaceVertexX, ndcSpaceVertexY, oneDividedByW, screenPixelPosX, screenPixelPosY, mMaskedOcclusionCulling->mDepthBuffer);

	BackfaceCulling(screenPixelPosX, screenPixelPosY, triangleCullMask);

	if (triangleCullMask == 0x00000000)
	{
		return;
	}


	//////////////////////////////////////////////////


	Sort_8_3DTriangles
	(
		screenPixelPosX[0],
		screenPixelPosY[0],
		ndcSpaceVertexZ[0],

		screenPixelPosX[1],
		screenPixelPosY[1],
		ndcSpaceVertexZ[1],

		screenPixelPosX[2],
		screenPixelPosY[2],
		ndcSpaceVertexZ[2]
	);


	culling::EVERYCULLING_M256F LEFT_MIDDLE_POINT_X;
	culling::EVERYCULLING_M256F LEFT_MIDDLE_POINT_Y;
	culling::EVERYCULLING_M256F LEFT_MIDDLE_POINT_Z;

	culling::EVERYCULLING_M256F RIGHT_MIDDLE_POINT_X;
	culling::EVERYCULLING_M256F RIGHT_MIDDLE_POINT_Y;
	culling::EVERYCULLING_M256F RIGHT_MIDDLE_POINT_Z;


	// split triangle
	culling::rasterizerHelper::GetMiddlePointOfTriangle
	(
		screenPixelPosX[0],
		screenPixelPosY[0],
		ndcSpaceVertexZ[0],

		screenPixelPosX[1],
		screenPixelPosY[1],
		ndcSpaceVertexZ[1],

		screenPixelPosX[2],
		screenPixelPosY[2],
		ndcSpaceVertexZ[2],

		LEFT_MIDDLE_POINT_X,
		LEFT_MIDDLE_POINT_Y,
		LEFT_MIDDLE_POINT_Z,

		RIGHT_MIDDLE_POINT_X,
		RIGHT_MIDDLE_POINT_Y,
		RIGHT_MIDDLE_POINT_Z
	);

#ifdef EVERYCULLING_DEBUG_CULLING



#endif


	{
		//Bin Bottom Flat Triangle


		culling::EVERYCULLING_M256I outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY;
		//Bin Triangles to tiles

		//Compute Bin Bounding Box
		//Get Intersecting Bin List
		culling::depthBufferTileHelper::ComputeBinBoundingBoxFromThreeVertices
		(
			screenPixelPosX[0],
			screenPixelPosY[0],

			LEFT_MIDDLE_POINT_X,
			LEFT_MIDDLE_POINT_Y,

			RIGHT_MIDDLE_POINT_X,
			RIGHT_MIDDLE_POINT_Y,

			outBinBoundingBoxMinX,
			outBinBoundingBoxMinY,
			outBinBoundingBoxMaxX,
			outBinBoundingBoxMaxY,
			mMaskedOcclusionCulling->mDepthBuffer
		);

#ifdef EVERYCULLING_DEBUG_CULLING
		for (size_t triangleIndex = 0; triangleIndex < fetchTriangleCount; triangleIndex++)
		{
			if ((triangleCullMask & (1 << triangleIndex)) != 0x00000000)
			{
				assert(reinterpret_cast<const int*>(&outBinBoundingBoxMinX)[triangleIndex] <= reinterpret_cast<const int*>(&outBinBoundingBoxMaxX)[triangleIndex]);
				assert(reinterpret_cast<const int*>(&outBinBoundingBoxMinY)[triangleIndex] <= reinterpret_cast<const int*>(&outBinBoundingBoxMaxY)[triangleIndex]);
			}
		}
#endif

		// Pass triangle in counter clock wise
		PassTrianglesToTileBin
		(
			screenPixelPosX[0],
			screenPixelPosY[0],
			ndcSpaceVertexZ[0],

			LEFT_MIDDLE_POINT_X,
			LEFT_MIDDLE_POINT_Y,
			LEFT_MIDDLE_POINT_Z,

			RIGHT_MIDDLE_POINT_X,
			RIGHT_MIDDLE_POINT_Y,
			RIGHT_MIDDLE_POINT_Z,

			triangleCullMask,
			fetchTriangleCount,
			outBinBoundingBoxMinX,
			outBinBoundingBoxMinY,
			outBinBoundingBoxMaxX,
			outBinBoundingBoxMaxY
		);
	}


	{
		//Bin Top Flat Triangle

		culling::EVERYCULLING_M256I outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY;

		culling::depthBufferTileHelper::ComputeBinBoundingBoxFromThreeVertices
		(
			screenPixelPosX[2],
			screenPixelPosY[2],

			RIGHT_MIDDLE_POINT_X,
			RIGHT_MIDDLE_POINT_Y,

			LEFT_MIDDLE_POINT_X,
			LEFT_MIDDLE_POINT_Y,

			outBinBoundingBoxMinX,
			outBinBoundingBoxMinY,
			outBinBoundingBoxMaxX,
			outBinBoundingBoxMaxY,
			mMaskedOcclusionCulling->mDepthBuffer
		);

#ifdef EVERYCULLING_DEBUG_CULLING
		for (size_t triangleIndex = 0; triangleIndex < fetchTriangleCount; triangleIndex++)
		{
			if ((triangleCullMask & (1 << triangleIndex)) != 0x00000000)
			{
				assert(reinterpret_cast<const int*>(&outBinBoundingBoxMinX)[triangleIndex] <= reinterpret_cast<const int*>(&outBinBoundingBoxMaxX)[triangleIndex]);
				assert(reinterpret_cast<const int*>(&outBinBoundingBoxMinY)[triangleIndex] <= reinterpret_cast<const int*>(&outBinBoundingBoxMaxY)[triangleIndex]);
			}
		}
#endif

		// Pass triangle in counter clock wise
		PassTrianglesToTileBin
		(
			screenPixelPosX[2],
			screenPixelPosY[2],
			ndcSpaceVertexZ[2],

			RIGHT_MIDDLE_POINT_X,
			RIGHT_MIDDLE_POINT_Y,
			RIGHT_MIDDLE_POINT_Z,

			LEFT_MIDDLE_POINT_X,
			LEFT_MIDDLE_POINT_Y,
			LEFT_MIDDLE_POINT_Z,

			triangleCullMask,
			fetchTriangleCount,
			outBinBoundingBoxMinX,
			outBinBoundingBoxMinY,
			outBinBoundingBoxMaxX,
			outBinBoundingBoxMaxY
		);
	}
}

