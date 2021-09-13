#include "EntityBlockViewer.h"

#include <cassert>
#include <cstring>

#include "EntityBlock.h"

bool culling::EntityBlockViewer::GetIsCulled(const unsigned int cameraIndex) const
{
	assert(bmIsActive == true);
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
	return mTargetEntityBlock->GetIsCulled(mEntityIndexInBlock, cameraIndex);
}


void culling::EntityBlockViewer::SetEntityPosition(const float* worldPosition)
{
	assert(bmIsActive == true);
	std::memcpy((mTargetEntityBlock->mPositions + mEntityIndexInBlock), worldPosition, sizeof(Vector3));
}

void culling::EntityBlockViewer::SetSphereBoundRadius(float sphereRadius)
{
	assert(bmIsActive == true);
	assert(sphereRadius >= 0.0f);

	// WHY NEGATIVE??
	// Think Sphere is on(!!) frustum plane. But it still should be drawd
	// Distance from plane to EntityPoint is negative.
	// If Distance from plane to EntityPoint is larget than negative radius, it should be drawed
	mTargetEntityBlock->mPositions[mEntityIndexInBlock].SetBoundingSphereRadius(sphereRadius);
}

