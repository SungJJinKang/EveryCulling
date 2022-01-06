#include "RasterizeOccludersStage.h"

#include "../MaskedSWOcclusionCulling.h"
#include "../Utility/CoverageRasterizer.h"
#include "../Utility/DepthValueComputer.h"
#include "../Utility/RasterizerHelper.h"
#include "../Utility/triangleSlopeEventGetter.h"


/// <summary>
///
/// Shuffle Coverage Mask
///
/// 77777777 77777777 77777777 77777777
/// 66666666 66666666 66666666 66666666
/// 55555555 55555555 55555555 55555555
/// 44444444 44444444 44444444 44444444 <-- ( fourth tile. 1, 0 )
/// 33333333 33333333 33333333 33333333
/// 22222222 22222222 22222222 22222222
/// 11111111 11111111 11111111 11111111 <-- ( second tile. 0, 1 )
/// 00000000 00000000 00000000 00000000 <-- ( first tile. 0, 0 )
///
///   |
///   |
///	  V
///
/// 44444444 55555555 66666666 777777777
/// 44444444 55555555 66666666 777777777
/// 44444444 55555555 66666666 777777777
/// 44444444 55555555 66666666 777777777
/// 00000000 11111111 22222222 333333333
/// 00000000 11111111 22222222 333333333
/// 00000000 11111111 22222222 333333333
/// 00000000 11111111 22222222 333333333
///
/// </summary>
/// <param name="coverageMask"></param>
/// <returns></returns>
FORCE_INLINE culling::M256I culling::RasterizeOccludersStage::ShuffleCoverageMask(const culling::M256I& coverageMask) const
{
	static const culling::M256I shuffleMask
	=
	_mm256_setr_epi8
	(
		0, 4, 8, 12,
		1, 5, 9, 13,
		2, 6, 10, 14,
		3, 7, 11, 15,

		0, 4, 8, 12,
		1, 5, 9, 13,
		2, 6, 10, 14,
		3, 7, 11, 15
	);

	return _mm256_shuffle_epi8(coverageMask, shuffleMask);
}

