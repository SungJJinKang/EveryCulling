#pragma once

#include "../EveryCullingCore.h"

#include <atomic>

#include "Math/Vector.h"
#include "Position_BoundingSphereRadius.h"
#include "VertexData.h"
#include "Math/Matrix.h"

#define EVERYCULLING_PAGE_SIZE 4096
#define EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK 16

namespace culling
{
	/// <summary>
	/// EntityBlock size should be less 4KB(Page size) for Block data being allocated in a page
	/// </summary>
	struct alignas(EVERYCULLING_CACHE_LINE_SIZE) EntityBlock
	{
		/// <summary>
		/// You don't need to worry about false sharing.
		/// void* mRenderer[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK] and mCurrentEntityCount isn't read during CullJob
		/// </summary>
		char mIsVisibleBitflag[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];

		/// <summary>
		/// x, y, z : components is position of entity
		/// w : component is radius of entity's sphere bound
		/// 
		/// Writeen in Pre Culling, Read in ViewFrustum Culling, Distance Culling
		/// </summary>
		alignas(EVERYCULLING_CACHE_LINE_SIZE) culling::Position_BoundingSphereRadius mWorldPositionAndWorldBoundingSphereRadius[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK]; // 4 * 16 byte
		
		/**
		 * \brief Written in BinTriangleStage, Read in BinTriangleStage.
		 */
		VertexData mVertexDatas[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK]; // 4 * 16 byte

		// Written in PreCulling Stage ---------------------------------------------------------------------------------------------------

