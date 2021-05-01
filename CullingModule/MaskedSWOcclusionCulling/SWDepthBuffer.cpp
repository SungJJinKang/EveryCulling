#include "SWDepthBuffer.h"
#include <cstdio>
#include <cstdlib>

culling::SWDepthBuffer::SWDepthBuffer(unsigned int width, unsigned int height)
	: 
	mResolution{
	width, height,
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
	_mm_set_ps1(static_cast<float>(width * 0.5f)),
	_mm_set_ps1(static_cast<float>(height * 0.5f))
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
	_mm_set_ps1(static_cast<float>(width)),
	_mm_set_ps1(static_cast<float>(height))
#endif
	},
	mBinCountInRow{ width / SUB_TILE_WIDTH }, mBinCountInColumn{ height / SUB_TILE_HEIGHT }, mSubTileCount{ width / SUB_TILE_WIDTH * height / SUB_TILE_HEIGHT }
{
	assert(mResolution.mWidth% SUB_TILE_WIDTH == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_WIDTH");
	assert(mResolution.mHeight% SUB_TILE_HEIGHT == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_HEIGHT");

	this->mSubTiles.mHizDatas = new HizData[static_cast<size_t>(mSubTileCount)];
	//this->mSubTiles.mTriangleBins = std::aligned_alloc(64, sizeof(TriangleBin) * mSubTileCount);
}