void culling::RasterizeOccludersStage::RasterizeBinnedTriangles
(
	const size_t cameraIndex,
	culling::Tile* const tile
)
{
	assert(tile != nullptr);

	const culling::Vec2 tileOriginPoint{ static_cast<float>(tile->GetLeftBottomTileOrginX()), static_cast<float>(tile->GetLeftBottomTileOrginY()) };

	const size_t occluderCount = mMaskedOcclusionCulling->GetOccluderCount();

	for (size_t occluderIndex = 0; occluderIndex < occluderCount; occluderIndex++)
	{
		TriangleList* const triangleListOfOccluder = tile->GetBinnedTriangleListOfOccluder(occluderIndex);
		for (size_t triangleBatchIndex = 0; triangleBatchIndex < triangleListOfOccluder->mCurrentTriangleCount; triangleBatchIndex += 8)
		{
			const size_t triangleCount = MIN(8, triangleListOfOccluder->mCurrentTriangleCount - triangleBatchIndex);
			assert(triangleCount <= 8);

			//Triangle is already counter clock wise, and front facing
			culling::M256F& TriPointA_X = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexX[0][triangleBatchIndex]));
			culling::M256F& TriPointA_Y = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexY[0][triangleBatchIndex]));
			culling::M256F& TriPointA_Z = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexZ[0][triangleBatchIndex]));
			culling::M256F& TriPointB_X = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexX[1][triangleBatchIndex]));
			culling::M256F& TriPointB_Y = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexY[1][triangleBatchIndex]));
			culling::M256F& TriPointB_Z = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexZ[1][triangleBatchIndex]));
			culling::M256F& TriPointC_X = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexX[2][triangleBatchIndex]));
			culling::M256F& TriPointC_Y = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexY[2][triangleBatchIndex]));
			culling::M256F& TriPointC_Z = *reinterpret_cast<culling::M256F*>(&(triangleListOfOccluder->VertexZ[2][triangleBatchIndex]));

			culling::M256I LeftSlopeEventOfTriangle[8];
			culling::M256I RightSlopeEventOfTriangle[8];


			//second point ( pointB ) should be left(x) of third point ( pointA )

			{
				const culling::M256F leftBCCmpResult = _mm256_cmp_ps(TriPointB_X, TriPointC_X, _CMP_LT_OS);

				const culling::M256F sortedTriPointB_X = _mm256_blendv_ps(TriPointC_X, TriPointB_X, leftBCCmpResult);
				const culling::M256F sortedTriPointB_Y = _mm256_blendv_ps(TriPointC_Y, TriPointB_Y, leftBCCmpResult);

				const culling::M256F sortedTriPointC_X = _mm256_blendv_ps(TriPointB_X, TriPointC_X, leftBCCmpResult);
				const culling::M256F sortedTriPointC_Y = _mm256_blendv_ps(TriPointB_Y, TriPointC_Y, leftBCCmpResult);

				culling::triangleSlopeHelper::GatherBottomFlatTriangleSlopeEvent
				(
					triangleCount,
					tileOriginPoint,
					LeftSlopeEventOfTriangle,
					RightSlopeEventOfTriangle,

					TriPointA_X,
					TriPointA_Y,

					sortedTriPointB_X,
					sortedTriPointB_Y,

					sortedTriPointC_X,
					sortedTriPointC_Y
				);


				// Clamp slope event 0 ~ TILE_WIDTH
				for (size_t triangleIndex = 0; triangleIndex < 8; triangleIndex++)
				{
					LeftSlopeEventOfTriangle[triangleIndex] = _mm256_max_epi32(_mm256_min_epi32(LeftSlopeEventOfTriangle[triangleIndex], _mm256_set1_epi32(TILE_WIDTH)), _mm256_set1_epi32(0));
					RightSlopeEventOfTriangle[triangleIndex] = _mm256_max_epi32(_mm256_min_epi32(RightSlopeEventOfTriangle[triangleIndex], _mm256_set1_epi32(TILE_WIDTH)), _mm256_set1_epi32(0));
				}
			}

			std::uint32_t triangleMask = (1 << triangleCount) - 1;

			culling::M256I CoverageMask[8];
			culling::M256F subTileMaxDepth[8];
			for (size_t triangleIndex = 0; triangleIndex < 8; triangleIndex++)
			{
				CoverageMask[triangleIndex] = _mm256_setzero_si256(); // clear coverage mask
				subTileMaxDepth[triangleIndex] = _mm256_set1_ps((float)MIN_DEPTH_VALUE); // clear subTileMaxDepth
			}

			culling::M256F minY = _mm256_min_ps(_mm256_min_ps(TriPointA_Y, TriPointB_Y), TriPointC_Y);
			culling::M256F maxY = _mm256_max_ps(_mm256_max_ps(TriPointA_Y, TriPointB_Y), TriPointC_Y);

			{

				culling::CoverageRasterizer::FillFlatTriangleBatch
				(
					triangleCount,
					CoverageMask,
					tileOriginPoint,

					LeftSlopeEventOfTriangle,
					RightSlopeEventOfTriangle,
					minY,
					maxY
				);

				for (size_t triangleIndex = 0; triangleIndex < 8; triangleIndex++)
				{
					// if coveragemask is all zero, isCoverageMaskAllZero is 1
					const int isCoverageMaskAllZero = _mm256_testz_si256(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), CoverageMask[triangleIndex]);

					triangleMask &= ~(isCoverageMaskAllZero << triangleIndex);
				}


				// ShuffleCoverageMask is really cheap!!.
				// Branchless is faster than considering triangleCount, triangleMask
				for (size_t triangleIndex = 0; triangleIndex < 8; triangleIndex++)
				{
					CoverageMask[triangleIndex] = ShuffleCoverageMask(CoverageMask[triangleIndex]);
				}

				// 44444444 55555555 66666666 77777777
				// 44444444 55555555 66666666 77777777
				// 44444444 55555555 66666666 77777777
				// 44444444 55555555 66666666 77777777
				// 
				// 00000000 11111111 22222222 33333333
				// 00000000 11111111 22222222 33333333
				// 00000000 11111111 22222222 33333333
				// 00000000 11111111 22222222 33333333
				//
				// --> 256bit
				//
				//
				//
				// 0 : CoverageMask ( 0 ~ 32 )
				// 1 : CoverageMask ( 32 ~ 64 )
				// 2 : CoverageMask ( 64 ~ 96 )
				// 3 : CoverageMask ( 96 ~ 128 )
				// 4 : CoverageMask ( 128 ~ 160 )
				// 5 : CoverageMask ( 160 ~ 192 )
				// 6 : CoverageMask ( 192 ~ 224 )
				// 7 : CoverageMask ( 224 ~ 256 )

			}





			culling::DepthValueComputer::ComputeFlatTriangleMaxDepthValue
			(
				triangleCount,
				subTileMaxDepth,
				tileOriginPoint.x,
				tileOriginPoint.y,

				TriPointA_X,
				TriPointA_Y,
				TriPointA_Z,

				TriPointB_X,
				TriPointB_Y,
				TriPointB_Z,

				TriPointC_X,
				TriPointC_Y,
				TriPointC_Z,

				LeftSlopeEventOfTriangle,
				RightSlopeEventOfTriangle,

				minY,
				maxY,

				triangleMask
			);




			for (size_t triangleIndex = 0; triangleIndex < 8; triangleIndex++)
			{
				if ((triangleMask & (1 << triangleIndex)) != 0x00000000)
				{
					// algo : if coverage mask is full, overrite tile->mHizDatas.L1SubTileMaxDepthValue to tile->mHizDatas.lMaxDepthValue and clear coverage mask



					// exclude L1 depth of sub tile with zero coverage mask 
					const culling::M256I tileCoverageMaskIsZero = _mm256_cmpeq_epi32(CoverageMask[triangleIndex], _mm256_set1_epi32(0));
					subTileMaxDepth[triangleIndex] = _mm256_blendv_ps(subTileMaxDepth[triangleIndex], _mm256_set1_ps(-1.0f), *reinterpret_cast<const culling::M256F*>(&tileCoverageMaskIsZero));

					tile->mHizDatas.L1SubTileMaxDepthValue = _mm256_max_ps(tile->mHizDatas.L1SubTileMaxDepthValue, subTileMaxDepth[triangleIndex]);
					tile->mHizDatas.L1CoverageMask = _mm256_or_si256(tile->mHizDatas.L1CoverageMask, CoverageMask[triangleIndex]);

					const culling::M256I maskCoveredByOne = _mm256_cmpeq_epi32(tile->mHizDatas.L1CoverageMask, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));
					tile->mHizDatas.L0SubTileMaxDepthValue = _mm256_blendv_ps(tile->mHizDatas.L0SubTileMaxDepthValue, _mm256_min_ps(tile->mHizDatas.L0SubTileMaxDepthValue, tile->mHizDatas.L1SubTileMaxDepthValue), *reinterpret_cast<const culling::M256F*>(&maskCoveredByOne));
					tile->mHizDatas.L1SubTileMaxDepthValue = _mm256_blendv_ps(tile->mHizDatas.L1SubTileMaxDepthValue, _mm256_set1_ps((float)MIN_DEPTH_VALUE), *reinterpret_cast<const culling::M256F*>(&maskCoveredByOne));
					const culling::M256F maskBlendResult = _mm256_blendv_ps(*reinterpret_cast<const culling::M256F*>(&(tile->mHizDatas.L1CoverageMask)), _mm256_setzero_ps(), *reinterpret_cast<const culling::M256F*>(&maskCoveredByOne));
					tile->mHizDatas.L1CoverageMask = *reinterpret_cast<const culling::M256I*>(&maskBlendResult);





					//coverage mask test codes
					//tile->mHizDatas.L1CoverageMask = _mm256_or_si256(tile->mHizDatas.L1CoverageMask, CoverageMask[triangleIndex]);
				}
			}


			// Compute max depth value of subtiles's L0 max depth value
			float maxDepthValue = -1.0f;
			for (size_t i = 0; i < 8; i++)
			{
				maxDepthValue = MAX(maxDepthValue, reinterpret_cast<const float*>(&(tile->mHizDatas.L0SubTileMaxDepthValue))[i]);
			}
			tile->mHizDatas.L0MaxDepthValue = maxDepthValue;
			assert(maxDepthValue >= -1.0f && maxDepthValue <= 1.0f);

