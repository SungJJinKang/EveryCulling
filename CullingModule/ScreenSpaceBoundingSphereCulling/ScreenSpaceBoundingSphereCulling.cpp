#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING

#include "ScreenSpaceBoundingSphereCulling.h"
#include "../../DataType/Math/Common.h"

#include "../../EveryCulling.h"

culling::ScreenSpaceBoundingSphereCulling::ScreenSpaceBoundingSphereCulling(EveryCulling* everyCulling) 
	: CullingModule{ everyCulling }
{

}

void culling::ScreenSpaceBoundingSphereCulling::CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex)
{
	alignas(32) char cullingMask[ENTITY_COUNT_IN_ENTITY_BLOCK] = { 0 };
	culling::Vector4 mScreenSpaceAABBMin;
	culling::Vector4 mScreenSpaceAABBMax;
	float x, y, area;
	for (std::uint32_t j = 0; j < entityCountInBlock; j++)
	{
		//TODO : Think How to use SIMD

		/*
		const culling::AABB& entityAABB = currentEntityBlock->mWorldAABB[j];

		// TODO : This is completly wrong, 
		// 8 corner should be projected to screen space
		mScreenSpaceAABBMin = mCameraViewProjectionMatrixs[cameraIndex] * entityAABB.mMin;
		mScreenSpaceAABBMax = mCameraViewProjectionMatrixs[cameraIndex] * entityAABB.mMax;

		mScreenSpaceAABBMin.values[0] /= mScreenSpaceAABBMin.values[3];
		mScreenSpaceAABBMin.values[1] /= mScreenSpaceAABBMin.values[3];
		mScreenSpaceAABBMin.values[2] /= mScreenSpaceAABBMin.values[3];
		mScreenSpaceAABBMin.values[3] /= mScreenSpaceAABBMin.values[3];

		mScreenSpaceAABBMax.values[0] /= mScreenSpaceAABBMax.values[3];
		mScreenSpaceAABBMax.values[1] /= mScreenSpaceAABBMax.values[3];
		mScreenSpaceAABBMax.values[2] /= mScreenSpaceAABBMax.values[3];
		mScreenSpaceAABBMax.values[3] /= mScreenSpaceAABBMax.values[3];

		//x = math::Max(mScreenSpaceAABBMin.x, mScreenSpaceAABBMax.x) - math::Min(mScreenSpaceAABBMin.x, mScreenSpaceAABBMax.x);
		//y = math::Max(mScreenSpaceAABBMin.y, mScreenSpaceAABBMax.y) - math::Min(mScreenSpaceAABBMin.y, mScreenSpaceAABBMax.y);
		x = ABS(mScreenSpaceAABBMin.values[0] - mScreenSpaceAABBMax.values[0]);
		y = ABS(mScreenSpaceAABBMin.values[1] - mScreenSpaceAABBMax.values[1]);
		
		area = x * y;

		if (area > mMinimumScreenSpaceAABBArea)
		{
			cullingMask[j] |= 1 << cameraIndex;
		}
		*/
	}

	//TODO : If CullingMask is True, Do Calculate ScreenSpace AABB Area And Check Is Culled
	// use mCulledScreenSpaceAABBArea
	culling::M256F* m256f_isVisible = reinterpret_cast<culling::M256F*>(currentEntityBlock->mIsVisibleBitflag);
	const culling::M256F* m256f_cullingMask = reinterpret_cast<const culling::M256F*>(cullingMask);



	std::uint32_t m256_count_isvisible = 1 + ((currentEntityBlock->mCurrentEntityCount * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / sizeof(culling::M256F));

	/// <summary>
	/// M256 = 8bit(1byte = bool size) * 32 
	/// 
	/// And operation with result culling mask and entityblock's visible bitflag
	/// </summary>
	for (std::uint32_t i = 0; i < m256_count_isvisible; i++)
	{
		m256f_isVisible[i] = _mm256_and_ps(m256f_isVisible[i], m256f_cullingMask[i]);
	}
}


#endif