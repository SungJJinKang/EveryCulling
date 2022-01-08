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
		friend class QueryOcclusionCulling;

	private:

		bool bmIsActive;
		EntityBlock* mTargetEntityBlock;
		/// <summary>
		/// Entity Index in Entity Block
		/// </summary>
		size_t mEntityIndexInBlock;

	public:

		EntityBlockViewer();
		EntityBlockViewer(EntityBlock* const entityBlock, const size_t entityIndexInBlock);

		FORCE_INLINE bool GetIsActive() const
		{
			return bmIsActive;
		}

		FORCE_INLINE EntityBlock* GetTargetEntityBlock()
		{
			assert(GetIsActive() == true);
			return mTargetEntityBlock;
		}

		FORCE_INLINE size_t GetEntityIndexInBlock() const
		{
			assert(GetIsActive() == true);
			return mEntityIndexInBlock;
		}
		
		/// <summary>
		/// Get if entity is visible from Camera of parameter cameraIndex
		/// </summary>
		/// <param name="cameraIndex">0 <= cameraIndex < EveryCulling::mCameraCount</param>
		/// <returns></returns>
		FORCE_INLINE bool GetIsCulled(const std::uint32_t cameraIndex) const
		{
			assert(GetIsActive() == true);
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mTargetEntityBlock->GetIsCulled(mEntityIndexInBlock, cameraIndex);
		}

		FORCE_INLINE void SetModelMatrix(const float* const modelMatrix)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetModelMatrix(mEntityIndexInBlock, modelMatrix);
			}
		}

		void SetMeshVertexData
		(
			const culling::Vec3* const vertices,
			const size_t verticeCount,
			const std::uint32_t* const indices,
			const size_t indiceCount,
			const size_t verticeStride
		);
		
		FORCE_INLINE const culling::VertexData& GetVertexData() const
		{
			assert(GetIsActive() == true);
			return mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock];
		}
		FORCE_INLINE bool GetIsOccluder() const
		{
			assert(GetIsActive() == true);
			return mTargetEntityBlock->GetIsOccluder(mEntityIndexInBlock);
		}

		FORCE_INLINE void SetIsObjectEnabled(const bool isEnabled)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetIsObjectEnabled(mEntityIndexInBlock, isEnabled);
			}
		}

		FORCE_INLINE void SetAABBWorldPosition(const float* const minWorldPos, const float* const maxWorldPos)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetAABBWorldPosition(mEntityIndexInBlock, minWorldPos, maxWorldPos);
			}
		}

		FORCE_INLINE void SetObjectWorldPosition(const float* const worldPos)
		{
			assert(GetIsActive() == true);
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->mWorldPositionAndWorldBoundingSphereRadius[mEntityIndexInBlock].SetPosition(worldPos);
			}
		}

		FORCE_INLINE void SetDesiredMaxDrawDistance(const float desiredMaxDrawDistance)
		{
			assert(GetIsActive() == true);
			assert(entityIndex < mCurrentEntityCount);

			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetDesiredMaxDrawDistance(mEntityIndexInBlock, desiredMaxDrawDistance);
			}
		}
	};
}
