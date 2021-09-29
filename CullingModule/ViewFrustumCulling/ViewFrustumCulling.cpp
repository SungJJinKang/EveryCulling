#include "ViewFrustumCulling.h"

#include <cassert>

#include "../../DataType/Math/Common.h"
#include "../../EveryCulling.h"

#include <Rendering/Renderer/Renderer.h>

void culling::ViewFrustumCulling::CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex)
{
	alignas(64) char cullingMask[ENTITY_COUNT_IN_ENTITY_BLOCK] = { 0 };

	const Vector4* frustumPlane = mSIMDFrustumPlanes[cameraIndex].mFrustumPlanes;
	for (size_t entityIndex = 0; entityIndex < entityCountInBlock; entityIndex = entityIndex + 2)
	{
		doom::Renderer* renderer = static_cast<doom::Renderer*>(currentEntityBlock->mRenderer[entityIndex]);

		const float worldRadius = renderer->doom::ColliderUpdater<doom::physics::Sphere>::GetWorldCollider()->mRadius;

		const math::Vector3& renderedObjectPos = renderer->GetTransform()->GetPosition();

		currentEntityBlock->mPositions[entityIndex].SetPosition(reinterpret_cast<const void*>(&renderedObjectPos));
		currentEntityBlock->mPositions[entityIndex].SetBoundingSphereRadius(worldRadius);

		char result = CheckInFrustumSIMDWithTwoPoint(frustumPlane, currentEntityBlock->mPositions + entityIndex);
		// if first low bit has 1 value, Pos A is In Frustum
		// if second low bit has 1 value, Pos A is In Frustum

		//for maximizing cache hit, Don't set Entity's IsVisiable at here
		cullingMask[entityIndex] |= (result | ~1) << cameraIndex;
		cullingMask[entityIndex + 1] |= ((result | ~2) >> 1) << cameraIndex;

	}

	//TODO : If CullingMask is True, Do Calculate ScreenSpace AABB Area And Check Is Culled
	// use mCulledScreenSpaceAABBArea
	M256F* m256f_isVisible = reinterpret_cast<M256F*>(currentEntityBlock->mIsVisibleBitflag);
	const M256F* m256f_cullingMask = reinterpret_cast<const M256F*>(cullingMask);
	const unsigned int m256_count_isvisible = 1 + ((currentEntityBlock->mCurrentEntityCount * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / sizeof(M256F));

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

void culling::ViewFrustumCulling::SetViewProjectionMatrix(const unsigned int cameraIndex, const Matrix4X4& viewProjectionMatrix)
{
	culling::CullingModule::SetViewProjectionMatrix(cameraIndex, viewProjectionMatrix);

	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	ExtractSIMDPlanesFromViewProjectionMatrix(viewProjectionMatrix, mSIMDFrustumPlanes[cameraIndex].mFrustumPlanes, true);
}

