#pragma once

#include <cassert>
#include "EntityBlock.h"

namespace culling
{
	struct EntityBlock;
	class QueryOcclusionCulling;

	/// <summary>
	/// Used for storing specific EntityBlock pointer
	/// </summary>
	class EntityBlockViewer
	{
		friend class EveryCulling;
	private:

		bool bmIsActive;
		EntityBlock* mTargetEntityBlock;
		/// <summary>
		/// Entity Index in Entity Block
		/// </summary>
		size_t mEntityIndexInBlock;

		void ResetEntityData();

	public:

		EntityBlockViewer();
		EntityBlockViewer(EntityBlock* const entityBlock, const size_t entityIndexInBlock);

		EVERYCULLING_FORCE_INLINE bool GetIsActive() const
		{
			return bmIsActive;
		}

		EVERYCULLING_FORCE_INLINE EntityBlock* GetTargetEntityBlock()
		{
			assert(GetIsActive() == true);
			return mTargetEntityBlock;
		}

		EVERYCULLING_FORCE_INLINE const EntityBlock* GetTargetEntityBlock() const
		{
			assert(GetIsActive() == true);
			return mTargetEntityBlock;
		}

		EVERYCULLING_FORCE_INLINE size_t GetEntityIndexInBlock() const
		{
			assert(GetIsActive() == true);
			return mEntityIndexInBlock;
		}
		
		/// <summary>
		/// Get if entity is visible from Camera of parameter cameraIndex
		/// </summary>
		/// <param name="cameraIndex">0 <= cameraIndex < EveryCulling::mCameraCount</param>
		/// <returns></returns>
		EVERYCULLING_FORCE_INLINE bool GetIsCulled(const std::uint32_t cameraIndex) const
		{
			assert(GetIsActive() == true);
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mTargetEntityBlock->GetIsCulled(mEntityIndexInBlock, cameraIndex);
		}

		EVERYCULLING_FORCE_INLINE void SetModelMatrix(const float* const modelMatrix)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetModelMatrix(mEntityIndexInBlock, modelMatrix);
			}
		}

		/**
		 * \brief Set entity's mesh vertex data.
		 * \param vertices 
		 * \param verticeCount 
		 * \param indices 
		 * \param indiceCount 
		 * \param verticeStride 
		 */
		void SetMeshVertexData
		(
			const culling::Vec3* const vertices,
			const size_t verticeCount,
			const std::uint32_t* const indices,
			const size_t indiceCount,
			const size_t verticeStride
		);
		
		EVERYCULLING_FORCE_INLINE const culling::VertexData& GetVertexData() const
		{
			assert(GetIsActive() == true);
			return mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock];
		}
		EVERYCULLING_FORCE_INLINE bool GetIsOccluder() const
		{
			assert(GetIsActive() == true);
			return mTargetEntityBlock->GetIsOccluder(mEntityIndexInBlock);
		}

		EVERYCULLING_FORCE_INLINE void SetIsObjectEnabled(const bool isEnabled)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetIsObjectEnabled(mEntityIndexInBlock, isEnabled);
			}
		}

		EVERYCULLING_FORCE_INLINE void SetAABBWorldPosition(const float* const minWorldPos, const float* const maxWorldPos)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetAABBWorldPosition(mEntityIndexInBlock, minWorldPos, maxWorldPos);
			}
		}

		EVERYCULLING_FORCE_INLINE void SetObjectWorldPosition(const float* const worldPos)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->mWorldPositionAndWorldBoundingSphereRadius[mEntityIndexInBlock].SetPosition(worldPos);
			}
		}

		EVERYCULLING_FORCE_INLINE void SetDesiredMaxDrawDistance(const float desiredMaxDrawDistance)
		{
			assert(GetIsActive() == true);

			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetDesiredMaxDrawDistance(mEntityIndexInBlock, desiredMaxDrawDistance);
			}
		}

		/// <summary>
		/// Update EntityData
		///	Should be updated every frame before cull job
		/// </summary>
		/// <param name="entityWorldPositionVec3"></param>
		/// <param name="aabbMinWorldPositionVec3"></param>
		/// <param name="aabbMaxWorldPositionVec3"></param>
		/// <param name="entityModelMatrix4x4"></param>
		/// <returns></returns>
		EVERYCULLING_FORCE_INLINE void UpdateEntityData
		(
			const float* const entityWorldPositionVec3, 
			const float* const aabbMinWorldPositionVec3,
			const float* const aabbMaxWorldPositionVec3,
			const float* const entityModelMatrix4x4
		)
		{
			SetObjectWorldPosition(entityWorldPositionVec3);
			SetAABBWorldPosition(aabbMinWorldPositionVec3, aabbMaxWorldPositionVec3);

			SetModelMatrix(entityModelMatrix4x4);
		}
	};
}
