#include "FrotbiteCullingSystem.h"

#include <assert.h>
#include <utility>

#include "DataStructure/EntityBlock.h"
#include <vector_erase_move_lastelement/vector_swap_erase.h>

void culling::FrotbiteCullingSystem::FreeEntityBlock(EntityBlock* freedEntityBlock)
{
	assert(freedEntityBlock != nullptr);
	int freedEntityBlockIndex = -1;
	for (unsigned int i = 0; i < this->mEntityGridCell.mBlockCount; i++)
	{
		//Freeing entity block happen barely
		//So this looping is acceptable
		if (this->mEntityGridCell.mEntityBlocks[i] == freedEntityBlock)
		{
			freedEntityBlockIndex = i;
			break;
		}
	}
	assert(freedEntityBlockIndex != -1);

	//swap and pop back trick
	std::swap(this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1], this->mEntityGridCell.mEntityBlocks[freedEntityBlockIndex]);
	std::swap(this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1], this->mEntityGridCell.AllocatedEntityCountInBlocks[freedEntityBlockIndex]);
	freedEntityBlock->mCurrentEntityCount = 0;
	this->mEntityGridCell.mBlockCount--;

	this->mFreeEntityBlockList.push_back(freedEntityBlock);
	std::vector_find_swap_popback(this->mActiveEntityBlockList, freedEntityBlock);
}

culling::EntityBlock* culling::FrotbiteCullingSystem::GetNewEntityBlockFromPool()
{
	assert(this->mFreeEntityBlockList.size() != 0);

	EntityBlock* entityBlock = this->mFreeEntityBlockList.back();
	this->mFreeEntityBlockList.pop_back();
	return entityBlock;
}

void culling::FrotbiteCullingSystem::CacheCullBlockEntityJobs()
{
	for (unsigned int cameraIndex = 0; cameraIndex < MAX_CAMERA_COUNT; cameraIndex++)
	{
		for (unsigned int entityIndex = 0; entityIndex < MAX_ENTITY_BLOCK_COUNT; entityIndex++)
		{
			this->mCachedCullBlockEntityJobs[cameraIndex].push_back(std::bind(&FrotbiteCullingSystem::CullBlockEntityJob, this, entityIndex, cameraIndex));
		}
	}
	
}

culling::FrotbiteCullingSystem::FrotbiteCullingSystem()
	:mViewFrustumCulling{ this } 
#ifndef DISABLE_SCREEN_SAPCE_AABB_CULLING
	,mScreenSpaceAABBCulling{ this }
#endif
{
	AllocateEntityBlockPool();

	this->CacheCullBlockEntityJobs();
	this->bmIsInitializedEntityBlockPool = true;
}

void culling::FrotbiteCullingSystem::AllocateEntityBlockPool()
{
	/// <summary>
	/// Entity Block size should be less than 4KB(Page Size On Window Platform)
	/// </summary>
	assert(sizeof(EntityBlock) < 4000);
	this->mEntityBlockPool = std::make_unique<EntityBlock[]>(MAX_ENTITY_BLOCK_COUNT);
	for (unsigned int i = 0; i < MAX_ENTITY_BLOCK_COUNT; i++)
	{
		this->mFreeEntityBlockList.push_back(this->mEntityBlockPool.get() + i);
	}
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

culling::EntityBlock* culling::FrotbiteCullingSystem::AllocateNewEntityBlockFromPool()
{
	EntityBlock* newEntityBlock = this->GetNewEntityBlockFromPool();
	this->mEntityGridCell.mBlockCount++;
	this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1] = newEntityBlock;
	this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1] = 0;
	newEntityBlock->mCurrentEntityCount = 0;

	this->mActiveEntityBlockList.push_back(newEntityBlock);

	return newEntityBlock;
}

