#pragma once
#include "MaskedSWOcclusionCullingStage.h"

#include "../../../DataType/Math/Vector.h"

namespace culling
{
	class SolveMeshRoleStage : public MaskedSWOcclusionCullingStage
	{
	
	private:
		
		float mOccluderAABBScreenSpaceMinArea = 500.0f;
		float mOccluderLimitOfDistanceToCamera = 500.0f;

		/*
		bool CheckIsOccluderFromBoundingSphere
		(
			const size_t cameraIndex,
			const culling::Vec3& spherePointInWorldSpace,
			const float sphereRadiusInWorldSpace
		) const;
		*/


		bool CheckIsOccluder
		(
			EntityBlock* const currentEntityBlock,
			const size_t entityIndex,
			const culling::Vec3& CameraWorldPos
		) const;

		void SolveMeshRole
		(
			const size_t cameraIndex,
			EntityBlock* const currentEntityBlock,
			bool& isOccluderExist
		);

	public:
		
		SolveMeshRoleStage(MaskedSWOcclusionCulling* occlusionCulling);
		
		void CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount) override;
		const char* GetCullingModuleName() const override;
		void ResetCullingModule(const unsigned long long currentTickCount) override;

		void SetOccluderAABBScreenSpaceMinArea(const float occluderAABBScreenSpaceMinArea);
		void SetOccluderLimitOfDistanceToCamera(const float OccluderLimitOfDistanceToCamera);
	};
}


