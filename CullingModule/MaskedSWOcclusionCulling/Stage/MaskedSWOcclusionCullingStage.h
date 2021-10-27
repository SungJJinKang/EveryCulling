#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	class MaskedSWOcclusionCulling;
	class MaskedSWOcclusionCullingStage
	{

	protected:
		MaskedSWOcclusionCulling& mMaskedOcclusionCulling;
		MaskedSWOcclusionCullingStage(MaskedSWOcclusionCulling& mOcclusionCulling)
			: mMaskedOcclusionCulling{ mOcclusionCulling }
		{

		}

	};
}


