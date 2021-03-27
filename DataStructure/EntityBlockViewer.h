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

			FORCE_INLINE bool GetIsVisibleBitflag() const
			{
				assert(this->bmIsActive == true);
				return mTargetEntityBlock->mIsVisibleBitflag[mEntityIndexInBlock];
			}
		};
	}
}