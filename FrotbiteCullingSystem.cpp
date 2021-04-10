#include "FrotbiteCullingSystem.h"

#include <assert.h>
#include <utility>

#include "DataStructure/EntityBlock.h"
#include <vector_erase_move_lastelement/vector_swap_erase.h>

void culling::FrotbiteCullingSystem::FreeEntityBlock(EntityBlock* freedEntityBlock)
{
	assert(freedEntityBlock != nullptr);
	size_t freedEntityBlockIndex;
	size_t entityBlockCount = this->mEntityGridCell.mEntityBlocks.size();
	bool IsSuccessToFind = false;
	for (size_t i = 0; i < entityBlockCount; i++)
	{
		//Freeing entity block happen barely
		//So this looping is acceptable
		if (this->mEntityGridCell.mEntityBlocks[i] == freedEntityBlock)
		{
			freedEntityBlockIndex = i;
			IsSuccessToFind = true;
			break;
		}
	}
	assert(IsSuccessToFind == true);

	//swap and pop back trick
	std::vector_swap_popback(this->mEntityGridCell.mEntityBlocks, freedEntityBlockIndex);
	std::vector_swap_popback(this->mEntityGridCell.AllocatedEntityCountInBlocks, freedEntityBlockIndex);
	
	freedEntityBlock->mCurrentEntityCount = 0;

	this->mFreeEntityBlockList.push_back(freedEntityBlock);
	std::vector_find_swap_popback(this->mActiveEntityBlockList, freedEntityBlock);
}

culling::EntityBlock* culling::FrotbiteCullingSystem::GetNewEntityBlockFromPool()
{
	if (this->mFreeEntityBlockList.size() == 0)
	{
		this->AllocateEntityBlockPool();
	}

	assert(this->mFreeEntityBlockList.size() != 0);
	EntityBlock* entityBlock = this->mFreeEntityBlockList.back();
	this->mFreeEntityBlockList.pop_back();
	return entityBlock;
}

void culling::FrotbiteCullingSystem::CacheCullBlockEntityJobs()
{
	for (unsigned int cameraIndex = 0; cameraIndex < MAX_CAMERA_COUNT; cameraIndex++)
	{
		this->mCachedCullBlockEntityJobs[cameraIndex].reserve(MAX_ENTITY_BLOCK_COUNT);
		for (unsigned int entityBlockIndex = 0; entityBlockIndex < MAX_ENTITY_BLOCK_COUNT; entityBlockIndex++)
		{
			this->mCachedCullBlockEntityJobs[cameraIndex].push_back(std::bind(&FrotbiteCullingSystem::CullBlockEntityJob, this, entityBlockIndex, cameraIndex));
		}
	}
	
}



void culling::FrotbiteCullingSystem::ReleaseThreadLocalFinishedCullJobBlockCount()
{
	this->mFinishedCullJobBlockCount.fetch_add(this->mThreadLocalFinishedCullJobBlockCount, std::memory_order_release);
	this->mThreadLocalFinishedCullJobBlockCount = 0;
}

culling::FrotbiteCullingSystem::FrotbiteCullingSystem()
	:
	mCommitThreadLocalFinishedCullJobBlockCountStdFunction{ std::bind(&FrotbiteCullingSystem::ReleaseThreadLocalFinishedCullJobBlockCount, this) },
	mViewFrustumCulling{ this } 
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	,mScreenSpaceAABBCulling{ this }
#endif
{
	this->mFreeEntityBlockList.reserve(100);
	this->mEntityGridCell.mEntityBlocks.reserve(100);
	this->mEntityGridCell.AllocatedEntityCountInBlocks.reserve(100);

	AllocateEntityBlockPool();

	this->CacheCullBlockEntityJobs();
	this->bmIsInitializedEntityBlockPool = true;
}

culling::FrotbiteCullingSystem::~FrotbiteCullingSystem()
{
	for (auto allocatedEntityBlockChunk : this->mAllocatedEntityBlockChunkList)
	{
		delete[] allocatedEntityBlockChunk;
	}
}

void culling::FrotbiteCullingSystem::AllocateEntityBlockPool()
{
	EntityBlock* newEntityBlockChunk = new EntityBlock[INITIAL_ENTITY_BLOCK_COUNT];
	for (unsigned int i = 0; i < INITIAL_ENTITY_BLOCK_COUNT; i++)
	{
		this->mFreeEntityBlockList.push_back(newEntityBlockChunk + i);
	}
	this->mAllocatedEntityBlockChunkList.push_back(newEntityBlockChunk);
}

void culling::FrotbiteCullingSystem::RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock)
{
	assert(ownerEntityBlock != nullptr);
	assert(entityIndexInBlock >= 0 && entityIndexInBlock < ENTITY_COUNT_IN_ENTITY_BLOCK);

	//Do nothing......

	//Never Decrease this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it

	ownerEntityBlock->mCurrentEntityCount--;
	if (ownerEntityBlock->mCurrentEntityCount == 0)
	{
		this->FreeEntityBlock(ownerEntityBlock);
	}
	
}

