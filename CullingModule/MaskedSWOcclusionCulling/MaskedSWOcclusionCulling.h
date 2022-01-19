#pragma once

#include <vector>
#include <array>

#include "SWDepthBuffer.h"

#include "../../DataType/Math/AABB.h"

#include "../CullingModule.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeOccludersStage.h"
#include "Stage/SolveMeshRoleStage.h"
#include "Stage/QueryOccludeeStage.h"

#include "OccluderListManager.h"

#define INVALID_BINNED_OCCLUDER_COUNT (std::int32_t)(-1)

namespace culling
{
	

	class MaskedSWOcclusionCullingStage;
	/// <summary>
	/// 
	/// Masked SW Occlusion Culling
	/// 
	/// 
	/// Supported SIMD Version : AVX2
	/// 
	/// How Wokrs ? : 
	/// Please read "MaskedSWOcclusionCulling_HowWorks.md" file
	/// 
	/// references : 
	/// https://www.slideshare.net/IntelSoftware/masked-software-occlusion-culling
	/// https://www.slideshare.net/IntelSoftware/masked-occlusion-culling
	/// </summary>
	class MaskedSWOcclusionCulling : public CullingModule
	{
	private:


		const std::uint32_t binCountInRow, binCountInColumn;

		void ResetDepthBuffer(const unsigned long long currentTickCount);

		std::atomic<bool> mIsOccluderExist;

		

		

	public:

		OccluderListManager mOccluderListManager;
		
		culling::EveryCulling* const mEveryCulling;

		SolveMeshRoleStage mSolveMeshRoleStage;
		BinTrianglesStage mBinTrianglesStage;
		RasterizeOccludersStage mRasterizeTrianglesStage;
		QueryOccludeeStage mQueryOccludeeStage;

		SWDepthBuffer mDepthBuffer;

		MaskedSWOcclusionCulling
		(
			EveryCulling* everyCulling,
			const std::uint32_t depthBufferWidth, 
			const std::uint32_t depthBufferheight
		);
	
		void ResetState(const unsigned long long currentTickCount);
		void CullBlockEntityJob(const size_t cameraIndex, const std::int32_t localThreadIndex, const unsigned long long currentTickCount) override;
		const char* GetCullingModuleName() const override;

		void SetIsOccluderExistTrue();
		bool GetIsOccluderExist() const;
	};
}

