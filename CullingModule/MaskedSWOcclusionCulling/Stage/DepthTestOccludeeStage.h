#pragma once

#include "MaskedSWOcclusionCullingStage.h"

namespace culling
{
	class DepthTestOccludeeStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

	public:

		DepthTestOccludeeStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void CullBlockEntityJob(const size_t cameraIndex) override;
	};
}


