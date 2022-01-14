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
EVERYCULLING_FORCE_INLINE culling::M256I culling::RasterizeOccludersStage::ShuffleCoverageMask(const culling::M256I& coverageMask) const
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

	const size_t binnedTriangleCount = tile->mmBinnedTriangleCount;

	for (size_t triangleIndex = 0; triangleIndex < binnedTriangleCount; triangleIndex++)
	{
		//Triangle is already counter clock wise, and front facing
		const float TriPointA_X = tile->mBinnedTriangleList[triangleIndex].PointAVertexX;
		const float TriPointA_Y = tile->mBinnedTriangleList[triangleIndex].PointAVertexY;
		const float TriPointA_Z = tile->mBinnedTriangleList[triangleIndex].PointAVertexZ;
		const float TriPointB_X = tile->mBinnedTriangleList[triangleIndex].PointBVertexX;
		const float TriPointB_Y = tile->mBinnedTriangleList[triangleIndex].PointBVertexY;
		const float TriPointB_Z = tile->mBinnedTriangleList[triangleIndex].PointBVertexZ;
		const float TriPointC_X = tile->mBinnedTriangleList[triangleIndex].PointCVertexX;
		const float TriPointC_Y = tile->mBinnedTriangleList[triangleIndex].PointCVertexY;
		const float TriPointC_Z = tile->mBinnedTriangleList[triangleIndex].PointCVertexZ;
		
		culling::M256I LeftSlopeEventOfTriangle;
		culling::M256I RightSlopeEventOfTriangle;


		//second point ( pointB ) should be left(x) of third point ( pointA )

		{
			const float sortedTriPointB_X = TriPointB_X >= TriPointC_X ? TriPointC_X : TriPointB_X;
			const float sortedTriPointB_Y = TriPointB_X >= TriPointC_X ? TriPointC_Y : TriPointB_Y;

			const float sortedTriPointC_X = TriPointB_X >= TriPointC_X ? TriPointB_X : TriPointC_X;
			const float sortedTriPointC_Y = TriPointB_X >= TriPointC_X ? TriPointB_Y : TriPointC_Y;

			culling::triangleSlopeHelper::GatherBottomFlatTriangleSlopeEvent
			(
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


			LeftSlopeEventOfTriangle = _mm256_max_epi32(_mm256_min_epi32(LeftSlopeEventOfTriangle, _mm256_set1_epi32(TILE_WIDTH)), _mm256_set1_epi32(0));
			RightSlopeEventOfTriangle = _mm256_max_epi32(_mm256_min_epi32(RightSlopeEventOfTriangle, _mm256_set1_epi32(TILE_WIDTH)), _mm256_set1_epi32(0));
		}
		
		culling::M256I CoverageMask = _mm256_setzero_si256(); // clear coverage mask
		culling::M256F subTileMaxDepth = _mm256_set1_ps((float)MIN_DEPTH_VALUE); // clear subTileMaxDepth

		const float minY = MIN(MIN(TriPointA_Y, TriPointB_Y), TriPointC_Y);
		const float maxY = MAX(MAX(TriPointA_Y, TriPointB_Y), TriPointC_Y);

		{

			culling::CoverageRasterizer::FillFlatTriangleBatch
			(
				CoverageMask,
				tileOriginPoint,

				LeftSlopeEventOfTriangle,
				RightSlopeEventOfTriangle,
				minY,
				maxY
			);

			// ShuffleCoverageMask is really cheap!!.
			// Branchless is faster than considering triangleCount, triangleMask
			CoverageMask = ShuffleCoverageMask(CoverageMask);

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
			maxY
		);




		// algo : if coverage mask is full, overrite tile->mHizDatas.L1SubTileMaxDepthValue to tile->mHizDatas.lMaxDepthValue and clear coverage mask



				// exclude L1 depth of sub tile with zero coverage mask 
		const culling::M256I tileCoverageMaskIsZero = _mm256_cmpeq_epi32(CoverageMask, _mm256_set1_epi32(0));
		subTileMaxDepth = _mm256_blendv_ps(subTileMaxDepth, _mm256_set1_ps(-1.0f), *reinterpret_cast<const culling::M256F*>(&tileCoverageMaskIsZero));

		tile->mHizDatas.L1SubTileMaxDepthValue = _mm256_max_ps(tile->mHizDatas.L1SubTileMaxDepthValue, subTileMaxDepth);
		tile->mHizDatas.L1CoverageMask = _mm256_or_si256(tile->mHizDatas.L1CoverageMask, CoverageMask);

		const culling::M256I maskCoveredByOne = _mm256_cmpeq_epi32(tile->mHizDatas.L1CoverageMask, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));
		tile->mHizDatas.L0SubTileMaxDepthValue = _mm256_blendv_ps(tile->mHizDatas.L0SubTileMaxDepthValue, _mm256_min_ps(tile->mHizDatas.L0SubTileMaxDepthValue, tile->mHizDatas.L1SubTileMaxDepthValue), *reinterpret_cast<const culling::M256F*>(&maskCoveredByOne));
		tile->mHizDatas.L1SubTileMaxDepthValue = _mm256_blendv_ps(tile->mHizDatas.L1SubTileMaxDepthValue, _mm256_set1_ps((float)MIN_DEPTH_VALUE), *reinterpret_cast<const culling::M256F*>(&maskCoveredByOne));
		const culling::M256F maskBlendResult = _mm256_blendv_ps(*reinterpret_cast<const culling::M256F*>(&(tile->mHizDatas.L1CoverageMask)), _mm256_setzero_ps(), *reinterpret_cast<const culling::M256F*>(&maskCoveredByOne));
		tile->mHizDatas.L1CoverageMask = *reinterpret_cast<const culling::M256I*>(&maskBlendResult);

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

void culling::RasterizeOccludersStage::ResetCullingModule(const unsigned long long currentTickCount)
{
	MaskedSWOcclusionCullingStage::ResetCullingModule(currentTickCount);

	for (std::atomic<size_t>& atomicVal : mFinishedTileCount)
	{
		atomicVal.store(0, std::memory_order_relaxed);
	}
}

void culling::RasterizeOccludersStage::CullBlockEntityJob(const size_t cameraIndex)
{
	if (mMaskedOcclusionCulling->GetIsOccluderExist() == true)
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

