#pragma once

#include "../../FrotbiteCullingSystemCore.h"

#include "../../SIMD_Core.h"

namespace culling
{
	/// <summary>
	/// DepthBufferTile
	/// This is a "Bin"
	/// </summary>
	struct SWDepthBufferTile
	{
		/// <summary>
		/// AVX1
		/// </summary>
		float mPixels[TILE_WIDTH * TILE_HEIGHT];
	};

	/// <summary>
	/// 8 * 4 SubTile Of 16 * 8 DepthBufferTile
	/// </summary>
	struct SWDepthBufferSubTile
	{
		/// <summary>
		/// 128bit =  16byte -> AVX1
		/// </summary>
		M128F z0Max[4];
		M128F z1Max[4];

		/// <summary>
		/// Bitmask
		/// Posion where depth value came from
		/// if second bit is 1, Depth value came from second pixel of subtile
		/// </summary>
		M128I depthPosiiton[4];
	};

	class SWDepthBuffer
	{
	public:
		const unsigned int mWidth;
		const unsigned int mHeight;

		const unsigned int mBinCountInRow;
		const unsigned int mBinCountInColumn;
		const unsigned int mTileCount;

		SWDepthBufferTile* mTiles;
		SWDepthBuffer(unsigned int width, unsigned int height)
			: mWidth{ width }, mHeight{ height }, mBinCountInRow{ width / TILE_WIDTH }, mBinCountInColumn{ height / TILE_HEIGHT }, mTileCount{ width / TILE_WIDTH * height / TILE_HEIGHT }
		{
			assert(mWidth % TILE_WIDTH == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_WIDTH");
			assert(mHeight % TILE_HEIGHT == 0, "DepthBuffer's size should be multiple of DEPTH_BUFFER_TITL_HEIGHT");

			this->mTiles = new SWDepthBufferTile[static_cast<size_t>(mTileCount)];
		}
	};
}