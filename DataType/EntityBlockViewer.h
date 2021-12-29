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
		std::uint32_t mEntityIndexInBlock;

	public:

		EntityBlockViewer();
		EntityBlockViewer(EntityBlock* const entityBlock, const std::uint32_t entityIndexInBlock);

		FORCE_INLINE bool GetIsActive() const
		{
			return bmIsActive;
		}

		/// <summary>
		/// Get if entity is visible from Camera of parameter cameraIndex
		/// </summary>
		/// <param name="cameraIndex">0 <= cameraIndex < EveryCulling::mCameraCount</param>
		/// <returns></returns>
		FORCE_INLINE bool GetIsCulled(const std::uint32_t cameraIndex) const
		{
			assert(bmIsActive == true);
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mTargetEntityBlock->GetIsCulled(mEntityIndexInBlock, cameraIndex);
		}


		void SetEntityPosition(const float* worldPosition);

		void SetSphereBoundRadius(float sphereRadius);

		void SetMeshVertexData
		(
			const culling::Vec3* const vertices,
			const size_t verticeCount,
			const std::uint32_t* const indices,
			const size_t indiceCount,
			const size_t verticeStride,
			const culling::Vec3 aabbMinLocalPoint,
			const culling::Vec3 aabbMaxLocalPoint
		);
		
		void SetModelMatrix(const float* const modelMatrix);
		FORCE_INLINE const float* GetModelMatrix() const
		{
			return mTargetEntityBlock->mModelMatrixes[mEntityIndexInBlock];
		}
		FORCE_INLINE const culling::VertexData& GetVertexData() const
		{
			return mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock];
		}
		FORCE_INLINE bool GetIsOccluder(const size_t cameraIndex) const
		{
			return mTargetEntityBlock->GetIsOccluder(mEntityIndexInBlock, cameraIndex);
		}

		FORCE_INLINE void SetIsObjectEnabled(const bool isEnabled)
		{
			if (GetIsActive() == true)
			{
				mTargetEntityBlock->SetIsObjectEnabled(mEntityIndexInBlock, isEnabled);
			}
		}
	};
}
