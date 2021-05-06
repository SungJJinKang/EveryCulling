#pragma once

#include "../../FrotbiteCullingSystemCore.h"

#include <memory>

#include "../../SIMD_Core.h"
#include "../../DataType/Math/Triangle.h"

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
		/// Binned Screen Pixel Space Triangles
		/// </summary>
		ThreeDTriangle mTriangleList[BIN_TRIANGLE_CAPACITY_PER_TILE];

		size_t mCurrentTriangleCount = 0;
	};

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
		const unsigned int mTileCountInARow;
		const unsigned int mTileCountInAColumn;

		/// <summary>
		/// 0
		/// </summary>
		const unsigned int mLeftBottomTileOrginX;
		
		/// <summary>
		/// 0
		/// </summary>
		const unsigned int mLeftBottomTileOrginY;
		
		/// <summary>
		/// Left Bottom PointX of Right Top Tile
		/// if Buffer Width is 37 -> 32
		/// if Buffer Width is 64 -> 32
		/// </summary>
		const unsigned int mRightTopTileOrginX;
		
		/// <summary>
		/// Left Bottom PointY of Right Top Tile
		/// if Buffer Width is 37 -> 32
		/// if Buffer Width is 64 -> 32
		/// </summary>
		const unsigned int mRightTopTileOrginY;

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
		/// 
		/// Left Bottom is (0, 0)
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