#include "MaskedSWOcclusionCulling.h"


culling::MaskedSWOcclusionCulling::MaskedSWOcclusionCulling
(
	unsigned int width, unsigned int height, 
	float nearClipPlaneDis, float farClipPlaneDis, 
	float* viewProjectionMatrix
)
	: mDepthBuffer{ width, height }, binCountInRow{ width / SUB_TILE_WIDTH }, binCountInColumn{ height / SUB_TILE_HEIGHT },
	mNearClipPlaneDis{ nearClipPlaneDis }, mFarClipPlaneDis{ farClipPlaneDis }, mViewProjectionMatrix{ viewProjectionMatrix }
{
	assert(width % TILE_WIDTH == 0);
	assert(height % TILE_HEIGHT == 0);
}

void culling::MaskedSWOcclusionCulling::SetNearFarClipPlaneDistance(float nearClipPlaneDis, float farClipPlaneDis)
{
	this->mNearClipPlaneDis = nearClipPlaneDis;
	this->mFarClipPlaneDis = farClipPlaneDis;
}

void culling::MaskedSWOcclusionCulling::SetViewProjectionMatrix(float* viewProjectionMatrix)
{
	this->mViewProjectionMatrix = viewProjectionMatrix;
}




	


