#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	class MaskedSWOcclusionCulling;
	class DOOM_API MaskedSWOcclusionCullingStage
	{

	protected:
		MaskedSWOcclusionCulling& mMaskedOcclusionCulling;
		MaskedSWOcclusionCullingStage(MaskedSWOcclusionCulling& mOcclusionCulling)
			: mMaskedOcclusionCulling{ mOcclusionCulling }
		{

		}

	};
}


