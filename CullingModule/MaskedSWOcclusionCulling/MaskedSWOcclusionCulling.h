#pragma once

#include <vector>

#include "SWDepthBuffer.h"

#include "../../DataType/Math/AABB.h"

#include "../CullingModule.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeTrianglesStage.h"
#include "Stage/SolveMeshRoleStage.h"


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


		/// <summary>
		/// Depth Test Multiple Occludees
		/// </summary>
		/// <param name="worldAABBs"></param>
		void DepthTestOccludee(const AABB* worldAABBs, size_t aabbCount, char* visibleBitFlags);

		void GetOccluderCandidates()
		{

		}

	public:

		culling::EveryCulling* const mEveryCulling;

		SolveMeshRoleStage mSolveMeshRoleStage;
		BinTrianglesStage mBinTrianglesStage;
		RasterizeTrianglesStage mRasterizeTrianglesStage;

		SWDepthBuffer mDepthBuffer;

		MaskedSWOcclusionCulling
		(
			EveryCulling* everyCulling,
			const std::uint32_t depthBufferWidth, 
			const std::uint32_t depthBufferheight
		);
	
		void ResetState();
		void CullBlockEntityJob(const size_t cameraIndex) override;
	};
}

