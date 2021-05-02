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

	struct TriangleBin
	{
		TwoDTriangle mClipSpaceTriangles[TRIANGLE_BIN_CAPACITY_PER_TILE];
	};


	/// <summary>
	/// AOS
	/// </summary>
	struct SubTile
	{
		HizData* mHizDatas;
		TriangleBin* mTriangleBins;
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