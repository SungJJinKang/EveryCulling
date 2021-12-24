#include "RasterizeOccludersStage.h"

#include "../MaskedSWOcclusionCulling.h"
#include "../Rasterizer/CoverageRasterizer.h"


#define FETCH_TILE_COUNT_IN_CHUNK 10 //for minimizing performance drop form write combined buffer flush

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

	for(size_t triangleIndex = 0 ; triangleIndex < tile->mBinnedTriangles.mCurrentTriangleCount ; triangleIndex++)
	{
		culling::CoverageRasterizer::FillTriangle(tile->mHizDatas.l1CoverageMask, tileOriginPoint, tile->mBinnedTriangles.mTriangleList[triangleIndex]);



		//Snap Screen Space Coordinates To Integer Coordinate In ScreenBuffer(or DepthBuffer)

		//A grid square, including its (x, y) window coordinates, z (depth), and associated data which may be added by fragment shaders, is called a fragment. A
		//fragment is located by its lower left corner, which lies on integer grid coordinates.
		//Rasterization operations also refer to a fragment��s center, which is offset by ( 1/2, 1/2 )
		//from its lower left corner(and so lies on half - integer coordinates).
	}

	tile->mHizDatas.l1CoverageMask = ShuffleCoverageMask(tile->mHizDatas.l1CoverageMask);


	
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

culling::Tile* culling::RasterizeOccludersStage::GetNextDepthBufferTileChunk(const size_t cameraIndex)
{
	culling::Tile* nextDepthBufferTile = nullptr;

	const size_t currentTileIndex = mFinishedTileCount[cameraIndex].fetch_add(FETCH_TILE_COUNT_IN_CHUNK, std::memory_order_seq_cst);

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
		culling::Tile* const nextTileChunk = GetNextDepthBufferTileChunk(cameraIndex);

		if(nextTileChunk != nullptr)
		{
			for (size_t tileIndexInChunk = 0; tileIndexInChunk < FETCH_TILE_COUNT_IN_CHUNK; tileIndexInChunk++)
			{
				culling::Tile* const nextTile = nextTileChunk + tileIndexInChunk;

				if (nextTile < endTile)
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
		else
		{
			break;
		}
	}

}

