#include "CullingModule.h"

#include "../EveryCulling.h"

culling::EntityBlock* culling::CullingModule::GetNextEntityBlock(const size_t cameraIndex, const bool forceOrdering)
{
	// TODO : Implement Cache Friendly GetNextEntityBlock to prevent cache coherency.
	//        After thread finished its blocks, steal other threads's block. 
	// Ex)
	// First Thread : 1 4 7 10  ....
	// Second Thread : 2 5 8 11 ....
	// Third Thread : 3 6 9 12 ....
	//

	const std::uint32_t currentEntityBlockIndex = mCullJobState.mCurrentCulledEntityBlockIndexOfThread[0][cameraIndex].fetch_add(1, forceOrdering == true ? std::memory_order_seq_cst : std::memory_order_relaxed);

	const size_t entityBlockCount = mCullingSystem->GetActiveEntityBlockCount();
	EntityBlock* const currentEntityBlock = (currentEntityBlockIndex >= entityBlockCount) ? (nullptr) : (mCullingSystem->GetActiveEntityBlockList()[currentEntityBlockIndex]);

	if(currentEntityBlock != nullptr)
	{
		assert(currentEntityBlock->mCurrentEntityCount != 0);
	}

	return currentEntityBlock;
}

size_t culling::CullingModule::ComputeEndEntityBlockIndexOfThread(const std::int32_t threadIndex)
{
	return mCullingSystem->GetActiveEntityBlockCount() - (mCullingSystem->GetActiveEntityBlockCount() % threadIndex);
}

culling::EntityBlock* culling::CullingModule::GetNextEntityBlockForMultipleThreads(const size_t cameraIndex, const std::int32_t localThreadIndex)
{
	culling::EntityBlock* entityBlock = nullptr;

	if(localThreadIndex != EVERYCULLING_INVALID_LOCAL_THREAD_INDEX)
	{
		//const std::int32_t localThreadIndex = mCullingSystem->GetLocalThreadIndex();
		const std::uint32_t threadCount = mCullingSystem->GetThreadCount();
		const size_t entityBlockCount = mCullingSystem->GetActiveEntityBlockCount();

		size_t threadEntityBlockIndex = mCullJobState.mCurrentCulledEntityBlockIndexOfThread[localThreadIndex][cameraIndex]++;
		size_t entityBlockIndex = localThreadIndex + threadEntityBlockIndex * threadCount;
		if(entityBlockIndex < entityBlockCount)
		{
			entityBlock = mCullingSystem->GetActiveEntityBlockList()[entityBlockIndex];
		}
		else
		{
			for(std::int32_t threadIndex = 0 ; threadIndex < threadCount ; threadIndex++)
			{
				if(threadIndex != localThreadIndex)
				{
					threadEntityBlockIndex = mCullJobState.mCurrentCulledEntityBlockIndexOfThread[threadIndex][cameraIndex]++;
					entityBlockIndex = threadIndex + threadEntityBlockIndex * threadCount;
					if (entityBlockIndex < entityBlockCount)
					{
						entityBlock = mCullingSystem->GetActiveEntityBlockList()[entityBlockIndex];
						break;
					}
				}
			}
		}
	}
	else
	{
		entityBlock = GetNextEntityBlock(cameraIndex, true);
	}

	return entityBlock;
}


culling::CullingModule::CullingModule
(
	EveryCulling* cullingSystem
)
:mCullingSystem{ cullingSystem }, IsEnabled(true)
{

}

culling::CullingModule::~CullingModule() = default;

void culling::CullingModule::ResetCullingModule(const unsigned long long currentTickCount)
{
	for(auto& threadCurrentCulledEntityBlockIndex : mCullJobState.mCurrentCulledEntityBlockIndexOfThread)
	{
		for (std::atomic<std::uint32_t>& atomicVal : threadCurrentCulledEntityBlockIndex)
		{
			atomicVal.store(0, std::memory_order_relaxed);
		}
	}
	

	for (std::atomic<std::uint32_t>& atomicVal : mCullJobState.mFinishedThreadCount)
	{
		atomicVal.store(0, std::memory_order_relaxed);
	}
}

void culling::CullingModule::OnSetThreadCount(const size_t threadCount)
{
	/*
	 *for(auto& cameraEntityBlockIndex : mCullJobState.mCurrentCulledEntityBlockIndexOfThread)
	{
		cameraEntityBlockIndex.resize(threadCount);
	}
	*/
}

void culling::CullingModule::ThreadCullJob(const size_t cameraIndex, const std::int32_t localThreadIndex, const unsigned long long currentTickCount)
{
	std::atomic_thread_fence(std::memory_order_acquire);
	CullBlockEntityJob(cameraIndex, localThreadIndex, currentTickCount);


	mCullJobState.mFinishedThreadCount[cameraIndex].fetch_add(1, std::memory_order_seq_cst);
}
