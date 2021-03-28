#pragma once

#include <cassert>
#include <Vector3.h>

#include "TransformData.h"
#include "EntityHandle.h"
#include "EntityBlock.h"

namespace doom
{
	namespace graphics
	{
		class EntityBlockViewer
		{
			friend class LinearTransformDataCulling;

		private:

			bool bmIsActive;
			EntityBlock* mTargetEntityBlock;
			unsigned int mEntityIndexInBlock;

		public:

			EntityBlockViewer() = default;
			EntityBlockViewer(EntityBlock* entityBlock, unsigned int entityIndexInBlock)
				: mTargetEntityBlock{ entityBlock }, mEntityIndexInBlock{ entityIndexInBlock }, bmIsActive{ true }
			{}

			FORCE_INLINE void SetEntityPosition(const math::Vector3& worldPosition)
			{
				assert(this->bmIsActive == true);
				std::memcpy((mTargetEntityBlock->mPositions + mEntityIndexInBlock)->data(), worldPosition.data(), sizeof(math::Vector3));
			}

			FORCE_INLINE void SetSphereBoundRadius(float sphereRadius)
			{
				assert(this->bmIsActive == true);
				mTargetEntityBlock->mPositions[mEntityIndexInBlock].w = sphereRadius;
			}

			FORCE_INLINE TransformData* GetTransformData() const
			{
				assert(this->bmIsActive == true);
				return mTargetEntityBlock->mTransformDatas + mEntityIndexInBlock;
			}

			FORCE_INLINE EntityHandle* GetEntityHandle() const
			{
				assert(this->bmIsActive == true);
				return mTargetEntityBlock->mHandles + mEntityIndexInBlock;
			}

			/// <summary>
			/// Get if entity is visible from all camera
			/// return true when all camera can see this entity ( not culled )
			/// </summary>
			/// <returns></returns>
			FORCE_INLINE bool GetIsVisibleBitflag() const
			{
				assert(this->bmIsActive == true);
				return mTargetEntityBlock->mIsVisibleBitflag[mEntityIndexInBlock];
			}
			/// <summary>
			/// Get if entity is visible from Camera of parameter cameraIndex
			/// </summary>
			/// <param name="cameraIndex">0 <= cameraIndex < LinearTransformDataCulling::mCameraCount</param>
			/// <returns></returns>
			FORCE_INLINE char GetIsVisibleBitflag(unsigned int cameraIndex) const
			{
				assert(this->bmIsActive == true);
				assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
				return mTargetEntityBlock->mIsVisibleBitflag[mEntityIndexInBlock] & (1 << cameraIndex);
			}
		};
	}
}