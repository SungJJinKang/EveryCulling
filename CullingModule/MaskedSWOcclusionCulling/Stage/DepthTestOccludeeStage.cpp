#include "DepthTestOccludeeStage.h"

culling::DepthTestOccludeeStage::DepthTestOccludeeStage
(
	MaskedSWOcclusionCulling& mOcclusionCulling
)
	: MaskedSWOcclusionCullingStage{ mOcclusionCulling }
{
}

void culling::DepthTestOccludeeStage::DoStageJob
(
	EntityBlock* const currentEntityBlock, 
	const size_t entityIndex,
	const size_t cameraIndex
)
{
}
