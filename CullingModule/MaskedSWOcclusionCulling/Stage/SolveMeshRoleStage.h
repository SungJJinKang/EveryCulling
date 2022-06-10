#pragma once
#include "MaskedSWOcclusionCullingStage.h"

#include "../../../DataType/Math/Vector.h"

namespace culling
{
	class SolveMeshRoleStage : public MaskedSWOcclusionCullingStage
	{
	
	private:
		

		/*
		bool CheckIsOccluderFromBoundingSphere
		(
			const size_t cameraIndex,
			const culling::Vec3& spherePointInWorldSpace,
			const float sphereRadiusInWorldSpace
		) const;
		*/


		EVERYCULLING_FORCE_INLINE bool CheckIsOccluderFromAABB
		(
			EntityBlock* const currentEntityBlock,
			const size_t entityIndex
		) const;

		void SolveMeshRole
		(
			const size_t cameraIndex,
			EntityBlock* const currentEntityBlock,
			bool& isOccluderExist
		);

	public:
		
		float mOccluderAABBScreenSpaceMinArea = 500.0f;

		SolveMeshRoleStage(MaskedSWOcclusionCulling* occlusionCulling);
		
		void CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount) override;
		const char* GetCullingModuleName() const override;
		void ResetCullingModule(const unsigned long long currentTickCount) override;
	};
}


