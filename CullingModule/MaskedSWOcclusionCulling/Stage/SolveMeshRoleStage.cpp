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

	// TODO : Consider camera rotation.

	const culling::Vec3 vecFromCameraToBoundingSphere = mCullingSystem->GetCameraWorldPosition(cameraIndex) - spherePointInWorldSpace;
	const float distanceFromCameraToBoundingSphere = culling::Dot(vecFromCameraToBoundingSphere, vecFromCameraToBoundingSphere);

	const float disFromCameraToSphereHorizon = std::sqrt(std::abs(distanceFromCameraToBoundingSphere - sphereRadiusInWorldSpace * sphereRadiusInWorldSpace));
	const float halfOfFieldOfView = (mCullingSystem->GetCameraFieldOfView(cameraIndex) * culling::DEGREE_TO_RADIAN ) / 2.0f;
	
	const float radiusOfViewSpaceSphere = (std::cos(halfOfFieldOfView) / std::sin(halfOfFieldOfView)) * sphereRadiusInWorldSpace / disFromCameraToSphereHorizon;
	
	return (radiusOfViewSpaceSphere > mOccluderViewSpaceBoundingSphereRadius);
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

culling::SolveMeshRoleStage::SolveMeshRoleStage(MaskedSWOcclusionCulling* occlusionCulling)
	: MaskedSWOcclusionCullingStage(occlusionCulling)
{
}

void culling::SolveMeshRoleStage::SolveMeshRole
(
	const size_t cameraIndex,
	EntityBlock* const currentEntityBlock
)
{
	for(size_t entityIndex = 0 ; entityIndex < currentEntityBlock->mCurrentEntityCount ; entityIndex++)
	{
		if(currentEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false)
		{
			const culling::Position_BoundingSphereRadius& posAndBoundingSphere = currentEntityBlock->mPositionAndBoundingSpheres[entityIndex];

			const bool isOccluder = CheckIsOccluderFromBoundingSphere
			(
				cameraIndex,
				posAndBoundingSphere.Position,
				posAndBoundingSphere.BoundingSphereRadius
			);

			currentEntityBlock->SetIsOccluder(entityIndex, cameraIndex, isOccluder);
		}
	}	
}

void culling::SolveMeshRoleStage::CullBlockEntityJob(const size_t cameraIndex)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);;

		if (nextEntityBlock != nullptr)
		{
			SolveMeshRole(cameraIndex, nextEntityBlock);
		}
		else
		{
			break;
		}
	}
}
