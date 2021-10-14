#include "MaskedSWOcclusionCulling.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeTrianglesStage.h"

void culling::MaskedSWOcclusionCulling::ResetDepthBuffer()
{
	const SIZE_T tileCount = static_cast<SIZE_T>(mDepthBuffer.mResolution.mTileCountInARow) * static_cast<SIZE_T>(mDepthBuffer.mResolution.mTileCountInAColumn);
	std::shared_ptr<Tile[]> tiles = mDepthBuffer.mTiles;
	
	for (SIZE_T i = 0; i < tileCount; i++)
	{
		tiles[i].mBinnedTriangles.mCurrentTriangleCount = 0;
		tiles[i].mHizDatas.depthPosition = _mm256_set1_epi32(0);
		tiles[i].mHizDatas.z0Max = _mm256_set1_ps(1.0f);
		tiles[i].mHizDatas.z1Max = _mm256_set1_ps(1.0f);
	}
	
}

culling::MaskedSWOcclusionCulling::MaskedSWOcclusionCulling(EveryCulling* frotbiteCullingSystem, UINT32 depthBufferWidth, UINT32 depthBufferheight)
	: culling::CullingModule{frotbiteCullingSystem}, 
	mDepthBuffer {
	depthBufferWidth, depthBufferheight
}, binCountInRow{ depthBufferWidth / SUB_TILE_WIDTH }, binCountInColumn{ depthBufferheight / SUB_TILE_HEIGHT },
	mBinTrianglesStage{*this}, mRasterizeTrianglesStage{*this}
{
	assert(depthBufferWidth% TILE_WIDTH == 0);
	assert(depthBufferheight% TILE_HEIGHT == 0);
}

void culling::MaskedSWOcclusionCulling::SetNearFarClipPlaneDistance(FLOAT32 nearClipPlaneDis, FLOAT32 farClipPlaneDis)
{
	mNearClipPlaneDis = nearClipPlaneDis;
	mFarClipPlaneDis = farClipPlaneDis;
}

void culling::MaskedSWOcclusionCulling::SetViewProjectionMatrix(FLOAT32* viewProjectionMatrix)
{
	mViewProjectionMatrix = viewProjectionMatrix;
}

void culling::MaskedSWOcclusionCulling::ResetState()
{
	ResetDepthBuffer();

}

void culling::MaskedSWOcclusionCulling::CullBlockEntityJob(EntityBlock* currentEntityBlock, SIZE_T entityCountInBlock, SIZE_T cameraIndex)
{
}




	