#ifdef DEBUG_CULLING
			const culling::M256I test
				=
				_mm256_setr_epi8
				(
					0, 4, 8, 12,
					1, 5, 9, 13,
					2, 6, 10, 14,
					3, 7, 11, 15,
					4, 2, 2, 1,
					10, 5, 9, 5,
					11, 3, 10, 4,
					10, 1, 11, 1
				);

			const culling::M256I correctTestResult
				=
				_mm256_setr_epi8
				(
					0, 1, 2, 3,
					4, 5, 6, 7,
					8, 9, 10, 11,
					12, 13, 14, 15,
					4, 10, 11, 10,
					2, 5, 3, 1,
					2, 9, 10, 11,
					1, 5, 4, 1
				);


			const culling::M256I testResult = ShuffleCoverageMask(test);

			assert(_mm256_test_all_ones(_mm256_cmpeq_epi8(correctTestResult, testResult)));

#endif

		}
	}
}

culling::Tile* culling::RasterizeOccludersStage::GetNextDepthBufferTile(const size_t cameraIndex)
{
	return GetNextDepthBufferTileBatch(cameraIndex, 1);
}

culling::Tile* culling::RasterizeOccludersStage::GetNextDepthBufferTileBatch
(
	const size_t cameraIndex,
	const size_t batchCount
)
{
	culling::Tile* nextDepthBufferTile = nullptr;

	const size_t currentTileIndex = mFinishedTileCount[cameraIndex].fetch_add(batchCount, std::memory_order_seq_cst);

	const size_t tileCount = mMaskedOcclusionCulling->mDepthBuffer.GetTileCount();

	if (currentTileIndex < tileCount)
	{
		nextDepthBufferTile = mMaskedOcclusionCulling->mDepthBuffer.GetTile(currentTileIndex);
	}

	return nextDepthBufferTile;
}

