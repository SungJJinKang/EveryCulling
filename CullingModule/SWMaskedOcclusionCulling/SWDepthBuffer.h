#pragma once

#include "../../FrotbiteCullingSystemCore.h"


namespace culling
{
	struct SWDepthBufferTile
	{
		/// <summary>
		/// AVX1
		/// </summary>
		float mPixels[TILE_WIDTH * TILE_HEIGHT];
	};

	class SWDepthBuffer
	{
	public:
		const unsigned int mWidth;
		const unsigned int mHeight;
		const unsigned int mTileCount;

		SWDepthBufferTile* mTiles;
		SWDepthBuffer(unsigned int width, unsigned int height)
			: mWidth{ width }, mHeight{ height }, mTileCount{ width / TILE_WIDTH * height / TILE_HEIGHT }
		{
			assert(mWidth% TILE_WIDTH == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_WIDTH");
			assert(mHeight% TILE_HEIGHT == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_HEIGHT");

			this->mTiles = new SWDepthBufferTile[static_cast<size_t>(mTileCount)];
		}
	};
}