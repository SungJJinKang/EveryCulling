#include "CullingModule.h"

#include "Graphics/Acceleration/LinearData_ViewFrustumCulling/EveryCulling.h"

culling::EntityBlock* culling::CullingModule::GetNextEntityBlock(const size_t cameraIndex, const bool forceOrdering)
{
	// TODO : Implement Cache Friendly GetNextEntityBlock to prevent cache coherency.
	//        After thread finished its blocks, steal other threads's block. 
	// Ex)
	// First Thread : 1 4 7 10  ....
	// Second Thread : 2 5 8 11 ....
	// Third Thread : 3 6 9 12 ....
	//

	const std::uint32_t currentEntityBlockIndex = mCullJobState.mCurrentCulledEntityBlockIndex[cameraIndex].fetch_add(1, forceOrdering == true ? std::memory_order_seq_cst : std::memory_order_relaxed);

	const size_t entityBlockCount = mCullingSystem->GetActiveEntityBlockCount();
	EntityBlock* const currentEntityBlock = (currentEntityBlockIndex >= entityBlockCount) ? (nullptr) : (mCullingSystem->GetActiveEntityBlockList()[currentEntityBlockIndex]);

	if(currentEntityBlock != nullptr)
	{
		assert(currentEntityBlock->mCurrentEntityCount != 0);
	}

	return currentEntityBlock;
}

culling::CullingModule::CullingModule
(
	EveryCulling* cullingSystem
)
:mCullingSystem{ cullingSystem }, IsEnabled(true)
{

}

culling::CullingModule::~CullingModule() = default;

void culling::CullingModule::ResetCullingModule()
{
	for (std::atomic<std::uint32_t>& atomicVal : mCullJobState.mCurrentCulledEntityBlockIndex)
	{
		atomicVal.store(0, std::memory_order_relaxed);
	}

	for (std::atomic<std::uint32_t>& atomicVal : mCullJobState.mFinishedThreadCount)
	{
		atomicVal.store(0, std::memory_order_relaxed);
	}
}

void culling::CullingModule::ThreadCullJob(const size_t cameraIndex)
{
	std::atomic_thread_fence(std::memory_order_acquire);
	CullBlockEntityJob(cameraIndex);


	mCullJobState.mFinishedThreadCount[cameraIndex].fetch_add(1, std::memory_order_seq_cst);
}
