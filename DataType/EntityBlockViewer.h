#pragma once

#include <cassert>

#include "TransformData.h"
#include "EntityBlock.h"


namespace culling
{
	class EntityBlockViewer
	{
		friend class FrotbiteCullingSystem;

	private:

		bool bmIsActive;
		EntityBlock* mTargetEntityBlock;
		unsigned int mEntityIndexInBlock;

	public:

		EntityBlockViewer() = default;
		EntityBlockViewer(EntityBlock* entityBlock, unsigned int entityIndexInBlock)
			: mTargetEntityBlock{ entityBlock }, mEntityIndexInBlock{ entityIndexInBlock }, bmIsActive{ true }
		{}

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
		/// <param name="cameraIndex">0 <= cameraIndex < FrotbiteCullingSystem::mCameraCount</param>
		/// <returns></returns>
		FORCE_INLINE char GetIsVisibleBitflag(unsigned int cameraIndex) const
		{
			assert(this->bmIsActive == true);
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mTargetEntityBlock->mIsVisibleBitflag[mEntityIndexInBlock] & (1 << cameraIndex);
		}


		FORCE_INLINE void SetEntityPosition(const float* worldPosition)
		{
			assert(this->bmIsActive == true);
			std::memcpy((mTargetEntityBlock->mPositions + mEntityIndexInBlock), worldPosition, sizeof(Vector3));
		}

		FORCE_INLINE void SetSphereBoundRadius(float sphereRadius)
		{
			assert(this->bmIsActive == true);
			assert(sphereRadius >= 0.0f);

			// WHY NEGATIVE??
			// Think Sphere is on(!!) frustum plane. But it still should be drawd
			// Distance from plane to EntityPoint is negative.
			// If Distance from plane to EntityPoint is larget than negative radius, it should be drawed
			mTargetEntityBlock->mPositions[mEntityIndexInBlock].values[3] = -(sphereRadius + BOUNDING_SPHRE_RADIUS_MARGIN);
		}
	};
}