culling::RasterizeOccludersStage::RasterizeOccludersStage(MaskedSWOcclusionCulling* mOcclusionCulling)
	: MaskedSWOcclusionCullingStage{ mOcclusionCulling }
{
}

void culling::RasterizeOccludersStage::ResetCullingModule()
{
	MaskedSWOcclusionCullingStage::ResetCullingModule();

	for (std::atomic<size_t>& atomicVal : mFinishedTileCount)
	{
		atomicVal.store(0, std::memory_order_relaxed);
	}
}

void culling::RasterizeOccludersStage::CullBlockEntityJob(const size_t cameraIndex)
{
	if (mMaskedOcclusionCulling->IsOccluderExist() == true)
	{
		const culling::Tile* const endTile = mMaskedOcclusionCulling->mDepthBuffer.GetTiles() + mMaskedOcclusionCulling->mDepthBuffer.GetTileCount();

		while (true)
		{
			culling::Tile* const nextTile = GetNextDepthBufferTile(cameraIndex);

			if (nextTile != nullptr)
			{
				RasterizeBinnedTriangles(cameraIndex, nextTile);
			}
			else
			{
				break;
			}
		}
	}

}

const char* culling::RasterizeOccludersStage::GetCullingModuleName() const
{
	return "RasterizeOccludersStage";
}

