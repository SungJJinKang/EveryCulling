#pragma once

#include <vector>
#include <functional>

#include <Matrix4x4.h>

#include "CullingModule.h"

namespace culling
{
	struct alignas(64) SIMDFrustumPlanes // for cache hit, align to 64 byte
	{
		math::Vector4 mFrustumPlanes[8]; // first 4 planes will be on same cache line
	};

	class FrotbiteCullingSystem;
	class ViewFrustumCulling : CullingModule
	{
		friend class FrotbiteCullingSystem;

	private:

		SIMDFrustumPlanes mSIMDFrustumPlanes[MAX_CAMERA_COUNT];

		ViewFrustumCulling(FrotbiteCullingSystem* frotbiteCullingSystem)
			:CullingModule{ frotbiteCullingSystem }
		{

		}

		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, unsigned int entityCountInBlock, unsigned int blockIndex, unsigned int cameraIndex) final;

	public:
		/// <summary>
		/// before Start solving culling, Update Every Camera's frustum plane
		/// Do this at main thread
		/// </summary>
		/// <param name="frustumPlaneIndex"></param>
		/// <param name="viewProjectionMatix"></param>
		void UpdateFrustumPlane(unsigned int frustumPlaneIndex, const math::Matrix4x4& viewProjectionMatrix);
		SIMDFrustumPlanes* GetSIMDPlanes();
	};
}
