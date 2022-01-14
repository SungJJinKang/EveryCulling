#pragma once

#include <functional>

#include "../../EveryCullingCore.h"

#include "../../DataType/Math/Matrix.h"
#include "../../DataType/Math/Vector.h"
#include "../../DataType/Position_BoundingSphereRadius.h"

#include "../CullingModule.h"



namespace culling
{
	struct alignas(64) SIMDFrustumPlanes // for cache hit, align to 64 byte
	{
		Vec4 mFrustumPlanes[8]; // first 4 planes will be on same cache line
	};

	class EveryCulling;
	class ViewFrustumCulling : public CullingModule
	{
	private:

		SIMDFrustumPlanes mSIMDFrustumPlanes[MAX_CAMERA_COUNT];

		
		//TODO : Add AVX2(__m256) version of this function
		char CheckInFrustumSIMDWithTwoPoint(const Vec4* eightPlanes, const Position_BoundingSphereRadius* twoPoint);
		
		void DoViewFrustumCulling
		(
			const size_t cameraIndex,
			culling::EntityBlock* const entityBlock
		);

	public:

		ViewFrustumCulling(EveryCulling* frotbiteCullingSystem);

		/// <summary>
		/// before Start solving culling, Update Every Camera's frustum plane
		/// Do this at main thread
		/// </summary>
		/// <param name="cameraIndex"></param>
		/// <param name="viewProjectionMatix"></param>
		virtual void OnSetViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& cameraViewProjectionMatrix) final;

		EVERYCULLING_FORCE_INLINE culling::SIMDFrustumPlanes* GetSIMDPlanes()
		{
			return mSIMDFrustumPlanes;
		}

		virtual void CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount);
		const char* GetCullingModuleName() const override;
	};
}
