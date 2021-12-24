#pragma once

#include "../../EveryCullingCore.h"

#include <atomic>

#include "../../DataType/Math/Triangle.h"

namespace culling
{
	class SWDepthBuffer;
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
		culling::M256F l0MaxDepthValue;

		/// <summary>
		/// Depth value of subtiles
		/// 8 floating-point = SubTile Count ( 8 )
		/// 
		/// A floating-point value represent Z0 Max DepthValue of A Subtile
		/// 
		/// </summary>
		culling::M256F l1MaxDepthValue;

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
		culling::M256I depthPosition;

		/// <summary>
		/// Coverage mask
		///
		/// index 0 : (0, 0) fragment is covered by L1 triangle
		/// index 1 : (1, 0) fragment is covered by L1 triangle
		/// index 2 : (2, 0) fragment is covered by L1 triangle
		///	.
		///	.
		///	.
		/// index 255 : (31, 7) fragment is covered by L1 triangle
		///
		///	1 bit mean 1 fragment is covered by L1 triangle
		/// </summary>
		culling::M256I l1CoverageMask;

		void Reset();

		void ClearCoverageMask();
		void FillCoverageMask();

		FORCE_INLINE void ClearL1MaxDepthValue()
		{
			l1MaxDepthValue = _mm256_set1_ps(0.0f);
		}

		FORCE_INLINE bool IsCoverageMaskFullByOne() const
		{
			return _mm256_testc_si256(l1CoverageMask, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)) == 1;
		}
		

	};
	
	/// <summary>
	/// TriangleList, Bin
	/// 
	/// TwoDTriangle should be reserved after initialized!!!!
	/// </summary>
	struct TriangleList
	{
		alignas(32) float VertexX[3][BIN_TRIANGLE_CAPACITY_PER_TILE]; // VertexX[0] : Point1 of Triangle, VertexX[1] : Point2 of Triangle, VertexX[2] : 3 of Triangle
		alignas(32) float VertexY[3][BIN_TRIANGLE_CAPACITY_PER_TILE];
		alignas(32) float VertexZ[3][BIN_TRIANGLE_CAPACITY_PER_TILE];

		size_t mCurrentTriangleCount = 0;

		void Reset();
	};

	static_assert(BIN_TRIANGLE_CAPACITY_PER_TILE % 8 == 0);

	/// <summary>
	/// 32 X 8 Tile
	/// 
	/// alignas(64) is for preventing false sharing
	/// </summary>
	class alignas(64) Tile
	{
		friend class SWDepthBuffer;
	private:
		
		std::uint32_t mLeftBottomTileOrginX = 0xFFFFFFFF;
		std::uint32_t mLeftBottomTileOrginY = 0xFFFFFFFF;
		
	public:

		HizData mHizDatas;
		TriangleList mBinnedTriangles;

		void Reset();
		FORCE_INLINE std::uint32_t GetLeftBottomTileOrginX() const
		{
			return mLeftBottomTileOrginX;
		}

		FORCE_INLINE std::uint32_t GetLeftBottomTileOrginY() const
		{
			return mLeftBottomTileOrginY;
		}
	};

	struct Resolution
	{
		const std::uint32_t mWidth;
		const std::uint32_t mHeight;
		const std::uint32_t mRowCount;
		const std::uint32_t mColumnCount;

		/// <summary>
		/// 0
		/// </summary>
		const std::uint32_t mLeftBottomTileOrginX;
		
		/// <summary>
		/// 0
		/// </summary>
		const std::uint32_t mLeftBottomTileOrginY;
		
		/// <summary>
		/// Left Bottom PointX of Right Top Tile
		/// if Buffer Width is 37 -> 32
		/// if Buffer Width is 64 -> 32
		/// </summary>
		const std::uint32_t mRightTopTileOrginX;
		
		/// <summary>
		/// Left Bottom PointY of Right Top Tile
		/// if Buffer Width is 37 -> 32
		/// if Buffer Width is 64 -> 32
		/// </summary>
		const std::uint32_t mRightTopTileOrginY;

#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
		culling::M256F mReplicatedScreenHalfWidth;
		culling::M256F mReplicatedScreenHalfHeight;
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
		culling::M256F mReplicatedScreenWidth;
		culling::M256F mReplicatedScreenHeight;
#endif

		Resolution
		(
			const std::uint32_t width,
			const std::uint32_t height,
			const std::uint32_t rowCount,
			const std::uint32_t columnCount,
			const std::uint32_t leftBottomTileOrginX,
			const std::uint32_t leftBottomTileOrginY,
			const std::uint32_t rightTopTileOrginX,
			const std::uint32_t rightTopTileOrginY,
			const culling::M256F replicatedScreenHalfWidth,
			const culling::M256F replicatedScreenHeight
		)
			:
			mWidth(width),
			mHeight(height),
			mRowCount(rowCount),
			mColumnCount(columnCount),
			mLeftBottomTileOrginX(leftBottomTileOrginX),
			mLeftBottomTileOrginY(leftBottomTileOrginY),
			mRightTopTileOrginX(rightTopTileOrginX),
			mRightTopTileOrginY(rightTopTileOrginY),
			mReplicatedScreenHalfWidth(replicatedScreenHalfWidth),
			mReplicatedScreenHalfHeight(replicatedScreenHeight)
		{
		}
	};

	class SWDepthBuffer
	{
	private:

		/// <summary>
		/// 32 X 8 Tiles
		/// 
		/// Left Bottom is (0, 0)
		/// </summary>
		Tile* mTiles;
		size_t mTileCount;


	public:

		const Resolution mResolution;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="width">Screen Width</param>
		/// <param name="height">Scree Height</param>
		SWDepthBuffer(std::uint32_t width, std::uint32_t height);

		~SWDepthBuffer();

		size_t GetTileCount() const;

		void Reset();
		
		const Tile* GetTiles() const;

		const Tile* GetTile(const size_t rowIndex, const size_t colIndex) const;
		Tile* GetTile(const size_t rowIndex, const size_t colIndex);
		const Tile* GetTile(const size_t tileIndex) const;
		Tile* GetTile(const size_t tileIndex);
	};
}