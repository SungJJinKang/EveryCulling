#include "LinearTransformDataCulling.h"

#include <assert.h>
#include <utility>

#include "DataStructure/EntityBlock.h"


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

doom::graphics::LinearTransformDataCulling::LinearTransformDataCulling()
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
	this->bmIsInitializedEntityBlockPool = true;
}

void doom::graphics::LinearTransformDataCulling::RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock)
{
	assert(ownerEntityBlock != nullptr);
	assert(entityIndexInBlock >= 0 && entityIndexInBlock < ENTITY_COUNT_IN_ENTITY_BLOCK);

	ownerEntityBlock->mCurrentEntityCount--;
	if (ownerEntityBlock->mCurrentEntityCount == 0)
	{
		this->FreeEntityBlock(ownerEntityBlock);
	}
	//Do nothing......

	//Never Decrease this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}

void doom::graphics::LinearTransformDataCulling::RemoveEntityFromBlock(EntityBlockViewer* entityBlockViewer)
{
	//Do nothing......

	entityBlockViewer->bmIsActive = false;
	this->RemoveEntityFromBlock(entityBlockViewer->mTargetEntityBlock, entityBlockViewer->mEntityIndexInBlock);
	//Never Decrease this->mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}


doom::graphics::EntityBlockViewer doom::graphics::LinearTransformDataCulling::AllocateNewEntity(const math::Vector3& position, float radius)
{
	EntityBlock* lastEntityBlock = this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1];
	unsigned int currentEntityCountInEntityBlock = this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1];
	
	assert(currentEntityCountInEntityBlock > MAX_ENTITY_BLOCK_COUNT); // something is weird........
	
	EntityBlock* entityBlockOfNewEntity{ nullptr };
	unsigned int entityIndexInBlock;

	if (currentEntityCountInEntityBlock < MAX_ENTITY_BLOCK_COUNT)
	{
		//lastEntityBlock have empty Entity space
		lastEntityBlock->mCurrentEntityCount++;
		this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1]++;

		entityBlockOfNewEntity = lastEntityBlock;
		entityIndexInBlock = currentEntityCountInEntityBlock;
	}
	else if (currentEntityCountInEntityBlock == MAX_ENTITY_BLOCK_COUNT)
	{
		//lastEntityBlock is full, Get new entity block from pool
		EntityBlock* newEntityBlock = this->GetNewEntityBlockFromPool();
		
		this->mEntityGridCell.mBlockCount++;
		this->mEntityGridCell.mEntityBlocks[this->mEntityGridCell.mBlockCount - 1] = newEntityBlock;
		this->mEntityGridCell.AllocatedEntityCountInBlocks[this->mEntityGridCell.mBlockCount - 1] = 1;
		newEntityBlock->mCurrentEntityCount = 1;

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



void doom::graphics::LinearTransformDataCulling::UpdateFrustumPlane(unsigned int frustumPlaneIndex, const math::Matrix4x4& mvpMatrix)
{
	assert(frustumPlaneIndex >= 0 && frustumPlaneIndex < MAX_CAMERA_COUNT);
	math::ExtractSIMDPlanesFromMVPMatrix(mvpMatrix, this->mSIMDFrustumPlanes[frustumPlaneIndex].mFrustumPlanes, true); // TODO : normalize 해야하는지 check 하자
}

void doom::graphics::LinearTransformDataCulling::CullBlockEntityJob()
{
	unsigned int currentBlockIndex = this->mAtomicCurrentBlockIndex++; // return curren block index and increment it

	if (currentBlockIndex >= this->mEntityGridCell.mBlockCount)
	{
		return; // Alread All blocks is executed culling job
	}

	EntityBlock* currentEntityBlock = this->mEntityGridCell.mEntityBlocks[currentBlockIndex];
	unsigned int entityCountInBlock = this->mEntityGridCell.AllocatedEntityCountInBlocks[currentBlockIndex]; // don't use mCurrentEntityCount

	alignas(32) bool cullingMask[ENTITY_COUNT_IN_ENTITY_BLOCK] = { 0 };
	for (unsigned int i = 0; i < this->mCameraCount; ++i)
	{
		math::Vector4* frustumPlane = this->mSIMDFrustumPlanes[i].mFrustumPlanes;
		for (unsigned int j = 0; j < entityCountInBlock; j = j + 2)
		{
			bool result = math::InFrustumSIMDWithTwoPoint(frustumPlane, currentEntityBlock->mPositions + j);
			// if first low bit has 1 value, Pos A is In Frustum
			// if second low bit has 1 value, Pos A is In Frustum


			//for maximizing cache hit, Don't set Entity's IsVisiable at here
			cullingMask[j] |= (result & 1) << i; 
			cullingMask[j + 1] |= ((result & 2) >> 1) << i; 
		}

	}

	M256F* m256f_isVisible = reinterpret_cast<M256F*>(currentEntityBlock->mIsVisibleBitflag);
	const M256F* m256f_cullingMask = reinterpret_cast<const M256F*>(cullingMask);

	
	
	unsigned int m256_count_isvisible = 1 + ( ( currentEntityBlock->mCurrentEntityCount * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1 ) / 32 );
	
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
	mCullJobFinishedBlockCount++;
}

bool doom::graphics::LinearTransformDataCulling::GetIsCullJobFinished()
{
	return this->mCullJobFinishedBlockCount == this->mEntityGridCell.mBlockCount;
}

void doom::graphics::LinearTransformDataCulling::ClearIsVisibleFlag()
{
	for (unsigned int i = 0; i < this->mEntityGridCell.mBlockCount; i++)
	{
		std::memset(this->mEntityGridCell.mEntityBlocks[i]->mIsVisibleBitflag, 0, sizeof(bool) * ENTITY_COUNT_IN_ENTITY_BLOCK);
	}
}

void doom::graphics::LinearTransformDataCulling::ResetCullJobState()
{
	this->mAtomicCurrentBlockIndex = 0;
	this->mCullJobFinishedBlockCount = 0;
}

