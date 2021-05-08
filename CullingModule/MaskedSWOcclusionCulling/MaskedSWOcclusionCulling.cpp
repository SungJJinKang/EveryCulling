#include "MaskedSWOcclusionCulling.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeTrianglesStage.h"

void culling::MaskedSWOcclusionCulling::ResetDepthBuffer()
{
	const size_t tileCount = static_cast<size_t>(this->mDepthBuffer.mResolution.mTileCountInARow) * static_cast<size_t>(this->mDepthBuffer.mResolution.mTileCountInAColumn);
	std::shared_ptr<Tile[]> tiles = this->mDepthBuffer.mTiles;
	
	for (size_t i = 0; i < tileCount; i++)
	{
		tiles[i].mBinnedTriangles.mCurrentTriangleCount = 0;
		tiles[i].mHizDatas.depthPosition = _mm256_set1_epi32(0);
		tiles[i].mHizDatas.z0Max = _mm256_set1_ps(1.0f);
		tiles[i].mHizDatas.z1Max = _mm256_set1_ps(1.0f);
	}
	
}

culling::MaskedSWOcclusionCulling::MaskedSWOcclusionCulling(FrotbiteCullingSystem* frotbiteCullingSystem, unsigned int depthBufferWidth, unsigned int depthBufferheight)
	: culling::CullingModule{frotbiteCullingSystem}, 
	mDepthBuffer {
	depthBufferWidth, depthBufferheight
}, binCountInRow{ depthBufferWidth / SUB_TILE_WIDTH }, binCountInColumn{ depthBufferheight / SUB_TILE_HEIGHT },
	mBinTrianglesStage{*this}, mRasterizeTrianglesStage{*this}
{
	assert(depthBufferWidth% TILE_WIDTH == 0);
	assert(depthBufferheight% TILE_HEIGHT == 0);
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

void culling::MaskedSWOcclusionCulling::ResetState()
{
	this->ResetDepthBuffer();

}

void culling::MaskedSWOcclusionCulling::CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex)
{
}




	


