#include "RasterizeTrianglesStage.h"

culling::RasterizeTrianglesStage::RasterizeTrianglesStage(MaskedSWOcclusionCulling& mOcclusionCulling)
	: MaskedSWOcclusionCullingStage{ mOcclusionCulling }
{
}

void culling::RasterizeTrianglesStage::DoStageJob
(
	EntityBlock* const currentEntityBlock,
	const size_t entityIndex,
	const size_t cameraIndex
)
{
}
