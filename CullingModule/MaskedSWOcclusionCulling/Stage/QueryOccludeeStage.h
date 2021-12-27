#pragma once

#include "MaskedSWOcclusionCullingStage.h"

namespace culling
{
	class QueryOccludeeStage : public MaskedSWOcclusionCullingStage
	{
	
	private:
		
		void QueryOccludee(const size_t cameraIndex, culling::EntityBlock* const entityBlock);

	public:

		QueryOccludeeStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void CullBlockEntityJob(const size_t cameraIndex) override;
	};
}


