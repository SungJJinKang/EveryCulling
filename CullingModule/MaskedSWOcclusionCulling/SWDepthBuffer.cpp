#include "SWDepthBuffer.h"
#include <cstdio>
#include <cstdlib>

culling::SWDepthBuffer::SWDepthBuffer(unsigned int width, unsigned int height)
	: 
	mResolution{
	width, height,
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
	_mm256_set1_ps(static_cast<float>(width * 0.5f)),
	_mm256_set1_ps(static_cast<float>(height * 0.5f))
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
	_mm256_set1_ps(static_cast<float>(width)),
	_mm256_set1_ps(static_cast<float>(height))
#endif
	}
{
	assert(this->mResolution.mWidth % TILE_WIDTH == 0, "DepthBuffer's size should be multiple of TILE_WIDTH");
	assert(this->mResolution.mHeight % TILE_HEIGHT == 0, "DepthBuffer's size should be multiple of TILE_HEIGHT");

	//assert(mResolution.mWidth% SUB_TILE_WIDTH == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_WIDTH");
	//assert(mResolution.mHeight% SUB_TILE_HEIGHT == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_HEIGHT");
	
	const size_t tileCountInARow = this->mResolution.mWidth / TILE_WIDTH;
	const size_t tileCountInAColumn = this->mResolution.mHeight / TILE_HEIGHT;

	// TODO : change to Aligned dynamic allocation ( aligned malloc ) 
	// In current, Tile struct is allocated contiguously, It incur False Sharing!!!!!!!!! 
	this->mTiles = new Tile[tileCountInARow * tileCountInAColumn];
}

culling::SWDepthBuffer::~SWDepthBuffer()
{
	// TODO : change to Aligned dynamic deallocation
	delete[] this->mTiles;
}

/*
void culling::InitializeTriangleBin(TileBin& triangleBin)
{
	for (size_t y = 0; y < TILE_HEIGHT; y++)
	{
		for (size_t x = 0; x < TILE_WIDTH; x++)
		{
			triangleBin.mBinnedTriangleList[x][y].mTriangleList = std::unique_ptr<TwoDTriangle[]>(new TwoDTriangle[BIN_TRIANGLE_CAPACITY_PER_TILE]);
		}
	}
}
*/