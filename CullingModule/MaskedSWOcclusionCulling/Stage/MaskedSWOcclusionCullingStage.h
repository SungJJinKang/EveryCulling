#pragma once

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


