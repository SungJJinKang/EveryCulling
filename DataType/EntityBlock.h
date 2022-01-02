#pragma once

#include "../EveryCullingCore.h"

#include "Math/Vector.h"
#include "Position_BoundingSphereRadius.h"
#include "VertexData.h"
#include "Math/Matrix.h"

#ifdef ENABLE_QUERY_OCCLUSION
#include "QueryObject.h"

#define QUERY_OBJECT_PTR_SIZE sizeof(culling::QueryObject*)

#else

#define QUERY_OBJECT_PTR_SIZE 0

#endif




#define CACHE_LINE_SIZE 64
#define PAGE_SIZE 4096

#define MAKE_EVEN_NUMBER(X) (X - (X%2))

namespace culling
{

	//This code doesn't consider Memory alignment optimzation.
	inline constexpr size_t ENTITY_COUNT_IN_ENTITY_BLOCK = 16;
	static_assert(ENTITY_COUNT_IN_ENTITY_BLOCK % 16 == 0);

	/// <summary>
	/// EntityBlock size should be less 4KB(Page size) for Block data being allocated in a page
	/// </summary>
	struct alignas(CACHE_LINE_SIZE) EntityBlock
	{
		/// <summary>
		/// You don't need to worry about false sharing.
		/// void* mRenderer[ENTITY_COUNT_IN_ENTITY_BLOCK] and mCurrentEntityCount isn't read during CullJob
		/// </summary>
		char mIsVisibleBitflag[ENTITY_COUNT_IN_ENTITY_BLOCK];
		
		/// <summary>
		/// x, y, z : components is position of entity
		/// w : component is radius of entity's sphere bound
		/// 
		/// This will be used for linearlly Frustum intersection check
		///
		/// If Size of mIsVisibleBitflag isn't multiples of 256bit,
		/// Setting mIsVisibleBitflag make mPositionAndBoundingSpheres value dirty
		/// </summary>
		culling::Position_BoundingSphereRadius mPositionAndBoundingSpheres[ENTITY_COUNT_IN_ENTITY_BLOCK];

#ifdef ENABLE_QUERY_OCCLUSION
		culling::QueryObject* mQueryObjects[ENTITY_COUNT_IN_ENTITY_BLOCK];
#endif
	
		/// <summary>
		/// Whether renderer component is enabled.
		/// </summary>
		bool mIsObjectEnabled[ENTITY_COUNT_IN_ENTITY_BLOCK];
		
		VertexData mVertexDatas[ENTITY_COUNT_IN_ENTITY_BLOCK];
		
		culling::Vec4 mAABBMinWorldPoint[ENTITY_COUNT_IN_ENTITY_BLOCK];
		culling::Vec4 mAABBMaxWorldPoint[ENTITY_COUNT_IN_ENTITY_BLOCK];
		culling::Mat4x4 mModelMatrixes[ENTITY_COUNT_IN_ENTITY_BLOCK];

		char mIsOccluder[ENTITY_COUNT_IN_ENTITY_BLOCK];
		
		/// <summary>
		/// this variable is only used to decide whether to free this EntityBlock
		/// </summary>
		std::uint32_t mCurrentEntityCount;

		FORCE_INLINE bool GetIsCulled(const size_t entityIndex, const size_t cameraIndex) const
		{
			assert(entityIndex < mCurrentEntityCount);

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
		FORCE_INLINE void UpdateIsCulled(const size_t entityIndex, const size_t cameraIndex, const bool isCullded)
		{
			// Setting value to invalid index is acceptable
			assert(entityIndex < ENTITY_COUNT_IN_ENTITY_BLOCK);

			const char cullMask = ~(((char)isCullded) << cameraIndex);

			mIsVisibleBitflag[entityIndex] &= cullMask;
		}

		FORCE_INLINE void SetCulled(const size_t entityIndex, const size_t cameraIndex)
		{
			assert(entityIndex < mCurrentEntityCount);

			mIsVisibleBitflag[entityIndex] &= ~(1 << cameraIndex);
		}

		FORCE_INLINE void SetNotCulled(const size_t entityIndex, const size_t cameraIndex)
		{
			assert(entityIndex < mCurrentEntityCount);

			mIsVisibleBitflag[entityIndex] |= (1 << cameraIndex);
		}

		FORCE_INLINE void SetIsObjectEnabled(const size_t entityIndex, const bool isEnabled)
		{
			assert(entityIndex < mCurrentEntityCount);

			mIsObjectEnabled[entityIndex] = isEnabled;
		}
		FORCE_INLINE bool GetIsObjectEnabled(const size_t entityIndex) const
		{
			assert(entityIndex < mCurrentEntityCount);

			return mIsObjectEnabled[entityIndex];
		}
		
		FORCE_INLINE bool GetIsOccluder(const size_t entityIndex, const size_t cameraIndex) const
		{
			assert(entityIndex < mCurrentEntityCount);

			return (mIsOccluder[entityIndex] & (1 << cameraIndex)) != 0;
		}

		FORCE_INLINE void SetIsOccluder(const size_t entityIndex, const size_t cameraIndex, const bool isOccluder)
		{
			assert(entityIndex < mCurrentEntityCount);

			if(isOccluder == true)
			{
				mIsOccluder[entityIndex] |= (1 << cameraIndex);
			}
			else
			{
				mIsOccluder[entityIndex] &= (~(1 << cameraIndex));
			}
		}

		FORCE_INLINE void SetModelMatrix(const size_t entityIndex, const float* const modelToClipspaceMatrix)
		{
			assert(entityIndex < mCurrentEntityCount);

			std::memcpy(mModelMatrixes + entityIndex, modelToClipspaceMatrix, sizeof(culling::Mat4x4));
		}
		FORCE_INLINE const culling::Mat4x4& GetModelMatrix(const size_t entityIndex) const
		{
			assert(entityIndex < mCurrentEntityCount);

			return mModelMatrixes[entityIndex];
		}

		FORCE_INLINE void UpdateBoundingSphereRadius(const size_t entityIndex)
		{
			assert(entityIndex < mCurrentEntityCount);

			const culling::Vec4 vec = mAABBMaxWorldPoint[entityIndex] - mAABBMinWorldPoint[entityIndex];
			
			mPositionAndBoundingSpheres[entityIndex].SetBoundingSphereRadius(vec.magnitude());
		}

		FORCE_INLINE void SetAABBWorldPosition(const size_t entityIndex, const float* const minWorldPos, const float* const maxWorldPos)
		{
			assert(entityIndex < mCurrentEntityCount);

			std::memcpy(mAABBMinWorldPoint + entityIndex, minWorldPos, sizeof(culling::Vec4));
			std::memcpy(mAABBMaxWorldPoint + entityIndex, maxWorldPos, sizeof(culling::Vec4));
		}
	};

	/// <summary>
	/// Size of Entity block should be less than 4kb(page size)
	/// </summary>
	static_assert(sizeof(EntityBlock) < PAGE_SIZE);

	/// <summary>
	/// ENTITY_COUNT_IN_ENTITY_BLOCK should be even number
	/// </summary>
	static_assert(ENTITY_COUNT_IN_ENTITY_BLOCK > 0 && ENTITY_COUNT_IN_ENTITY_BLOCK % 2 == 0);
}
