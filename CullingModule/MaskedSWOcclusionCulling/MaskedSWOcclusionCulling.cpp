#include "MaskedSWOcclusionCulling.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeOccludersStage.h"

void culling::MaskedSWOcclusionCulling::ResetDepthBuffer(const unsigned long long currentTickCount)
{
	mDepthBuffer.Reset(currentTickCount);
	
}




culling::MaskedSWOcclusionCulling::MaskedSWOcclusionCulling
(
	EveryCulling* everyCulling, 
	const std::uint32_t depthBufferWidth,
	const std::uint32_t depthBufferheight
)
	:
	CullingModule{ everyCulling},
	mEveryCulling{everyCulling},
	mDepthBuffer { depthBufferWidth, depthBufferheight },
	binCountInRow{ depthBufferWidth / EVERYCULLING_SUB_TILE_WIDTH },
	binCountInColumn{ depthBufferheight / EVERYCULLING_SUB_TILE_HEIGHT },
	mBinTrianglesStage{this},
	mRasterizeTrianglesStage{this},
	mSolveMeshRoleStage{ this },
	mQueryOccludeeStage{this},
	mOccluderListManager{}
{
	assert(depthBufferWidth% EVERYCULLING_TILE_WIDTH == 0);
	assert(depthBufferheight% EVERYCULLING_TILE_HEIGHT == 0);
}

void culling::MaskedSWOcclusionCulling::ResetState(const unsigned long long currentTickCount)
{
	ResetDepthBuffer(currentTickCount);
	if (EVERYCULLING_WHEN_TO_BIN_TRIANGLE(currentTickCount))
	{
		mIsOccluderExist.store(false, std::memory_order_relaxed);
	}

	mOccluderListManager.ResetOccluderList();
}

void culling::MaskedSWOcclusionCulling::CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount)
{
}

const char* culling::MaskedSWOcclusionCulling::GetCullingModuleName() const
{
	return "MaskedSWOcclusionCulling";
}

void culling::MaskedSWOcclusionCulling::SetIsOccluderExistTrue()
{
	mIsOccluderExist = true;
}

bool culling::MaskedSWOcclusionCulling::GetIsOccluderExist() const
{
	return mIsOccluderExist;
}


	


