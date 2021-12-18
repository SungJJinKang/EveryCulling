#pragma once

#include "MaskedSWOcclusionCullingStage.h"

namespace culling
{
	class DepthTestOccludeeStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

	public:

		DepthTestOccludeeStage(MaskedSWOcclusionCulling& mOcclusionCulling);
		void DoStageJob
		(
			EntityBlock* const currentEntityBlock, 
			const size_t entityIndex,
			const size_t cameraIndex
		) override;
	};
}