		// This variable is for a camera
		float mAABBMinScreenSpacePointX[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		float mAABBMinScreenSpacePointY[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		float mAABBMaxScreenSpacePointX[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		float mAABBMaxScreenSpacePointY[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		/// <summary>
		/// This values is set only when mIsAllAABBClipPointWPositive[entityIndex] is true
		/// </summary>
		float mAABBMinNDCZ[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		/// <summary>
		/// If All vertex's homogeneous w of object aabb is negative.
		///	So AABBScreenSpacePoint is invalid
		/// </summary>
		bool mIsAllAABBClipPointWPositive[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		bool mIsAllAABBClipPointWNegative[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		
		
		
		// Below variables is written(set) before start culling. -----------------------------------------------------------------------------

		alignas(EVERYCULLING_CACHE_LINE_SIZE) culling::Vec4 mAABBMinWorldPoint[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		culling::Vec4 mAABBMaxWorldPoint[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		culling::Mat4x4 mModelMatrixes[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		/// <summary>
		/// Whether renderer component is enabled.
		/// </summary>
		bool mIsObjectEnabled[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];
		float mDesiredMaxDrawDistance[EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK];

		/// <summary>
		/// this variable is only used to decide whether to free this EntityBlock
		/// </summary>
		std::uint32_t mCurrentEntityCount;

		// ----------------------------------------------------------------------------------------------------------------------

		EVERYCULLING_FORCE_INLINE bool GetIsAllAABBClipPointWNegative(const size_t entityIndex) const
		{
			return mIsAllAABBClipPointWNegative[entityIndex];
		}

		EVERYCULLING_FORCE_INLINE void SetIsAllAABBClipPointWNegative(const size_t entityIndex, const bool isAllAABBClipPointWNegative)
		{
			// Setting value to invalid index is acceptable
			assert(entityIndex < EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK);

			mIsAllAABBClipPointWNegative[entityIndex] = isAllAABBClipPointWNegative;
		}

		EVERYCULLING_FORCE_INLINE bool GetIsAnyAABBClipPointWNegative(const size_t entityIndex) const
		{
			return (mIsAllAABBClipPointWPositive[entityIndex] == false);
		}

		EVERYCULLING_FORCE_INLINE bool GetIsAllAABBClipPointWPositive(const size_t entityIndex) const
		{
			return mIsAllAABBClipPointWPositive[entityIndex];
		}

		/// <summary>
		/// Update IsMinNDCZDataUsedForQuery without branch
		///
		///	If isMinNDCZDataUsedForQueryMask flag is already 0, it preserve value.
		///	If isMinNDCZDataUsedForQueryMask flag is 1, visibility flag is updated based on isMinNDCZDataUsedForQuery parameter
		/// </summary>
		/// <param name="entityIndex"></param>
		/// <param name="cameraIndex"></param>
		/// <param name="isAllAABBClipPointWPositive"></param>
		/// <returns></returns>
		EVERYCULLING_FORCE_INLINE void SetIsAllAABBClipPointWPositive(const size_t entityIndex, const bool isAllAABBClipPointWPositive)
		{
			// Setting value to invalid index is acceptable
			assert(entityIndex < EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK);
			
			mIsAllAABBClipPointWPositive[entityIndex] = isAllAABBClipPointWPositive;
		}

		EVERYCULLING_FORCE_INLINE bool GetIsCulled(const size_t entityIndex, const size_t cameraIndex) const
		{
			return ( mIsVisibleBitflag[entityIndex] & (1 << cameraIndex) ) == 0;
		}

		/// <summary>
		/// Update IsCulled without branch
		///
		///	If visiblity flag is already 0, it preserve value.
		///	If visiblity flag is 1, visibility flag is updated based on isCulled parameter
		/// </summary>
		/// <param name="entityIndex"></param>
		/// <param name="cameraIndex"></param>
		/// <param name="isCullded"></param>
		/// <returns></returns>
		EVERYCULLING_FORCE_INLINE void UpdateIsCulled(const size_t entityIndex, const size_t cameraIndex, const bool isCullded)
		{
			// Setting value to invalid index is acceptable
			assert(entityIndex < EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK);

			const char cullMask = ~((char)isCullded << cameraIndex);

			mIsVisibleBitflag[entityIndex] &= cullMask;
		}

		EVERYCULLING_FORCE_INLINE void SetCulled(const size_t entityIndex, const size_t cameraIndex)
		{
			mIsVisibleBitflag[entityIndex] &= ~(1 << cameraIndex);
		}

		EVERYCULLING_FORCE_INLINE void SetNotCulled(const size_t entityIndex, const size_t cameraIndex)
		{
			mIsVisibleBitflag[entityIndex] |= (1 << cameraIndex);
		}

		EVERYCULLING_FORCE_INLINE void SetIsObjectEnabled(const size_t entityIndex, const bool isEnabled)
		{
			mIsObjectEnabled[entityIndex] = isEnabled;
		}
		EVERYCULLING_FORCE_INLINE bool GetIsObjectEnabled(const size_t entityIndex) const
		{
			return mIsObjectEnabled[entityIndex];
		}
		
		EVERYCULLING_FORCE_INLINE void SetModelMatrix(const size_t entityIndex, const float* const modelToClipspaceMatrix)
		{
			std::memcpy(mModelMatrixes + entityIndex, modelToClipspaceMatrix, sizeof(culling::Mat4x4));
		}
		EVERYCULLING_FORCE_INLINE const culling::Mat4x4& GetModelMatrix(const size_t entityIndex) const
		{
			return mModelMatrixes[entityIndex];
		}

		EVERYCULLING_FORCE_INLINE void UpdateBoundingSphereRadius(const size_t entityIndex)
		{
			culling::Vec4 vec = mAABBMaxWorldPoint[entityIndex] - mAABBMinWorldPoint[entityIndex];
			vec[3] = 1.0f;
			mWorldPositionAndWorldBoundingSphereRadius[entityIndex].SetBoundingSphereRadius(vec.magnitude() * 0.5f);
		}

		EVERYCULLING_FORCE_INLINE const culling::Position_BoundingSphereRadius& GetEntityWorldPositionAndBoudingSphereRadius(const size_t entityIndex) const
		{
			return mWorldPositionAndWorldBoundingSphereRadius[entityIndex];
		}

		EVERYCULLING_FORCE_INLINE void SetAABBWorldPosition(const size_t entityIndex, const float* const minWorldPos, const float* const maxWorldPos)
		{
			std::memcpy(mAABBMinWorldPoint + entityIndex, minWorldPos, sizeof(culling::Vec4));
			std::memcpy(mAABBMaxWorldPoint + entityIndex, maxWorldPos, sizeof(culling::Vec4));
		}
		
		EVERYCULLING_FORCE_INLINE void ResetEntityBlock(const unsigned long long currentTickCount)
		{
			for(size_t entityIndex = 0 ; entityIndex < EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK ; entityIndex++)
			{
				mVertexDatas[entityIndex].Reset(currentTickCount);
			}
			std::memset(mIsVisibleBitflag, 0xFF, sizeof(char) * EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK);
			std::memset(mIsAllAABBClipPointWPositive, 0xFF, sizeof(char) * EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK);
		}

		EVERYCULLING_FORCE_INLINE void SetDesiredMaxDrawDistance(const size_t entityIndex, const float desiredMaxDrawDistance)
		{
			assert(desiredMaxDrawDistance >= 0.0f);

			mDesiredMaxDrawDistance[entityIndex] = desiredMaxDrawDistance;
		}

		EVERYCULLING_FORCE_INLINE float GetDesiredMaxDrawDistance(const size_t entityIndex)
		{
			assert(mDesiredMaxDrawDistance[entityIndex] >= 0.0f);

			return mDesiredMaxDrawDistance[entityIndex];
		}
		
		EVERYCULLING_FORCE_INLINE const culling::VertexData& GetVertexData(const size_t entityIndex) const
		{
			return mVertexDatas[entityIndex];
		}

		void ClearEntityBlock();
	};



	/// <summary>
	/// Size of Entity block should be less than 4kb(page size)
	/// </summary>
	static_assert(sizeof(EntityBlock) < EVERYCULLING_PAGE_SIZE);
	static_assert(sizeof(culling::Position_BoundingSphereRadius) == 16);
	/// <summary>
	/// EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK should be even number
	/// </summary>
	static_assert(EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK > 0 && EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK % 2 == 0);
}
