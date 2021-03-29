#include "LinearTransformDataCulling.h"

#include <assert.h>
#include <utility>

#include "DataStructure/EntityBlock.h"

#include <iostream>

void doom::graphics::LinearTransformDataCulling::FreeEntityBlock(EntityBlock* freedEntityBlock)
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
}

doom::graphics::EntityBlock* doom::graphics::LinearTransformDataCulling::GetNewEntityBlockFromPool()
{
	assert(this->mFreeEntityBlockList.size() != 0);

	EntityBlock* entityBlock = this->mFreeEntityBlockList.back();
	this->mFreeEntityBlockList.pop_back();
	return entityBlock;
}

void doom::graphics::LinearTransformDataCulling::CacheCullBlockEntityJobs()
{
	for (unsigned int i = 0; i < MAX_ENTITY_BLOCK_COUNT; i++)
	{
		this->mCachedCullBlockEntityJobs.push_back(std::bind(&LinearTransformDataCulling::CullBlockEntityJob, this, i));
	}
}

doom::graphics::LinearTransformDataCulling::LinearTransformDataCulling()
{
	AllocateEntityBlockPool();

	this->CacheCullBlockEntityJobs();
	this->bmIsInitializedEntityBlockPool = true;
}

void doom::graphics::LinearTransformDataCulling::AllocateEntityBlockPool()
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

void doom::graphics::LinearTransformDataCulling::RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock)
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

doom::graphics::EntityBlock* doom::graphics::LinearTransformDataCulling::AllocateNewEntityBlockFromPool()
{
	EntityBlock* newEntityBlock = this->GetNewEntityBlockFromPool();


	this->mEntityGridCell.mBlockCount++;
	this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1] = newEntityBlock;
	this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1] = 0;
	newEntityBlock->mCurrentEntityCount = 0;
	return newEntityBlock;
}

void doom::graphics::LinearTransformDataCulling::RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer)
{
	//Do nothing......

	entityBlockViewer.bmIsActive = false;
	this->RemoveEntityFromBlock(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock);
	//Never Decrease this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}


doom::graphics::EntityBlockViewer doom::graphics::LinearTransformDataCulling::AllocateNewEntity(const math::Vector3& position, float radius)
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

	std::memcpy(entityBlockOfNewEntity->mPositions[entityIndexInBlock].data(), position.data(), sizeof(math::Vector3));
	entityBlockOfNewEntity->mPositions[entityIndexInBlock].w = radius;
	return EntityBlockViewer(entityBlockOfNewEntity, entityIndexInBlock);
}



// ////////////////////////////////////////



void doom::graphics::LinearTransformDataCulling::UpdateFrustumPlane(unsigned int frustumPlaneIndex, const math::Matrix4x4& viewProjectionMatrix)
{
	assert(frustumPlaneIndex >= 0 && frustumPlaneIndex < MAX_CAMERA_COUNT);

	math::ExtractSIMDPlanesFromViewProjectionMatrix(viewProjectionMatrix, this->mSIMDFrustumPlanes[frustumPlaneIndex].mFrustumPlanes, true); 
}

void doom::graphics::LinearTransformDataCulling::SetCameraCount(unsigned int cameraCount)
{
	this->mCameraCount = cameraCount;
}

doom::graphics::SIMDFrustumPlanes* doom::graphics::LinearTransformDataCulling::GetSIMDPlanes()
{
	return this->mSIMDFrustumPlanes;
}

