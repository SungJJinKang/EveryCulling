#include "EveryCulling.h"

#include <cstring>
#include <utility>

#include "DataType/EntityBlock.h"
#include <vector_erase_move_lastelement/vector_swap_erase.h>

void culling::EveryCulling::FreeEntityBlock(EntityBlock* freedEntityBlock)
{
	assert(freedEntityBlock != nullptr);

	size_t freedEntityBlockIndex;
	const size_t entityBlockCount = mEntityGridCell.mEntityBlocks.size();
	bool IsSuccessToFind = false;
	for (size_t i = 0; i < entityBlockCount; i++)
	{
		//Freeing entity block happen barely
		//So this looping is acceptable
		if (mEntityGridCell.mEntityBlocks[i] == freedEntityBlock)
		{
			freedEntityBlockIndex = i;
			IsSuccessToFind = true;
			break;
		}
	}

	assert(IsSuccessToFind == true);

	//swap and pop back trick
	std::vector_swap_popback(mEntityGridCell.mEntityBlocks, freedEntityBlockIndex);
	std::vector_swap_popback(mEntityGridCell.AllocatedEntityCountInBlocks, freedEntityBlockIndex);

	freedEntityBlock->mCurrentEntityCount = 0;

	mFreeEntityBlockList.push_back(freedEntityBlock);
	std::vector_find_swap_popback(mActiveEntityBlockList, freedEntityBlock);
}


culling::EntityBlock* culling::EveryCulling::GetNewEntityBlockFromPool()
{
	if (mFreeEntityBlockList.size() == 0)
	{
		AllocateEntityBlockPool();
	}

	assert(mFreeEntityBlockList.size() != 0);
	EntityBlock* entityBlock = mFreeEntityBlockList.back();
	mFreeEntityBlockList.pop_back();
	return entityBlock;
}

void culling::EveryCulling::ResetCullJobStateVariable()
{
	for (unsigned int cameraIndex = 0; cameraIndex < mCameraCount; cameraIndex++)
	{
		for (size_t moduleIndex = 0; moduleIndex < mUpdatedCullingModules.size(); moduleIndex++)
		{
			CullingModule* cullingModule = mUpdatedCullingModules[moduleIndex];
			cullingModule->mCurrentCullEntityBlockIndex[cameraIndex].store(0, std::memory_order_relaxed);
			cullingModule->mFinishedCullEntityBlockCount[cameraIndex].store(0, std::memory_order_relaxed);
		}
	}
	mIsCullJobFinished.store(false, std::memory_order_relaxed);

	//release!
	std::atomic_thread_fence(std::memory_order_seq_cst);
}

void culling::EveryCulling::SetAllOneIsVisibleFlag()
{
	//Maybe Compiler use SIMD or do faster than SIMD instruction
	for (auto entityBlock : mEntityGridCell.mEntityBlocks)
	{
		std::memset(entityBlock->mIsVisibleBitflag, 0xFF, sizeof(char) * ENTITY_COUNT_IN_ENTITY_BLOCK);
	}
}

void culling::EveryCulling::AllocateEntityBlockPool()
{
	EntityBlock* newEntityBlockChunk = new EntityBlock[INITIAL_ENTITY_BLOCK_COUNT];
	for (unsigned int i = 0; i < INITIAL_ENTITY_BLOCK_COUNT; i++)
	{
		mFreeEntityBlockList.push_back(newEntityBlockChunk + i);
	}
	mAllocatedEntityBlockChunkList.push_back(newEntityBlockChunk);
}

void culling::EveryCulling::ResetEntityBlock(culling::EntityBlock* entityBlock)
{
	entityBlock->mCurrentEntityCount = 0;
	std::memset(entityBlock->mQueryObjects, 0x00, sizeof(decltype(*(entityBlock->mQueryObjects))) * culling::ENTITY_COUNT_IN_ENTITY_BLOCK);
	
}

