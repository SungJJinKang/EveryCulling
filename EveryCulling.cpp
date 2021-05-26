#include "EveryCulling.h"

#include <utility>

#include "DataType/EntityBlock.h"
#include <vector_erase_move_lastelement/vector_swap_erase.h>

void culling::EveryCulling::FreeEntityBlock(EntityBlock* freedEntityBlock)
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


culling::EntityBlock* culling::EveryCulling::GetNewEntityBlockFromPool()
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

void culling::EveryCulling::ResetCullJobStateVariable()
{
	for (unsigned int cameraIndex = 0; cameraIndex < this->mCameraCount; cameraIndex++)
	{
		for (size_t moduleIndex = 0; moduleIndex < this->mUpdatedCullingModules.size(); moduleIndex++)
		{
			CullingModule* cullingModule = this->mUpdatedCullingModules[moduleIndex];
			cullingModule->mCurrentCullEntityBlockIndex[cameraIndex].store(0, std::memory_order_relaxed);
			cullingModule->mFinishedCullEntityBlockCount[cameraIndex].store(0, std::memory_order_relaxed);
		}
	}
	this->mIsCullJobFinished.store(false, std::memory_order_relaxed);

	//release!
	std::atomic_thread_fence(std::memory_order_seq_cst);
}

void culling::EveryCulling::SetAllOneIsVisibleFlag()
{
	//Maybe Compiler use SIMD or do faster than SIMD instruction
	for (auto entityBlock : this->mEntityGridCell.mEntityBlocks)
	{
		std::memset(entityBlock->mIsVisibleBitflag, 0xFF, sizeof(char) * ENTITY_COUNT_IN_ENTITY_BLOCK);
	}
}

void culling::EveryCulling::AllocateEntityBlockPool()
{
	EntityBlock* newEntityBlockChunk = new EntityBlock[INITIAL_ENTITY_BLOCK_COUNT];
	for (unsigned int i = 0; i < INITIAL_ENTITY_BLOCK_COUNT; i++)
	{
		this->mFreeEntityBlockList.push_back(newEntityBlockChunk + i);
	}
	this->mAllocatedEntityBlockChunkList.push_back(newEntityBlockChunk);
}

void culling::EveryCulling::RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock)
{
	assert(ownerEntityBlock != nullptr);
	assert(entityIndexInBlock >= 0 && entityIndexInBlock < ENTITY_COUNT_IN_ENTITY_BLOCK);

	this->mViewFrustumCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
	this->mMaskedSWOcclusionCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	this->mScreenSpaceAABBCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
#endif
#ifdef ENABLE_QUERY_OCCLUSION
	this->mQueryOcclusionCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
#endif

	//Don't decrement this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it

	ownerEntityBlock->mCurrentEntityCount--;
	if (ownerEntityBlock->mCurrentEntityCount == 0)
	{
		this->FreeEntityBlock(ownerEntityBlock);
	}
	
}

std::pair<culling::EntityBlock*, unsigned int*> culling::EveryCulling::AllocateNewEntityBlockFromPool()
{
	EntityBlock* newEntityBlock = this->GetNewEntityBlockFromPool();
	this->mEntityGridCell.mEntityBlocks.push_back(newEntityBlock); 
	this->mEntityGridCell.AllocatedEntityCountInBlocks.push_back(0);
	newEntityBlock->mCurrentEntityCount = 0;

	this->mActiveEntityBlockList.push_back(newEntityBlock);

	return { this->mEntityGridCell.mEntityBlocks.back(), &(this->mEntityGridCell.AllocatedEntityCountInBlocks.back()) };
}





culling::EntityBlockViewer culling::EveryCulling::AllocateNewEntity(void* renderer)
{
	std::pair<culling::EntityBlock*, unsigned int*> targetEntityBlock;
	if (this->mEntityGridCell.mEntityBlocks.size() == 0)
	{
		// if Any entityBlock isn't allocated yet
		targetEntityBlock = this->AllocateNewEntityBlockFromPool();
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
		}
	}
	
	
	assert(*(targetEntityBlock.second) <= ENTITY_COUNT_IN_ENTITY_BLOCK); // something is weird........
	
	targetEntityBlock.first->mCurrentEntityCount++;
	(*(targetEntityBlock.second))++;

	targetEntityBlock.first->mRenderer[targetEntityBlock.first->mCurrentEntityCount - 1] = renderer;
	return EntityBlockViewer(targetEntityBlock.first, targetEntityBlock.first->mCurrentEntityCount - 1);
}

void culling::EveryCulling::RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer)
{
	//Do nothing......

	entityBlockViewer.bmIsActive = false;
	this->RemoveEntityFromBlock(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock);
	//Don't decrement this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}

culling::EveryCulling::EveryCulling(unsigned int resolutionWidth, unsigned int resolutionHeight)
	:
	mViewFrustumCulling{ this }
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	, mScreenSpaceAABBCulling{ this }
#endif
	, mMaskedSWOcclusionCulling{ this, resolutionWidth, resolutionHeight }
	, mQueryOcclusionCulling{ this }
{
	//to protect 
	this->mFreeEntityBlockList.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);
	this->mEntityGridCell.mEntityBlocks.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);
	this->mEntityGridCell.AllocatedEntityCountInBlocks.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);

	this->AllocateEntityBlockPool();

	//this->CacheCullBlockEntityJobs();
	this->bmIsEntityBlockPoolInitialized = true;

}

culling::EveryCulling::~EveryCulling()
{
	for (culling::EntityBlock* allocatedEntityBlockChunk : this->mAllocatedEntityBlockChunkList)
	{
		delete[] allocatedEntityBlockChunk;
	}
}

void culling::EveryCulling::SetCameraCount(unsigned int cameraCount)
{
	this->mCameraCount = cameraCount;
	this->mViewFrustumCulling.mCameraCount = cameraCount;
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	this->mScreenSpaceAABBCulling.mCameraCount = cameraCount;
#endif
}

void culling::EveryCulling::SetViewProjectionMatrix(const culling::Matrix4X4& viewProjectionMatrix)
{
	this->mViewProjectionMatrix = viewProjectionMatrix;
}

unsigned int culling::EveryCulling::GetCameraCount() const
{
	return this->mCameraCount;
}

std::vector<culling::EntityBlock*> culling::EveryCulling::GetActiveEntityBlockList() const
{
	return this->mActiveEntityBlockList;
}

std::function<void()> culling::EveryCulling::GetCullJob()
{
	return std::function<void()>(std::bind(&EveryCulling::CullBlockEntityJob, this));
}


