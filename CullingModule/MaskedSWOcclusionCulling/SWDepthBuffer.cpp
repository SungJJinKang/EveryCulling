#include "SWDepthBuffer.h"
#include <cstdio>
#include <cstdlib>

culling::SWDepthBuffer::SWDepthBuffer(unsigned int width, unsigned int height)
	: mWidth{ width }, mHeight{ height }, mBinCountInRow{ width / SUB_TILE_WIDTH }, mBinCountInColumn{ height / SUB_TILE_HEIGHT }, mSubTileCount{ width / SUB_TILE_WIDTH * height / SUB_TILE_HEIGHT }
{
	assert(mWidth% SUB_TILE_WIDTH == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_WIDTH");
	assert(mHeight% SUB_TILE_HEIGHT == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_HEIGHT");

	this->mSubTiles.mHizDatas = new HizData[static_cast<size_t>(mSubTileCount)];
	//this->mSubTiles.mTriangleBins = std::aligned_alloc(64, sizeof(TriangleBin) * mSubTileCount);
}