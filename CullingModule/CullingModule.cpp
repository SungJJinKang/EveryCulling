#include "CullingModule.h"

#include "Graphics/Acceleration/LinearData_ViewFrustumCulling/EveryCulling.h"

culling::EntityBlock* culling::CullingModule::GetNextEntityBlock(const size_t cameraIndex)
{
	const std::uint32_t currentEntityBlockIndex = mCullJobState.mCurrentCulledEntityBlockIndex[cameraIndex].fetch_add(1, std::memory_order_seq_cst);

	const size_t entityBlockCount = mCullingSystem->GetActiveEntityBlockCount();
	EntityBlock* const currentEntityBlock = (currentEntityBlockIndex >= entityBlockCount) ? (nullptr) : (mCullingSystem->GetActiveEntityBlockList()[currentEntityBlockIndex]);

	if(currentEntityBlock != nullptr)
	{
		assert(currentEntityBlock->mCurrentEntityCount != 0);
	}

	return currentEntityBlock;
}

void culling::CullingModule::ResetCullingModule()
{
	for (std::atomic<size_t>& atomicVal : mCullJobState.mCurrentCulledEntityBlockIndex)
	{
		atomicVal.store(0, std::memory_order_relaxed);
	}

	for (std::atomic<size_t>& atomicVal : mCullJobState.mFinishedThreadCount)
	{
		atomicVal.store(0, std::memory_order_relaxed);
	}
}

std::uint32_t culling::CullingModule::GetFinishedThreadCount(const size_t cameraIndex) const
{
	return mCullJobState.mFinishedThreadCount[cameraIndex];
}


void culling::CullingModule::ThreadCullJob(const size_t cameraIndex)
{
	CullBlockEntityJob(cameraIndex);
	++(mCullJobState.mFinishedThreadCount[cameraIndex]);
}
