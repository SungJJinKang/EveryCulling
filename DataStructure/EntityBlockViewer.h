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
			FORCE_INLINE char GetIsVisibleBitflag() const
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


			FORCE_INLINE void SetEntityPosition(const math::Vector3& worldPosition)
			{
				assert(this->bmIsActive == true);
				std::memcpy((mTargetEntityBlock->mPositions + mEntityIndexInBlock)->data(), worldPosition.data(), sizeof(math::Vector3));
			}


			FORCE_INLINE void SetWorldAABB(const math::Vector3& minPoint, const math::Vector3& maxPoint)
			{
				assert(this->bmIsActive == true);
				TransformData* transformData = this->GetTransformData();
				transformData->mMinAABBPoint = minPoint;
				transformData->mMaxAABBPoint = maxPoint;
			}

			FORCE_INLINE void SetEntityRotation(const math::Quaternion& rotation)
			{
				assert(this->bmIsActive == true);
				TransformData* transformData = this->GetTransformData();
				transformData->mRotation = rotation;
			}

			FORCE_INLINE void SetEntityScale(const math::Vector3& scale)
			{
				assert(this->bmIsActive == true);
				TransformData* transformData = this->GetTransformData();
				transformData->mScale = scale;
			}

			FORCE_INLINE void SetSphereBoundRadius(float sphereRadius)
			{
				assert(this->bmIsActive == true);
				assert(sphereRadius >= 0.0f);

				// WHY NEGATIVE??
				// Think Sphere is on(!!) frustum plane. But it still should be drawd
				// Distance from plane to EntityPoint is negative.
				// If Distance from plane to EntityPoint is larget than negative radius, it should be drawed
				mTargetEntityBlock->mPositions[mEntityIndexInBlock].w = -(sphereRadius + BOUNDING_SPHRE_RADIUS_MARGIN);
			}
		};
	}
}