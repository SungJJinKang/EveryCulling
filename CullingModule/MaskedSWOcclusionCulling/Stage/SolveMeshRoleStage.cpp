#include "SolveMeshRoleStage.h"

#include <cmath>

#include "../MaskedSWOcclusionCulling.h"

bool culling::SolveMeshRoleStage::CheckIsOccluderFromBoundingSphere
(
	const size_t cameraIndex,
	const culling::Vec3& spherePointInWorldSpace,
	const float sphereRadiusInWorldSpace
) const
{
	//check https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space
	//		https://stackoverflow.com/questions/3717226/radius-of-projected-sphere
	//		https://en.wikipedia.org/wiki/Spherical_cap


	const culling::Vec3 vecFromCameraToBoundingSphere = mMaskedOcclusionCulling.GetCameraWorldPosition(cameraIndex) - spherePointInWorldSpace;
	const float distanceFromCameraToBoundingSphere = culling::Dot(vecFromCameraToBoundingSphere, vecFromCameraToBoundingSphere);

	const float disFromCameraToSphereHorizon = std::sqrt(distanceFromCameraToBoundingSphere * distanceFromCameraToBoundingSphere - sphereRadiusInWorldSpace * sphereRadiusInWorldSpace);
	const float halfOfFieldOfView = (mMaskedOcclusionCulling.GetFieldOfViewInDegree(cameraIndex) / 2) * culling::DEGREE_TO_RADIAN;
	
	const float radiusOfViewSpaceSphere = (std::cos(halfOfFieldOfView) / std::sin(halfOfFieldOfView)) * sphereRadiusInWorldSpace / disFromCameraToSphereHorizon;
	
	return (radiusOfViewSpaceSphere > OCCLUDER_VIEW_BOUNDING_SPHERE_AREA);
}

bool culling::SolveMeshRoleStage::CheckIsOccluderFromAABB
(
	const size_t cameraIndex,
	const culling::Vec3& minPointInWorldSpace,
	const culling::Vec3& maxPointInWorldSpace
) const
{
	return false;
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
		cameraIndex,
		posAndBoundingSphere.Position,
		posAndBoundingSphere.BoundingSphereRadius
	);

	currentEntityBlock->mIsOccluder[entityIndex] = isOccluder;
}
