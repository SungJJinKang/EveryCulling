#pragma once
#include "MaskedSWOcclusionCullingStage.h"

#include "../../../DataType/Math/Vector.h"

namespace culling
{
	class SolveMeshRoleStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

		std::atomic<size_t> mOccluderCount;
		bool IsOccluderCapacityFull() const;
		size_t IncreamentOccluderCount();

		bool CheckIsOccluderFromBoundingSphere
		(
			const size_t cameraIndex,
			const culling::Vec3& spherePointInWorldSpace,
			const float sphereRadiusInWorldSpace
		) const;


		FORCE_INLINE bool CheckIsOccluderFromAABB
		(
			EntityBlock* const currentEntityBlock,
			const size_t entityIndex
		) const;

		void SolveMeshRole
		(
			const size_t cameraIndex,
			EntityBlock* const currentEntityBlock
		);

	public:

		float mOccluderViewSpaceBoundingSphereRadius = 0.9f;
		float mOccluderAABBScreenSpaceMinArea = 500.0f;

		SolveMeshRoleStage(MaskedSWOcclusionCulling* occlusionCulling);
		
		void CullBlockEntityJob(const size_t cameraIndex) override;
		const char* GetCullingModuleName() const override;
		void ResetCullingModule() override;
	};
}