std::pair<culling::EntityBlock*, unsigned int*> culling::FrotbiteCullingSystem::AllocateNewEntityBlockFromPool()
{
	EntityBlock* newEntityBlock = this->GetNewEntityBlockFromPool();
	this->mEntityGridCell.mEntityBlocks.push_back(newEntityBlock); 
	this->mEntityGridCell.AllocatedEntityCountInBlocks.push_back(0);
	newEntityBlock->mCurrentEntityCount = 0;

	this->mActiveEntityBlockList.push_back(newEntityBlock);

	return { this->mEntityGridCell.mEntityBlocks.back(), &(this->mEntityGridCell.AllocatedEntityCountInBlocks.back()) };
}

void culling::FrotbiteCullingSystem::RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer)
{
	//Do nothing......

	entityBlockViewer.bmIsActive = false;
	this->RemoveEntityFromBlock(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock);
	//Never Decrease this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}


culling::EntityBlockViewer culling::FrotbiteCullingSystem::AllocateNewEntity(void* renderer)
{
	std::pair<culling::EntityBlock*, unsigned int*> targetEntityBlock;
	if (this->mEntityGridCell.mEntityBlocks.size() == 0)
	{
		targetEntityBlock = this->AllocateNewEntityBlockFromPool();
	}

	targetEntityBlock = { this->mEntityGridCell.mEntityBlocks.back(), &(this->mEntityGridCell.AllocatedEntityCountInBlocks.back()) };

	if (*(targetEntityBlock.second) == ENTITY_COUNT_IN_ENTITY_BLOCK)
	{
		targetEntityBlock = this->AllocateNewEntityBlockFromPool();
	}
	
	assert(*(targetEntityBlock.second) <= ENTITY_COUNT_IN_ENTITY_BLOCK); // something is weird........
	
	targetEntityBlock.first->mCurrentEntityCount++;
	(*(targetEntityBlock.second))++;

	targetEntityBlock.first->mRenderer[targetEntityBlock.first->mCurrentEntityCount - 1] = renderer;
	return EntityBlockViewer(targetEntityBlock.first, targetEntityBlock.first->mCurrentEntityCount - 1);
}



// ////////////////////////////////////////




void culling::FrotbiteCullingSystem::SetCameraCount(unsigned int cameraCount)
{
	this->mCameraCount = cameraCount;
	this->mViewFrustumCulling.mCameraCount = cameraCount;
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	this->mScreenSpaceAABBCulling.mCameraCount = cameraCount;
#endif
}

unsigned int culling::FrotbiteCullingSystem::GetCameraCount()
{
	return this->mCameraCount;
}



void culling::FrotbiteCullingSystem::CullBlockEntityJob(unsigned int blockIndex, unsigned int cameraIndex)
{
	EntityBlock* currentEntityBlock = this->mEntityGridCell.mEntityBlocks[blockIndex];
	unsigned int entityCountInBlock = this->mEntityGridCell.AllocatedEntityCountInBlocks[blockIndex]; // don't use mCurrentEntityCount

	this->mViewFrustumCulling.CullBlockEntityJob(currentEntityBlock, entityCountInBlock, blockIndex, cameraIndex);

#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	this->mScreenSpaceAABBCulling.CullBlockEntityJob(currentEntityBlock, entityCountInBlock, blockIndex, cameraIndex);
#endif

	this->mThreadLocalFinishedCullJobBlockCount++;
}

bool culling::FrotbiteCullingSystem::GetIsCullJobFinished()
{
	//assert(this->mFinishedCullJobBlockCount <= this->mEntityGridCell.mBlockCount);
	return this->mFinishedCullJobBlockCount.load(std::memory_order_relaxed) == this->mEntityGridCell.mEntityBlocks.size();
}

void culling::FrotbiteCullingSystem::WaitToFinishCullJobs()
{
// 	{
// 		std::unique_lock<std::mutex> lk(this->mCullJobMutex);
// 		this->mCullJobConditionVaraible.wait(lk, [this] {return this->GetIsCullJobFinished(); });
// 	}
	
 	while (this->GetIsCullJobFinished() == false) // busy wait!
 	{
 
 	}
}

void culling::FrotbiteCullingSystem::SetAllOneIsVisibleFlag()
{
	for (auto entityBlock : this->mEntityGridCell.mEntityBlocks)
	{
		std::memset(entityBlock->mIsVisibleBitflag, 0xFF, sizeof(bool) * ENTITY_COUNT_IN_ENTITY_BLOCK);
	}
}

void culling::FrotbiteCullingSystem::ResetCullJobState()
{
	//this->mAtomicCurrentBlockIndex = 0;
	this->mFinishedCullJobBlockCount = 0;
	this->SetAllOneIsVisibleFlag();
}