void culling::FrotbiteCullingSystem::RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer)
{
	//Do nothing......

	entityBlockViewer.bmIsActive = false;
	this->RemoveEntityFromBlock(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock);
	//Never Decrease this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}


culling::EntityBlockViewer culling::FrotbiteCullingSystem::AllocateNewEntity(void* renderer, const math::Vector3& position, float radius)
{
	if (this->mEntityGridCell.mBlockCount == 0)
	{
		this->AllocateNewEntityBlockFromPool();
	}

	EntityBlock* lastEntityBlock = this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1];
	unsigned int currentEntityCountInEntityBlock = this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1];
	
	assert(currentEntityCountInEntityBlock <= ENTITY_COUNT_IN_ENTITY_BLOCK); // something is weird........
	
	EntityBlock* entityBlockOfNewEntity{ nullptr };
	unsigned int entityIndexInBlock;

	if (currentEntityCountInEntityBlock < ENTITY_COUNT_IN_ENTITY_BLOCK)
	{
		//lastEntityBlock have empty Entity space
		lastEntityBlock->mCurrentEntityCount++;
		this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1]++;

		entityBlockOfNewEntity = lastEntityBlock;
		entityIndexInBlock = currentEntityCountInEntityBlock;
	}
	else if (currentEntityCountInEntityBlock == ENTITY_COUNT_IN_ENTITY_BLOCK)
	{
		//lastEntityBlock is full, Get new entity block from pool
		EntityBlock* newEntityBlock = this->AllocateNewEntityBlockFromPool();
		
		assert(newEntityBlock == this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1]);

		this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1] = 1;
		this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1]->mCurrentEntityCount = 1;

		entityBlockOfNewEntity = newEntityBlock;
		entityIndexInBlock = 0;
	}
	else
	{
		assert(0); // something is weird........
	}

	entityBlockOfNewEntity->mRenderer[entityIndexInBlock] = renderer;
	std::memcpy(entityBlockOfNewEntity->mPositions[entityIndexInBlock].data(), position.data(), sizeof(math::Vector3));
	entityBlockOfNewEntity->mPositions[entityIndexInBlock].w = radius;
	return EntityBlockViewer(entityBlockOfNewEntity, entityIndexInBlock);
}



// ////////////////////////////////////////




void culling::FrotbiteCullingSystem::SetCameraCount(unsigned int cameraCount)
{
	this->mCameraCount = cameraCount;
	this->mViewFrustumCulling.mCameraCount = cameraCount;
#ifndef DISABLE_SCREEN_SAPCE_AABB_CULLING
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

#ifndef DISABLE_SCREEN_SAPCE_AABB_CULLING
	this->mScreenSpaceAABBCulling.CullBlockEntityJob(currentEntityBlock, entityCountInBlock, blockIndex, cameraIndex);
#endif

	unsigned int finshiedBlockCount = ++(this->mFinishedCullJobBlockCount);
	assert(finshiedBlockCount <= this->mEntityGridCell.mBlockCount);
	/*
	if (finshiedBlockCount == this->mEntityGridCell.mBlockCount)
	{
		std::scoped_lock<std::mutex> sk(this->mCullJobMutex);
		this->mCullJobConditionVaraible.notify_one();
	}
	*/
}

bool culling::FrotbiteCullingSystem::GetIsCullJobFinished()
{
	assert(this->mFinishedCullJobBlockCount <= this->mEntityGridCell.mBlockCount);
	return this->mFinishedCullJobBlockCount == this->mEntityGridCell.mBlockCount;
}

void culling::FrotbiteCullingSystem::WaitToFinishCullJobs()
{
	while (this->GetIsCullJobFinished() == false) // busy wait!
	{

	}
}

void culling::FrotbiteCullingSystem::SetAllOneIsVisibleFlag()
{
	for (unsigned int i = 0; i < this->mEntityGridCell.mBlockCount; i++)
	{
		std::memset(this->mEntityGridCell.mEntityBlocks[i]->mIsVisibleBitflag, 0xFF, sizeof(bool) * ENTITY_COUNT_IN_ENTITY_BLOCK);
	}
}

void culling::FrotbiteCullingSystem::ResetCullJobState()
{
	//this->mAtomicCurrentBlockIndex = 0;
	this->mFinishedCullJobBlockCount = 0;
	this->SetAllOneIsVisibleFlag();
}

