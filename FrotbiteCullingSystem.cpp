#include "FrotbiteCullingSystem.h"

#include <utility>

#include "DataType/EntityBlock.h"
#include <vector_erase_move_lastelement/vector_swap_erase.h>

#include <iostream>




void culling::FrotbiteCullingSystem::FreeEntityBlock(EntityBlock* freedEntityBlock)
{
	assert(freedEntityBlock != nullptr);

	size_t freedEntityBlockIndex;
	const size_t entityBlockCount = this->mEntityGridCell.mEntityBlocks.size();
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

void culling::FrotbiteCullingSystem::CacheCullJob(size_t currentEntityBlockCount)
{
	for (unsigned int cameraIndex = 0; cameraIndex < MAX_CAMERA_COUNT; cameraIndex++)
	{
		while (const size_t blockCount = this->mCachedCullBlockEntityJobs[cameraIndex].size() < currentEntityBlockCount)
		{
			this->mCachedCullBlockEntityJobs[cameraIndex].push_back(std::bind(&FrotbiteCullingSystem::CullBlockEntityJob, this, blockCount, cameraIndex));
		}
	}
}

/*
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
*/

void culling::FrotbiteCullingSystem::ReleaseFinishedBlockCount()
{
	this->mCullJobFinishedBlockCount.fetch_add(this->mThreadCullJobFinishedBlockCount, std::memory_order_release);
	this->mThreadCullJobFinishedBlockCount = 0;
}

void culling::FrotbiteCullingSystem::SetAllOneIsVisibleFlag()
{
	//TODO : Use SIMD M256 or Compiler can do that?(Check godbolt)
	for (auto entityBlock : this->mEntityGridCell.mEntityBlocks)
	{
		std::memset(entityBlock->mIsVisibleBitflag, 0xFF, sizeof(char) * ENTITY_COUNT_IN_ENTITY_BLOCK);
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

	//Don't decrement this->mEntityGridCell.AllocatedEntityCountInBlocks
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





culling::EntityBlockViewer culling::FrotbiteCullingSystem::AllocateNewEntity(void* renderer)
{
	std::pair<culling::EntityBlock*, unsigned int*> targetEntityBlock;
	if (this->mEntityGridCell.mEntityBlocks.size() == 0)
	{
		// if Any entityBlock isn't allocated yet
		targetEntityBlock = this->AllocateNewEntityBlockFromPool();
		this->CacheCullJob(this->mEntityGridCell.mEntityBlocks.size());
	}
	else
	{//When Allocated entity block count is at least one

		//Get last entityblock in active entities
		targetEntityBlock = { this->mEntityGridCell.mEntityBlocks.back(), &(this->mEntityGridCell.AllocatedEntityCountInBlocks.back()) };

		if (*(targetEntityBlock.second) == ENTITY_COUNT_IN_ENTITY_BLOCK)
		{
			//if last entityblock in active entities is full of entities
			//alocate new entity block
			targetEntityBlock = this->AllocateNewEntityBlockFromPool();
			this->CacheCullJob(this->mEntityGridCell.mEntityBlocks.size());
		}
	}
	
	
	assert(*(targetEntityBlock.second) <= ENTITY_COUNT_IN_ENTITY_BLOCK); // something is weird........
	
	targetEntityBlock.first->mCurrentEntityCount++;
	(*(targetEntityBlock.second))++;

	targetEntityBlock.first->mRenderer[targetEntityBlock.first->mCurrentEntityCount - 1] = renderer;
	return EntityBlockViewer(targetEntityBlock.first, targetEntityBlock.first->mCurrentEntityCount - 1);
}

void culling::FrotbiteCullingSystem::RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer)
{
	//Do nothing......

	entityBlockViewer.bmIsActive = false;
	this->RemoveEntityFromBlock(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock);
	//Don't decrement this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}

culling::FrotbiteCullingSystem::FrotbiteCullingSystem()
	:
	mCullJobCache{ std::bind(&FrotbiteCullingSystem::ReleaseFinishedBlockCount, this) },
	mViewFrustumCulling{ this }
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	, mScreenSpaceAABBCulling{ this }
#endif
{
	//to protect 
	this->mFreeEntityBlockList.reserve(100);
	this->mEntityGridCell.mEntityBlocks.reserve(100);
	this->mEntityGridCell.AllocatedEntityCountInBlocks.reserve(100);

	this->AllocateEntityBlockPool();

	//this->CacheCullBlockEntityJobs();
	this->bmIsEntityBlockPoolInitialized = true;
}

culling::FrotbiteCullingSystem::~FrotbiteCullingSystem()
{
	for (culling::EntityBlock* allocatedEntityBlockChunk : this->mAllocatedEntityBlockChunkList)
	{
		delete[] allocatedEntityBlockChunk;
	}
}

void culling::FrotbiteCullingSystem::SetCameraCount(unsigned int cameraCount)
{
	this->mCameraCount = cameraCount;
	this->mViewFrustumCulling.mCameraCount = cameraCount;
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	this->mScreenSpaceAABBCulling.mCameraCount = cameraCount;
#endif
}

unsigned int culling::FrotbiteCullingSystem::GetCameraCount() const
{
	return this->mCameraCount;
}

std::vector<culling::EntityBlock*> culling::FrotbiteCullingSystem::GetActiveEntityBlockList() const
{
	return this->mActiveEntityBlockList;
}

std::vector<std::function<void()>> culling::FrotbiteCullingSystem::GetCullBlockEnityJobs(unsigned int cameraIndex)
{
	std::vector<std::function<void()>> cullJobs{};
	cullJobs.assign(this->mCachedCullBlockEntityJobs[cameraIndex].begin(), this->mCachedCullBlockEntityJobs[cameraIndex].begin() + this->mEntityGridCell.mEntityBlocks.size());
	return cullJobs;
}

std::function<void()> culling::FrotbiteCullingSystem::GetCullJobs() const
{
	return this->mCullJobCache;
}


