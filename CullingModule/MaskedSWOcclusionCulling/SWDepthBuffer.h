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
		///
		/// </summary>
		culling::M256F L0MaxDepthValue;

		/// <summary>
		/// Depth value of subtiles
		/// 8 floating-point = SubTile Count ( 8 )
		/// 
		/// A floating-point value represent Z0 Max DepthValue of A Subtile
		/// 
		/// </summary>
		culling::M256F L1MaxDepthValue;

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
		//culling::M256I depthPosition;


		/*
		 
		// 44444444 55555555 66666666 77777777
		// 44444444 55555555 66666666 77777777
		// 44444444 55555555 66666666 77777777
		// 44444444 55555555 66666666 77777777
		// 
		// 00000000 11111111 22222222 33333333
		// 00000000 11111111 22222222 33333333
		// 00000000 11111111 22222222 33333333
		// 00000000 11111111 22222222 33333333
		//
		// --> 256bit
		//
		//
		//
		// 0 : CoverageMask ( 0 ~ 32 )
		// 1 : CoverageMask ( 32 ~ 64 )
		// 2 : CoverageMask ( 64 ~ 96 )
		// 3 : CoverageMask ( 96 ~ 128 )
		// 4 : CoverageMask ( 128 ~ 160 )
		// 5 : CoverageMask ( 160 ~ 192 )
		// 6 : CoverageMask ( 192 ~ 224 )
		// 7 : CoverageMask ( 224 ~ 256 )
			
		 */
		culling::M256I L1CoverageMask;

		void Reset();

		void ClearCoverageMaskAllSubTile();
		FORCE_INLINE void ClearCoverageMask(const size_t subTileIndex)
		{
			assert(subTileIndex < 8);
			(reinterpret_cast<std::uint32_t*>(&L1CoverageMask))[subTileIndex] = 0x00000000;
		}
		void FillCoverageMask();

		FORCE_INLINE void ClearL1MaxDepthValueAllSubTile()
		{
			L1MaxDepthValue = _mm256_set1_ps(0.0f);
		}

		FORCE_INLINE void ClearL1MaxDepthValue(const size_t subTileIndex)
		{
			assert(subTileIndex < 8);
			(reinterpret_cast<std::uint32_t*>(&L1MaxDepthValue))[subTileIndex] = 0x00000000;
		}

		

		/// <summary>
		///
		// 44444444 55555555 66666666 77777777
		// 44444444 55555555 66666666 77777777
		// 44444444 55555555 66666666 77777777
		// 44444444 55555555 66666666 77777777
		// 
		// 00000000 11111111 22222222 33333333
		// 00000000 11111111 22222222 33333333
		// 00000000 11111111 22222222 33333333
		// 00000000 11111111 22222222 33333333
		//
		//
		// --> 256bit
		//
		//
		//
		// 0 : CoverageMask ( 0 ~ 32 )
		// 1 : CoverageMask ( 32 ~ 64 )
		// 2 : CoverageMask ( 64 ~ 96 )
		// 3 : CoverageMask ( 96 ~ 128 )
		// 4 : CoverageMask ( 128 ~ 160 )
		// 5 : CoverageMask ( 160 ~ 192 )
		// 6 : CoverageMask ( 192 ~ 224 )
		// 7 : CoverageMask ( 224 ~ 256 )
		//
		/// </summary>
		/// <param name="subTileIndex"></param>
		/// <returns></returns>
		FORCE_INLINE bool IsCoverageMaskFullByOne(const size_t subTileIndex) const
		{
			assert(subTileIndex < 8);
			return reinterpret_cast<const std::uint32_t*>(&L1CoverageMask)[subTileIndex] == 0xFFFFFFFF;
		}
		
		/*
		FORCE_INLINE void OverriteCoverageMask(const size_t subTileIndex, const std::uint32_t tileCoverage)
		{
			reinterpret_cast<std::uint32_t*>(&L1CoverageMask)[subTileIndex] = tileCoverage;
		}
		*/

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
		const std::uint32_t mRowTileCount;
		const std::uint32_t mColumnTileCount;
		const std::uint32_t mRowSubTileCount;
		const std::uint32_t mColumnSubTileCount;

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
			mRowTileCount(rowCount),
			mColumnTileCount(columnCount),
			mLeftBottomTileOrginX(leftBottomTileOrginX),
			mLeftBottomTileOrginY(leftBottomTileOrginY),
			mRightTopTileOrginX(rightTopTileOrginX),
			mRightTopTileOrginY(rightTopTileOrginY),
			mReplicatedScreenHalfWidth(replicatedScreenHalfWidth),
			mReplicatedScreenHalfHeight(replicatedScreenHeight),

			mRowSubTileCount(mRowTileCount * (TILE_HEIGHT / SUB_TILE_HEIGHT)),
			mColumnSubTileCount(mColumnTileCount * (TILE_WIDTH / SUB_TILE_WIDTH))
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