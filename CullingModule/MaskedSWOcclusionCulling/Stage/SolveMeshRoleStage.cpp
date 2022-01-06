#include "SolveMeshRoleStage.h"

#include <cmath>

#include "../MaskedSWOcclusionCulling.h"
#include "../Utility/vertexTransformationHelper.h"

bool culling::SolveMeshRoleStage::IsOccluderCapacityFull() const
{
	return mOccluderCount >= MAX_OCCLUDER_COUNT;
}

size_t culling::SolveMeshRoleStage::IncreamentOccluderCount()
{
	const size_t currentOccluderCount = mOccluderCount.fetch_add(1, std::memory_order_seq_cst);
	if(currentOccluderCount < MAX_OCCLUDER_COUNT)
	{
		return currentOccluderCount;
	}
	else
	{
		return 0;
	}
}

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
	const float halfOfFieldOfView = (mCullingSystem->GetCameraFieldOfView(cameraIndex) * culling::DEGREE_TO_RADIAN) * 0.5f;
	
	const float radiusOfViewSpaceSphere = (std::cos(halfOfFieldOfView) / std::sin(halfOfFieldOfView)) * sphereRadiusInWorldSpace / disFromCameraToSphereHorizon;
	
	return (radiusOfViewSpaceSphere > mOccluderViewSpaceBoundingSphereRadius);
}

FORCE_INLINE bool culling::SolveMeshRoleStage::CheckIsOccluderFromAABB
(
	EntityBlock* const currentEntityBlock,
	const size_t entityIndex
) const
{
	const float clampedAABBMinScreenSpacePointX = CLAMP(currentEntityBlock->mAABBMinScreenSpacePointX[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mWidth);
	const float clampedAABBMinScreenSpacePointY = CLAMP(currentEntityBlock->mAABBMinScreenSpacePointY[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mHeight);
	const float clampedAABBMaxScreenSpacePointX = CLAMP(currentEntityBlock->mAABBMaxScreenSpacePointX[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mWidth);
	const float clampedAABBMaxScreenSpacePointY = CLAMP(currentEntityBlock->mAABBMaxScreenSpacePointY[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mHeight);

	const float screenSpaceAABBArea
		= (clampedAABBMaxScreenSpacePointX - clampedAABBMinScreenSpacePointX) *
		  (clampedAABBMaxScreenSpacePointY - clampedAABBMinScreenSpacePointY);

	assert(screenSpaceAABBArea >= 0.0f);
	//assert(screenSpaceAABBArea > std::numeric_limits<float>::epsilon());

	return screenSpaceAABBArea > mOccluderAABBScreenSpaceMinArea;
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
			const bool isOccluder = CheckIsOccluderFromAABB(currentEntityBlock, entityIndex) && currentEntityBlock->GetIsIsAABBScreenSpacePointValid(entityIndex);

			currentEntityBlock->SetIsOccluder(entityIndex, isOccluder);

			if(isOccluder == true)
			{
				const size_t currentOccluderCount = IncreamentOccluderCount();
				if (currentOccluderCount == 0)
				{
					//early finish this stage
					break;
				}
			}
		}
	}	
}

void culling::SolveMeshRoleStage::CullBlockEntityJob(const size_t cameraIndex)
{
	while (true)
	{
		if(IsOccluderCapacityFull() == true)
		{
			//early finish this stage
			break;
		}

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

const char* culling::SolveMeshRoleStage::GetCullingModuleName() const
{
	return "SolveMeshRoleStage";
}

void culling::SolveMeshRoleStage::ResetCullingModule()
{
	MaskedSWOcclusionCullingStage::ResetCullingModule();

	mOccluderCount = 0;
}
