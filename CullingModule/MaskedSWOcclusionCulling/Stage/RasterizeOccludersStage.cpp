#include "RasterizeOccludersStage.h"

#include "../MaskedSWOcclusionCulling.h"
#include "../Utility/CoverageRasterizer.h"
#include "../Utility/DepthValueComputer.h"

void culling::RasterizeOccludersStage::UpdateHierarchicalDepthBuffer()
{
	/*
			// Discard working layer heuristic
			dist1t = tile.zMax1 - tri.zMax
			dist01 = tile.zMax0 - tile.zMax1
			if (dist1t > dist01)
				tile.zMax1 = 0
				tile.mask = 0
			// Merge current triangle into working layer
			tile.zMax1 = max(tile.zMax1, tri.zMax)
			tile.mask |= tri.coverageMask
			// Overwrite ref. layer if working layer full
			if (tile.mask == ~0)
				tile.zMax0 = tile.zMax1
				tile.zMax1 = 0
				tile.mask = 0
			*/
}

void culling::RasterizeOccludersStage::ComputeTrianglesDepthValueInTile()
{
}


culling::M256I culling::RasterizeOccludersStage::ShuffleCoverageMask(const culling::M256I& coverageMask) const
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

	/*
	for (size_t triangleIndex = 0; triangleIndex < tile->mBinnedTriangles.mCurrentTriangleCount; triangleIndex++)
	{
		culling::TwoDTriangle twoDTriangle;
		twoDTriangle.Points[0] = { tile->mBinnedTriangles.VertexX[0][triangleIndex], tile->mBinnedTriangles.VertexY[0][triangleIndex] };
		twoDTriangle.Points[1] = { tile->mBinnedTriangles.VertexX[1][triangleIndex], tile->mBinnedTriangles.VertexY[1][triangleIndex] };
		twoDTriangle.Points[2] = { tile->mBinnedTriangles.VertexX[2][triangleIndex], tile->mBinnedTriangles.VertexY[2][triangleIndex] };
		culling::M256I result = culling::CoverageRasterizer::FillTriangle(tileOriginPoint, twoDTriangle);
		tile->mHizDatas.l1CoverageMask = _mm256_or_si256(tile->mHizDatas.l1CoverageMask, result);
	}
	*/
	
	for(size_t triangleBatchIndex = 0 ; triangleBatchIndex < tile->mBinnedTriangles.mCurrentTriangleCount ; triangleBatchIndex+=8)
	{
		TwoDTriangle twoDTriangle;
		
		culling::M256I CoverageMask[8];

		culling::CoverageRasterizer::FillTriangleBatch
		(
			CoverageMask,
			tileOriginPoint, 
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexX[0][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexY[0][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexX[1][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexY[1][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexX[2][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexY[2][triangleBatchIndex]))
		);
		
		/*
		for(size_t triangleIndex = triangleBatchIndex ; triangleIndex < triangleBatchIndex + 8 && triangleIndex < tile->mBinnedTriangles.mCurrentTriangleCount ; triangleIndex++)
		{
			tile->mHizDatas.l1CoverageMask = _mm256_or_si256(tile->mHizDatas.l1CoverageMask, CoverageMask[triangleIndex - triangleBatchIndex]);
		}
		*/
		culling::M256F subTileMaxDepth[8];

		culling::DepthValueComputer::ComputeDepthValue
		(
			subTileMaxDepth,
			tile->GetLeftBottomTileOrginX(),
			tile->GetLeftBottomTileOrginY(),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexX[0][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexY[0][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexZ[0][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexX[1][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexY[1][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexZ[1][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexX[2][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexY[2][triangleBatchIndex])),
			*reinterpret_cast<culling::M256F*>(&(tile->mBinnedTriangles.VertexZ[2][triangleBatchIndex]))
		);


		for (size_t triangleIndex = triangleBatchIndex; triangleIndex < triangleBatchIndex + 8 && triangleIndex < tile->mBinnedTriangles.mCurrentTriangleCount; triangleIndex++)
		{
			CoverageMask[triangleIndex] = ShuffleCoverageMask(CoverageMask[triangleIndex]);


		}
		
		// algo : if coverage mask is full, overrite tile->mHizDatas.l1MaxDepthValue to tile->mHizDatas.lMaxDepthValue and clear coverage mask
	}
	
	


	
	/*
	const culling::M256I test
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

	const culling::M256I correctTestResult
		=
		_mm256_setr_epi8
		(
			0, 1, 2, 3,
			4, 5, 6, 7,
			8, 9, 10, 11,
			12, 13, 14, 15,
			0, 1, 2, 3,
			4, 5, 6, 7,
			8, 9, 10, 11,
			12, 13, 14, 15
		);
	
	const culling::M256I testResult = ShuffleCoverageMask(test);

	assert(_mm256_testc_si256(correctTestResult, testResult) == 1);
	*/
	
}

culling::Tile* culling::RasterizeOccludersStage::GetNextDepthBufferTile(const size_t cameraIndex)
{
	culling::Tile* nextDepthBufferTile = nullptr;

	const size_t currentTileIndex = mFinishedTileCount[cameraIndex].fetch_add(1, std::memory_order_seq_cst);

	const size_t tileCount = mMaskedOcclusionCulling->mDepthBuffer.GetTileCount();

	if(currentTileIndex < tileCount)
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
	const culling::Tile* const endTile = mMaskedOcclusionCulling->mDepthBuffer.GetTiles() + mMaskedOcclusionCulling->mDepthBuffer.GetTileCount();

	while(true)
	{
		culling::Tile* const nextTile = GetNextDepthBufferTile(cameraIndex);

		if(nextTile != nullptr)
		{
			if (nextTile->mBinnedTriangles.mCurrentTriangleCount > 0)
			{
				RasterizeBinnedTriangles(cameraIndex, nextTile);
			}
		}
		else
		{
			break;
		}
	}

}

