#pragma once

#include "../../FrotbiteCullingSystemCore.h"

#include "../../SIMD_Core.h"
#include "Triangle.h"

namespace culling
{
	
	// TODO : 28 Byte is wasted per HizData because of memory alignment
	// This is really really huge!!!
	// But for prevent false sharing, We should do this
	// 
	// If A Thread write to continuous 14 HizData, We will waste just 

	/// <summary>
	/// 
	/// One 16 * 8 Tile is composed of 4 Subtile(8 * 4)
	/// 
	/// 8 * 4 SubTile Of 16 * 8 DepthBufferTile
	/// </summary>
	struct alignas(64) HizData
	{
		/// <summary>
		/// = SubTile Count ( 4 )
		/// </summary>
		M128F z0Max;
		M128F z1Max;

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

	class SWDepthBuffer
	{
		friend class MaskedSWOcclusionCulling;

	private:

		const unsigned int mWidth;
		const unsigned int mHeight;

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