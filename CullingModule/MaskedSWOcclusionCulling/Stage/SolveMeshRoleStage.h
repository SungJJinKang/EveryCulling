#pragma once
#include "MaskedSWOcclusionCullingStage.h"

#include "../../../DataType/Math/Vector.h"

namespace culling
{
	class SolveMeshRoleStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

		bool CheckIsOccluderFromBoundingSphere
		(
			const size_t cameraIndex,
			const culling::Vec3& spherePointInWorldSpace,
			const float sphereRadiusInWorldSpace
		) const;


		bool CheckIsOccluderFromAABB
		(
			const size_t cameraIndex,
			const culling::Vec3& minPointInWorldSpace,
			const culling::Vec3& maxPointInWorldSpace
		) const;

	public:

		SolveMeshRoleStage(MaskedSWOcclusionCulling& occlusionCulling);

		void DoStageJob
		(
			EntityBlock* const currentEntityBlock, 
			const size_t entityIndex,
			const size_t cameraIndex
		) override;

	};
}


