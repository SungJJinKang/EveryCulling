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
	class DOOM_API ViewFrustumCulling : CullingModule
	{
		friend class EveryCulling;

	private:

		SIMDFrustumPlanes mSIMDFrustumPlanes[MAX_CAMERA_COUNT];

		ViewFrustumCulling(EveryCulling* frotbiteCullingSystem)
			:CullingModule{ frotbiteCullingSystem }
		{

		}

		void CullBlockEntityJob(EntityBlock* currentEntityBlock, SIZE_T entityCountInBlock, SIZE_T cameraIndex);

		//TODO : Add AVX2(__m256) version of this function
		char CheckInFrustumSIMDWithTwoPoint(const Vec4* eightPlanes, const Position_BoundingSphereRadius* twoPoint);

	public:

		/// <summary>
		/// before Start solving culling, Update Every Camera's frustum plane
		/// Do this at main thread
		/// </summary>
		/// <param name="cameraIndex"></param>
		/// <param name="viewProjectionMatix"></param>
		virtual void SetViewProjectionMatrix(const UINT32 cameraIndex, const Mat4x4& viewProjectionMatrix) final;

		FORCE_INLINE culling::SIMDFrustumPlanes* GetSIMDPlanes()
		{
			return mSIMDFrustumPlanes;
		}


	};
}
