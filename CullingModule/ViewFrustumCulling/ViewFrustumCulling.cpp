#include "ViewFrustumCulling.h"

#include <cassert>

#include "../../FrotbiteCullingSystem.h"

void culling::ViewFrustumCulling::UpdateFrustumPlane(unsigned int frustumPlaneIndex, const math::Matrix4x4& viewProjectionMatrix)
{
	assert(frustumPlaneIndex >= 0 && frustumPlaneIndex < MAX_CAMERA_COUNT);

	math::ExtractSIMDPlanesFromViewProjectionMatrix(viewProjectionMatrix, this->mSIMDFrustumPlanes[frustumPlaneIndex].mFrustumPlanes, true);
}

culling::SIMDFrustumPlanes* culling::ViewFrustumCulling::GetSIMDPlanes()
{
	return this->mSIMDFrustumPlanes;
}

void culling::ViewFrustumCulling::CullBlockEntityJob(EntityBlock* currentEntityBlock, unsigned int entityCountInBlock, unsigned int blockIndex, unsigned int cameraIndex)
{
	alignas(32) char cullingMask[ENTITY_COUNT_IN_ENTITY_BLOCK] = { 0 };

	math::Vector4* frustumPlane = this->mSIMDFrustumPlanes[cameraIndex].mFrustumPlanes;
	for (unsigned int j = 0; j < entityCountInBlock; j = j + 2)
	{
		char result = math::CheckInFrustumSIMDWithTwoPoint(frustumPlane, currentEntityBlock->mPositions + j);
		// if first low bit has 1 value, Pos A is In Frustum
		// if second low bit has 1 value, Pos A is In Frustum

		//for maximizing cache hit, Don't set Entity's IsVisiable at here
		cullingMask[j] |= (result & 1) << cameraIndex;
		cullingMask[j + 1] |= ((result & 2) >> 1) << cameraIndex;

	}



	//TODO : If CullingMask is True, Do Calculate ScreenSpace AABB Area And Check Is Culled
	// use mCulledScreenSpaceAABBArea
	M256F* m256f_isVisible = reinterpret_cast<M256F*>(currentEntityBlock->mIsVisibleBitflag);
	const M256F* m256f_cullingMask = reinterpret_cast<const M256F*>(cullingMask);



	unsigned int m256_count_isvisible = 1 + ((currentEntityBlock->mCurrentEntityCount * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / sizeof(M256F));

	/// <summary>
	/// M256 = 8bit(1byte = bool size) * 32 
	/// 
	/// And operation with result culling mask and entityblock's visible bitflag
	/// </summary>
	for (unsigned int i = 0; i < m256_count_isvisible; i++)
	{
		m256f_isVisible[i] = _mm256_and_ps(m256f_isVisible[i], m256f_cullingMask[i]);
	}

}
