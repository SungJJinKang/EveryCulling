#include "RasterizeTrianglesStage.h"

culling::RasterizeTrianglesStage::RasterizeTrianglesStage(MaskedSWOcclusionCulling* mOcclusionCulling)
	: MaskedSWOcclusionCullingStage{ mOcclusionCulling }
{
}

void culling::RasterizeTrianglesStage::CullBlockEntityJob(const size_t cameraIndex)
{
}

