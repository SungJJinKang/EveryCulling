#include "SWDepthBuffer.h"
#include <cstdio>
#include <cstdlib>
culling::SWDepthBuffer::SWDepthBuffer(unsigned int width, unsigned int height)
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
	}
{
	//"DepthBuffer's size should be multiple of TILE_WIDTH"
	assert(mResolution.mWidth % TILE_WIDTH == 0);
	//"DepthBuffer's size should be multiple of TILE_HEIGHT"
	assert(mResolution.mHeight % TILE_HEIGHT == 0);

	

	//Why don't use std::make_unique
	//std::make_unique do default initialization, so it reset Tile values
	//That isn't needed to do
	mTiles = std::shared_ptr<Tile[]>(new Tile[static_cast<size_t>(mResolution.mTileCountInARow) * static_cast<size_t>(mResolution.mTileCountInAColumn)]);
}

