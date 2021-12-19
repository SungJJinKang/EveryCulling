#include "DepthTestOccludeeStage.h"

culling::DepthTestOccludeeStage::DepthTestOccludeeStage
(
	MaskedSWOcclusionCulling* mOcclusionCulling
)
	: MaskedSWOcclusionCullingStage{ mOcclusionCulling }
{
}

void culling::DepthTestOccludeeStage::CullBlockEntityJob(const size_t cameraIndex)
{
}