void culling::EveryCulling::RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock)
{
	assert(ownerEntityBlock != nullptr);
	assert(entityIndexInBlock >= 0 && entityIndexInBlock < ENTITY_COUNT_IN_ENTITY_BLOCK);

	mViewFrustumCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
	mMaskedSWOcclusionCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	mScreenSpaceBoudingSphereCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
#endif
#ifdef ENABLE_QUERY_OCCLUSION
	mQueryOcclusionCulling.ClearEntityData(ownerEntityBlock, entityIndexInBlock);
#endif

	//Don't decrement mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it

	ownerEntityBlock->mCurrentEntityCount--;
	if (ownerEntityBlock->mCurrentEntityCount == 0)
	{
		FreeEntityBlock(ownerEntityBlock);
	}
	
}

std::pair<culling::EntityBlock*, unsigned int*> culling::EveryCulling::AllocateNewEntityBlockFromPool()
{
	EntityBlock* newEntityBlock = GetNewEntityBlockFromPool();
	mEntityGridCell.mEntityBlocks.push_back(newEntityBlock); 
	mEntityGridCell.AllocatedEntityCountInBlocks.push_back(0);
	ResetEntityBlock(newEntityBlock);

	mActiveEntityBlockList.push_back(newEntityBlock);

	return { mEntityGridCell.mEntityBlocks.back(), &(mEntityGridCell.AllocatedEntityCountInBlocks.back()) };
}





culling::EntityBlockViewer culling::EveryCulling::AllocateNewEntity(void* renderer)
{
	std::pair<culling::EntityBlock*, unsigned int*> targetEntityBlock;
	if (mEntityGridCell.mEntityBlocks.size() == 0)
	{
		// if Any entityBlock isn't allocated yet
		targetEntityBlock = AllocateNewEntityBlockFromPool();
	}
	else
	{//When Allocated entity block count is at least one

		//Get last entityblock in active entities
		targetEntityBlock = { mEntityGridCell.mEntityBlocks.back(), &(mEntityGridCell.AllocatedEntityCountInBlocks.back()) };

		if (*(targetEntityBlock.second) == ENTITY_COUNT_IN_ENTITY_BLOCK)
		{
			//if last entityblock in active entities is full of entities
			//alocate new entity block
			targetEntityBlock = AllocateNewEntityBlockFromPool();
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
	RemoveEntityFromBlock(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock);
	//Don't decrement mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}

culling::EveryCulling::EveryCulling(unsigned int resolutionWidth, unsigned int resolutionHeight)
	:
	mViewFrustumCulling{ this }
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	, mScreenSpaceBoudingSphereCulling{ this }
#endif
	, mMaskedSWOcclusionCulling{ this, resolutionWidth, resolutionHeight }
	, mQueryOcclusionCulling{ this }
{
	//to protect 
	mFreeEntityBlockList.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);
	mEntityGridCell.mEntityBlocks.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);
	mEntityGridCell.AllocatedEntityCountInBlocks.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);

	AllocateEntityBlockPool();

	//CacheCullBlockEntityJobs();
	bmIsEntityBlockPoolInitialized = true;

}

culling::EveryCulling::~EveryCulling()
{
	for (culling::EntityBlock* allocatedEntityBlockChunk : mAllocatedEntityBlockChunkList)
	{
		delete[] allocatedEntityBlockChunk;
	}
}

void culling::EveryCulling::SetCameraCount(unsigned int cameraCount)
{
	mCameraCount = cameraCount;
	mViewFrustumCulling.mCameraCount = cameraCount;
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	mScreenSpaceBoudingSphereCulling.mCameraCount = cameraCount;
#endif
}

void culling::EveryCulling::SetViewProjectionMatrix(const culling::Matrix4X4& viewProjectionMatrix)
{
	IS_ALIGNED_ASSERT(reinterpret_cast<size_t>(&viewProjectionMatrix), 32);
	mViewProjectionMatrix = viewProjectionMatrix;
}

unsigned int culling::EveryCulling::GetCameraCount() const
{
	return mCameraCount;
}

std::vector<culling::EntityBlock*> culling::EveryCulling::GetActiveEntityBlockList() const
{
	return mActiveEntityBlockList;
}

std::function<void()> culling::EveryCulling::GetCullJob()
{
	return std::function<void()>(std::bind(&EveryCulling::CullBlockEntityJob, this));
}


