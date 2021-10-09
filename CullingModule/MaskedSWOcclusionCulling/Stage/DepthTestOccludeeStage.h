#pragma once

#include "MaskedSWOcclusionCullingStage.h"

namespace culling
{
	class DOOM_API DepthTestOccludeeStage : public MaskedSWOcclusionCullingStage
	{

	private:

	public:

		DepthTestOccludeeStage(MaskedSWOcclusionCulling& mOcclusionCulling);

	};
}


