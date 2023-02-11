#include "SWDepthBuffer.h"

#include <limits>

void culling::HizData::Reset()
{
	L0MaxDepthValue = (float)EVERYCULLING_MAX_DEPTH_VALUE;
	L0SubTileMaxDepthValue = _mm256_set1_ps((float)EVERYCULLING_MAX_DEPTH_VALUE);
	L1SubTileMaxDepthValue = _mm256_set1_ps((float)EVERYCULLING_MIN_DEPTH_VALUE);
	ClearCoverageMaskAllSubTile();
}

void culling::HizData::ClearCoverageMaskAllSubTile()
{
	L1CoverageMask = _mm256_setzero_si256();
}

void culling::HizData::FillCoverageMask()
{
	L1CoverageMask = _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF);
}


void culling::Tile::Reset(const unsigned long long currentTickCount)
{
	if(EVERYCULLING_WHEN_TO_RASTERIZE_DEPTHBUFFER(currentTickCount))
	{
		mHizDatas.Reset();
	}

	if (EVERYCULLING_WHEN_TO_BIN_TRIANGLE(currentTickCount))
	{
		mBinnedTriangleCount = 0;
	}
}


culling::SWDepthBuffer::SWDepthBuffer(std::uint32_t width, std::uint32_t height)
	: 
	mResolution{
	width, height,
	height / EVERYCULLING_TILE_HEIGHT,width / EVERYCULLING_TILE_WIDTH,
	0, 0,
	( (width % EVERYCULLING_TILE_WIDTH) > 0 ? width + (width - width % EVERYCULLING_TILE_WIDTH) : width ) - EVERYCULLING_TILE_WIDTH,
	( (height % EVERYCULLING_TILE_HEIGHT) > 0 ? height + (height - height % EVERYCULLING_TILE_HEIGHT) : height ) - EVERYCULLING_TILE_HEIGHT,
	
	_mm256_set1_ps(static_cast<float>(width * 0.5f)),
	_mm256_set1_ps(static_cast<float>(height * 0.5f)),
	_mm256_set1_ps(static_cast<float>(width)),
	_mm256_set1_ps(static_cast<float>(height))
	},
	mTiles(nullptr)
{
	//"DepthBuffer's size should be multiple of EVERYCULLING_TILE_WIDTH"
	assert(mResolution.mWidth % EVERYCULLING_TILE_WIDTH == 0);
	//"DepthBuffer's size should be multiple of EVERYCULLING_TILE_HEIGHT"
	assert(mResolution.mHeight % EVERYCULLING_TILE_HEIGHT == 0);

	
	const size_t tileCount = static_cast<size_t>(mResolution.mRowTileCount) * static_cast<size_t>(mResolution.mColumnTileCount);
	mTiles = new Tile[tileCount];
	mTileCount = tileCount;

	for(std::uint32_t y = 0 ; y < mResolution.mRowTileCount ; y++)
	{
		for (std::uint32_t x = 0; x < mResolution.mColumnTileCount; x++)
		{
			culling::Tile* const tile = GetTile(y, x);
			tile->mLeftBottomTileOrginX = x * (std::uint32_t)EVERYCULLING_TILE_WIDTH;
			tile->mLeftBottomTileOrginY = y * (std::uint32_t)EVERYCULLING_TILE_HEIGHT;
		}
	}

	//test
	for(size_t i = 0 ; i < tileCount ; i++)
	{
		assert(mTiles[i].mLeftBottomTileOrginX != 0xFFFFFFFF);
		assert(mTiles[i].mLeftBottomTileOrginY != 0xFFFFFFFF);
	}

}

culling::SWDepthBuffer::~SWDepthBuffer()
{
	if(mTiles != nullptr)
	{
		delete[] mTiles;
	}
	
}

void culling::SWDepthBuffer::Reset(const unsigned long long currentTickCount)
{
	for (size_t i = 0; i < mTileCount; i++)
	{
		mTiles[i].Reset(currentTickCount);
	}

	std::atomic_thread_fence(std::memory_order_release);
}

const culling::Tile* culling::SWDepthBuffer::GetTiles() const
{
	return mTiles;
}

