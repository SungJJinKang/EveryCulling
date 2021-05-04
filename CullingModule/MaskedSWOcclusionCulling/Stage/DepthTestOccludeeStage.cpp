#include "DepthTestOccludeeStage.h"

culling::DepthTestOccludeeStage::DepthTestOccludeeStage(MaskedSWOcclusionCulling& mOcclusionCulling)
	: MaskedSWOcclusionCullingStage{ mOcclusionCulling }
{
}
