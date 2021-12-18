#include "SolveMeshRoleStage.h"

#include "../MaskedSWOcclusionCulling.h"

bool culling::SolveMeshRoleStage::CheckIsOccluderFromBoundingSphere
(
	const culling::Vec3& spherePointInWorldSpace,
	const float sphereRadiusInWorldSpace
) const
{
	//check https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space
	//		https://stackoverflow.com/questions/3717226/radius-of-projected-sphere
	//		https://en.wikipedia.org/wiki/Spherical_cap
}

bool culling::SolveMeshRoleStage::CheckIsOccluderFromAABB
(
	const culling::Vec3& minPointInWorldSpace,
	const culling::Vec3& maxPointInWorldSpace
) const
{
}

culling::SolveMeshRoleStage::SolveMeshRoleStage(MaskedSWOcclusionCulling& occlusionCulling)
	: MaskedSWOcclusionCullingStage(occlusionCulling)
{
}

void culling::SolveMeshRoleStage::DoStageJob
(
	EntityBlock* const currentEntityBlock, 
	const size_t entityIndex,
	const size_t cameraIndex
)
{
	const culling::Position_BoundingSphereRadius& posAndBoundingSphere = currentEntityBlock->mPositionAndBoundingSpheres[entityIndex];

	const bool isOccluder = CheckIsOccluderFromBoundingSphere
	(
		posAndBoundingSphere.Position,
		posAndBoundingSphere.BoundingSphereRadius
	);

	currentEntityBlock->mIsOccluder[entityIndex] = isOccluder;
}
