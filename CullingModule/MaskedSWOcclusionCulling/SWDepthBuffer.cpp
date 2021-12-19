#include "SWDepthBuffer.h"

void culling::HizData::Reset()
{
	depthPosition = _mm256_set1_epi32(0);
	z0Max = _mm256_set1_ps(1.0f);
	z1Max = _mm256_set1_ps(1.0f);
}

void culling::TriangleList::Reset()
{
	mCurrentTriangleCount = 0;
}

void culling::Tile::Reset()
{
	mHizDatas.Reset();
	mBinnedTriangles.Reset();
}

culling::SWDepthBuffer::SWDepthBuffer(std::uint32_t width, std::uint32_t height)
	: 
	mResolution{
	width, height,
	width / TILE_WIDTH, height / TILE_HEIGHT,
	0, 0,
	( (width % TILE_WIDTH) > 0 ? width + (width - width % TILE_WIDTH) : width ) - TILE_WIDTH,
	( (height % TILE_HEIGHT) > 0 ? height + (height - height % TILE_HEIGHT) : height ) - TILE_HEIGHT,

#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
	_mm256_set1_ps(static_cast<float>(width * 0.5f)),
	_mm256_set1_ps(static_cast<float>(height * 0.5f))
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
	_mm256_set1_ps(static_cast<float>(width)),
	_mm256_set1_ps(static_cast<float>(height))
#endif
	},
	mTiles(nullptr)
{
	//"DepthBuffer's size should be multiple of TILE_WIDTH"
	assert(mResolution.mWidth % TILE_WIDTH == 0);
	//"DepthBuffer's size should be multiple of TILE_HEIGHT"
	assert(mResolution.mHeight % TILE_HEIGHT == 0);

	
	const size_t tileCount = GetTileCount();
	mTiles = new Tile[tileCount];
	mTileCount = tileCount;
}

culling::SWDepthBuffer::~SWDepthBuffer()
{
	if(mTiles != nullptr)
	{
		delete[] mTiles;
	}
	
}

size_t culling::SWDepthBuffer::GetTileCount() const
{
	return static_cast<size_t>(mResolution.mTileCountInARow) * static_cast<size_t>(mResolution.mTileCountInAColumn);
}

void culling::SWDepthBuffer::Reset()
{
	for (size_t i = 0; i < mTileCount; i++)
	{
		mTiles[i].Reset();
	}
}

