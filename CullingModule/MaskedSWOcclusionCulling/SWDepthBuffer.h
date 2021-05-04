#pragma once

#include "../../FrotbiteCullingSystemCore.h"

#include "../../SIMD_Core.h"
#include "Triangle.h"

namespace culling
{
	
	/// <summary>
	/// 
	/// One 32 * 8 Tile is composed of 8 Subtile(8 * 4)
	/// 
	/// 8 * 4 SubTile Of 32 * 8 DepthBufferTile
	/// </summary>
	struct alignas(64) HizData
	{
		/// <summary>
		/// 8 floating-point = SubTile Count ( 8 )
		/// </summary>
		M256F z0Max;
		M256F z1Max;

		/// <summary>
		/// Bitmask
		/// Posion where depth value came from
		/// if second bit is 1, Depth value came from second pixel of subtile
		/// </summary>
		unsigned int depthPosition;
	};
	
	/// <summary>
	/// TriangleList
	/// 
	/// TwoDTriangle should be reserved after initialized!!!!
	/// </summary>
	struct TriangleList
	{
		const size_t mMaxTriangleCount = MAX_TRIANGLE_COUNT_PER_TILE;
		size_t mCurrentTriangleIndex = 0;
		/// <summary>
		/// Screen Pixel Space Triangles
		/// 
		/// Dont' use std::unique_ptr
		/// </summary>
		TwoDTriangle* mTriangleList;
	};

	struct TileBin
	{
		TriangleList mBinnedTriangleList[TILE_WIDTH][TILE_HEIGHT];
	};

	void InitializeTriangleBin(TileBin& triangleBin)
	{
		for (size_t y = 0; y < TILE_HEIGHT; y++)
		{
			for (size_t x = 0; x < TILE_WIDTH; x++)
			{
				//triangleBin.mBinnedTriangleList[x][y].mTriangleList = std::make_uniqe
			}
		}
	}

	/// <summary>
	/// AOS
	/// </summary>
	struct SubTile
	{
		HizData* mHizDatas;
		TileBin* mTriangleBins;
	};

	struct alignas(64) Resolution
	{
		const unsigned int mWidth;
		const unsigned int mHeight;

#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
		M256F mReplicatedScreenHalfWidth;
		M256F mReplicatedScreenHalfHeight;
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
		M256F mReplicatedScreenWidth;
		M256F mReplicatedScreenHeight;
#endif
		
	};

	class SWDepthBuffer
	{
		friend class MaskedSWOcclusionCulling;
		friend class BinTrianglesStage;

	private:

		const Resolution mResolution;

		const unsigned int mBinCountInRow;
		const unsigned int mBinCountInColumn;
		const unsigned int mSubTileCount;

		SubTile mSubTiles;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="width">Screen Width</param>
		/// <param name="height">Scree Height</param>
		SWDepthBuffer(unsigned int width, unsigned int height);
	};
}