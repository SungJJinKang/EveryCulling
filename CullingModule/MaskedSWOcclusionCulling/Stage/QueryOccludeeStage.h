#pragma once

#include "MaskedSWOcclusionCullingStage.h"

namespace culling
{
	class QueryOccludeeStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

		/// <summary>
		/// Depth Test Multiple Occludees
		/// </summary>
		/// <param name="worldAABBs"></param>
		//void DepthTestOccludee(const AABB* worldAABBs, size_t aabbCount, char* visibleBitFlags);


	public:

		QueryOccludeeStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void CullBlockEntityJob(const size_t cameraIndex) override;
	};
}


