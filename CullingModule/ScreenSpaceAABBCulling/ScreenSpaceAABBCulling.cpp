#include "../../FrotbiteCullingSystemCore.h"

#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING

#include "ScreenSpaceAABBCulling.h"

void culling::ScreenSpaceAABBCulling::CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex)
{
	alignas(32) char cullingMask[ENTITY_COUNT_IN_ENTITY_BLOCK] = { 0 };
	Vector4 mScreenSpaceAABBMin;
	Vector4 mScreenSpaceAABBMax;
	float x, y, area;
	for (unsigned int j = 0; j < entityCountInBlock; j++)
	{
		//TODO : Think How to use SIMD

		culling::AABB& entityAABB = currentEntityBlock->mWorldAABB[j];

		mScreenSpaceAABBMin = this->mViewProjectionMatrix * entityAABB.mMin;
		mScreenSpaceAABBMax = this->mViewProjectionMatrix * entityAABB.mMax;

		mScreenSpaceAABBMin /= mScreenSpaceAABBMin.w;
		mScreenSpaceAABBMax /= mScreenSpaceAABBMax.w;

		//x = math::Max(mScreenSpaceAABBMin.x, mScreenSpaceAABBMax.x) - math::Min(mScreenSpaceAABBMin.x, mScreenSpaceAABBMax.x);
		//y = math::Max(mScreenSpaceAABBMin.y, mScreenSpaceAABBMax.y) - math::Min(mScreenSpaceAABBMin.y, mScreenSpaceAABBMax.y);
		x = math::abs(mScreenSpaceAABBMin.x - mScreenSpaceAABBMax.x);
		y = math::abs(mScreenSpaceAABBMin.y - mScreenSpaceAABBMax.y);
		
		area = x * y;

		if (area > this->mMinimumScreenSpaceAABBArea)
		{
			cullingMask[j] |= 1 << cameraIndex;
		}
	}

	//TODO : If CullingMask is True, Do Calculate ScreenSpace AABB Area And Check Is Culled
	// use mCulledScreenSpaceAABBArea
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
}

void culling::ScreenSpaceAABBCulling::SetViewProjectionMatrix(const math::Matrix4x4& viewProjectionMatrix)
{
	this->mViewProjectionMatrix = viewProjectionMatrix;
}

#endif