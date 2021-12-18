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
			const culling::Vec3& spherePointInWorldSpace,
			const float sphereRadiusInWorldSpace
		) const;


		bool CheckIsOccluderFromAABB
		(
			const culling::Vec3& const minPointInWorldSpace,
			const culling::Vec3& const maxPointInWorldSpace
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


