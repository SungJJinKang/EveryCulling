#pragma once

#include "../../FrotbiteCullingSystemCore.h"

#include <memory>

#include "../../SIMD_Core.h"
#include "Triangle.h"

namespace culling
{
	
	/// <summary>
	/// Hierarchical depth for a Tile ( 32 X 8 )
	/// </summary>
	struct HizData
	{
		/// <summary>
		/// Depth value of subtiles
		/// 8 floating-point = SubTile Count ( 8 )
		/// 
		/// A floating-point value represent Z0 Max DepthValue of A Subtile
		/// 
		/// </summary>
		M256F z0Max;

		/// <summary>
		/// Depth value of subtiles
		/// 8 floating-point = SubTile Count ( 8 )
		/// 
		/// A floating-point value represent Z0 Max DepthValue of A Subtile
		/// 
		/// </summary>
		M256F z1Max;

		/// <summary>
		/// Posion where Current Z1 Depth MaxValue come from
		/// 
		/// 4 byte * 8 byte
		/// 
		/// 4 byte -> 32 bit * 4 -> ( 32 = 8 X 4 = SubTile PixelCount )
		/// if first bit is 1, (0, 0) in subtile is position where Current Z1 Depth MaxValue come from
		/// 
		/// 8 byte -> Subtile Count in A Tile
		/// 
		/// </summary>
		M256I depthPosition;
	};
	
	/// <summary>
	/// TriangleList, Bin
	/// 
	/// TwoDTriangle should be reserved after initialized!!!!
	/// </summary>
	struct TriangleList
	{
		/// <summary>
		/// Binned screen sixel space Triangles
		/// </summary>
		TwoDTriangle mTriangleList[BIN_TRIANGLE_CAPACITY_PER_TILE];

		size_t mCurrentTriangleCount = 0;
	};

	/*
	struct TileBin
	{
		TriangleList mBinnedTriangleList[TILE_WIDTH][TILE_HEIGHT];
	};
	*/

	//void InitializeTriangleBin(TileBin& triangleBin);

	/// <summary>
	/// 32 X 8 Tile
	/// 
	/// alignas(64) is for preventing false sharing
	/// </summary>
	struct alignas(64) Tile
	{
		HizData mHizDatas;
		TriangleList mBinnedTriangles;
	};

	struct Resolution
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

		/// <summary>
		/// 32 X 8 Tiles
		/// </summary>
		Tile* mTiles;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="width">Screen Width</param>
		/// <param name="height">Scree Height</param>
		SWDepthBuffer(unsigned int width, unsigned int height);
		~SWDepthBuffer();
	};
}