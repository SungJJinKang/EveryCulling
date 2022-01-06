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

		void ResetDepthBuffer();



		std::atomic<size_t> mBinnedOccluderCount;

	public:
		
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
	
		void ResetState();
		void CullBlockEntityJob(const size_t cameraIndex) override;
		const char* GetCullingModuleName() const override;

		/// <summary>
		/// If fail, return 0
		/// </summary>
		/// <returns></returns>
		FORCE_INLINE size_t IncreamentBinnedOccluderCountIfPossible()
		{
			const size_t increamentedBinnedOccluderCount = mBinnedOccluderCount.fetch_add(1, std::memory_order_seq_cst);

			if(increamentedBinnedOccluderCount < MAX_OCCLUDER_COUNT)
			{
				return increamentedBinnedOccluderCount;
			}
			else
			{
				mBinnedOccluderCount.fetch_add(-1, std::memory_order_seq_cst);
				return 0;
			}
			
		}

		FORCE_INLINE bool GetIsBinnedOccluderListFull() const
		{
			return mBinnedOccluderCount >= MAX_OCCLUDER_COUNT;
		}

		FORCE_INLINE bool IsOccluderExist() const
		{
			return mBinnedOccluderCount > 0;
		}

		FORCE_INLINE size_t GetOccluderCount() const
		{
			return mBinnedOccluderCount;
		}
	};
}

