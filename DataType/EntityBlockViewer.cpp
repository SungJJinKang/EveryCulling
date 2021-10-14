#include "EntityBlockViewer.h"

#include <cassert>
#include <cstring>



void culling::EntityBlockViewer::SetEntityPosition(const FLOAT32* worldPosition)
{
	assert(bmIsActive == true);
	std::memcpy((mTargetEntityBlock->mPositions + mEntityIndexInBlock), worldPosition, sizeof(Vec3));
}

void culling::EntityBlockViewer::SetSphereBoundRadius(FLOAT32 sphereRadius)
{
	assert(bmIsActive == true);
	assert(sphereRadius >= 0.0f);

	// WHY NEGATIVE??
	// Think Sphere is on(!!) frustum plane. But it still should be drawd
	// Distance from plane to EntityPoint is negative.
	// If Distance from plane to EntityPoint is larget than negative radius, it should be drawed
	mTargetEntityBlock->mPositions[mEntityIndexInBlock].SetBoundingSphereRadius(sphereRadius);
}