void doom::graphics::LinearTransformDataCulling::CullBlockEntityJob(unsigned int blockIndex)
{
	assert(blockIndex < this->mEntityGridCell.mBlockCount);

	EntityBlock* currentEntityBlock = this->mEntityGridCell.mEntityBlocks[blockIndex];
	unsigned int entityCountInBlock = this->mEntityGridCell.AllocatedEntityCountInBlocks[blockIndex]; // don't use mCurrentEntityCount

	alignas(32) char cullingMask[ENTITY_COUNT_IN_ENTITY_BLOCK] = { 0 };
	for (unsigned int i = 0; i < this->mCameraCount; ++i)
	{
		math::Vector4* frustumPlane = this->mSIMDFrustumPlanes[i].mFrustumPlanes;
		for (unsigned int j = 0; j < entityCountInBlock; j = j + 2)
		{
			char result = math::InFrustumSIMDWithTwoPoint(frustumPlane, currentEntityBlock->mPositions + j);
			// if first low bit has 1 value, Pos A is In Frustum
			// if second low bit has 1 value, Pos A is In Frustum
			
			//for maximizing cache hit, Don't set Entity's IsVisiable at here
			cullingMask[j] |= (result & 1) << i;
			cullingMask[j + 1] |= ((result & 2) >> 1) << i;

		}

	}

	M256F* m256f_isVisible = reinterpret_cast<M256F*>(currentEntityBlock->mIsVisibleBitflag);
	const M256F* m256f_cullingMask = reinterpret_cast<const M256F*>(cullingMask);



	unsigned int m256_count_isvisible = 1 + ((currentEntityBlock->mCurrentEntityCount * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32);

	/// <summary>
	/// M256 = 8bit(1byte = bool size) * 32 
	/// 
	/// And operation with result culling mask and entityblock's visible bitflag
	/// </summary>
	for (unsigned int i = 0; i < m256_count_isvisible; i++)
	{
		m256f_isVisible[i] = _mm256_and_ps(m256f_isVisible[i], m256f_cullingMask[i]);
	}

	// We don't need this because currentEntityBlock->mIsVisibleBitflag and cullingMask is aligned to 32 byte
// 	for (; entityIndex < entityCountInBlock; entityIndex++)
// 	{
// 		currentEntityBlock->mIsVisibleBitflag[entityIndex] &= cullingMask[entityIndex];
// 	}

// 	{
// 		std::lock_guard<std::mutex> lk(this->mCullJobMutex); // Is this required??
// 
// 	}


	unsigned int finshiedBlockCount;
	{
		// even if this->mFinishedCullJobBlockCount is atomic, why need this? 
		// this will prevent from condition variable being fullfilled between Waiting thread checking predicate and waiting

		// What happen this->mFinishedCullJobBlockCount is set without mutex lock
		//
		// 1. Waiting thread wakes spuriously, aquires mutex, checks predicate and evaluates it to false, so it must wait on cv again(now thread is on between checks predicate and starting wait)
		// 2. Controlling thread sets shared variable to true.
		// 3. Controlling thread sends notification, which is not received by anybody, because there is no threads waiting on conditional variable.
		// 4. Waiting thread waits on conditional variable.Since notification was already sent, it would wait until next spurious wakeup, or next time when controlling thread sends notification.Potentially waiting indefinetly.
		
		std::scoped_lock<std::mutex> sk(this->mCullJobMutex);
		finshiedBlockCount = ++(this->mFinishedCullJobBlockCount); 
	}
	
	assert(finshiedBlockCount <= this->mEntityGridCell.mBlockCount);
	if (finshiedBlockCount == this->mEntityGridCell.mBlockCount)
	{
		this->mCullJobConditionVaraible.notify_one();
	}
}

bool doom::graphics::LinearTransformDataCulling::GetIsCullJobFinished()
{
	assert(mFinishedCullJobBlockCount <= this->mEntityGridCell.mBlockCount);
	return this->mFinishedCullJobBlockCount == this->mEntityGridCell.mBlockCount;
}

bool doom::graphics::LinearTransformDataCulling::WaitToFinishCullJobs()
{
	{
		// why need mutex lock ? 
		// read this : https://sungjjinkang.github.io/computerscience/2021/03/28/condtionvariable_atomic.html

		std::unique_lock<std::mutex> lk(this->mCullJobMutex);
		this->mCullJobConditionVaraible.wait(lk, [this] {return this->GetIsCullJobFinished(); });
		//
		//	condition variable check pred first
		//	
		//	while (!pred()) 
		//	{
		//		wait(lock);
		//	}
		//
		//

	}
	
	return true;
}

void doom::graphics::LinearTransformDataCulling::SetAllOneIsVisibleFlag()
{
	for (unsigned int i = 0; i < this->mEntityGridCell.mBlockCount; i++)
	{
		std::memset(this->mEntityGridCell.mEntityBlocks[i]->mIsVisibleBitflag, 0xFF, sizeof(bool) * ENTITY_COUNT_IN_ENTITY_BLOCK);
	}
}

void doom::graphics::LinearTransformDataCulling::ResetCullJobState()
{
	//this->mAtomicCurrentBlockIndex = 0;
	this->mFinishedCullJobBlockCount = 0;
	this->SetAllOneIsVisibleFlag();
}

