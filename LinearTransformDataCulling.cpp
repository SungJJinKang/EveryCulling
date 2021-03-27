#include "LinearTransformDataCulling.h"

#include <assert.h>

#include "DataStructure/EntityBlock.h"


doom::graphics::LinearTransformDataCulling::LinearTransformDataCulling()
{
	/// <summary>
	/// Entity Block size should be less than 4KB(Page Size On Window Platform)
	/// </summary>
	assert(sizeof(EntityBlock) < 4000);
	this->EntityBlockPool = new EntityBlock[MAX_ENTITY_BLOCK_COUNT];
	this->bmIsInitializedEntityBlockPool = true;
}

void doom::graphics::LinearTransformDataCulling::UpdateFrustumPlane(unsigned int frustumPlaneIndex, const math::Matrix4x4& mvpMatrix)
{
	assert(frustumPlaneIndex >= 0 && frustumPlaneIndex < MAX_CAMERA_COUNT);
	math::ExtractSIMDPlanesFromMVPMatrix(mvpMatrix, this->mSIMDFrustumPlanes[frustumPlaneIndex].mFrustumPlanes, true); // TODO : normalize 해야하는지 check 하자
}

void doom::graphics::LinearTransformDataCulling::CullBlockEntityJob()
{
	int currentBlockIndex = this->mAtomicCurrentBlockIndex++;

	if (currentBlockIndex >= this->mEntityGridCell.mBlockCount)
	{
		return; // Alread All blocks is executed culling job
	}

	EntityBlock* currentEntityBlock = this->mEntityGridCell.mEntityBlocks[currentBlockIndex];
	unsigned int entityCountInBlock = this->mEntityGridCell.EntityCountInBlocks[currentBlockIndex];

	alignas(32) bool cullingMask[ENTITY_COUNT_IN_ENTITY_BLOCK] = { 0 };
	for (unsigned int i = 0; i < this->mCameraCount; ++i)
	{
		math::Vector4* frustumPlane = this->mSIMDFrustumPlanes[i].mFrustumPlanes;
		for (unsigned int j = 0; j < entityCountInBlock; j = j + 2)
		{
			unsigned int result = math::InFrustumSIMDWithTwoPoint(frustumPlane, &(currentEntityBlock->mPositions[j]));
			
			unsigned int posAResult = result & 1; // if first low bit has 1 value, Pos A is In Frustum
			unsigned int posBResult = (result & 2) >> 1; // if second low bit has 1 value, Pos A is In Frustum
			//for maximizing cache hit, Don't set Entity's IsVisiable at here
			cullingMask[j] |= posAResult << i;
			cullingMask[j + 1] |= posBResult << i;
		}

	}

	M256F* m256f_isVisible = reinterpret_cast<M256F*>(currentEntityBlock->mIsVisible);
	const M256F* m256f_cullingMask = reinterpret_cast<const M256F*>(cullingMask);

	unsigned int entityIndex = 0;
	/// <summary>
	/// M256 = 8bit(1byte = bool size) * 32 
	/// </summary>
	while (entityIndex + 32 <= entityCountInBlock)
	{
		m256f_isVisible[entityIndex / 32] = _mm256_and_ps(m256f_isVisible[entityIndex / 32], m256f_cullingMask[entityIndex / 32]);
		entityIndex += 32;
	}
	
	for (; entityIndex < entityCountInBlock; entityIndex++)
	{
		currentEntityBlock->mIsVisible[entityIndex] &= cullingMask[entityIndex];
	}
	
}

void doom::graphics::LinearTransformDataCulling::ClearIsVisibleFlag()
{
	for (unsigned int i = 0; i < this->mEntityGridCell.mBlockCount; i++)
	{
		std::memset(this->mEntityGridCell.mEntityBlocks[i]->mIsVisible, 0, sizeof(bool) * ENTITY_COUNT_IN_ENTITY_BLOCK);
		
	}
}

void doom::graphics::LinearTransformDataCulling::ResetAtomicCurrentBlockIndex()
{
	this->mAtomicCurrentBlockIndex = 0;
}

