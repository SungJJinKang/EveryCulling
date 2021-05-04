#pragma once

namespace culling
{
	class MaskedSWOcclusionCulling;
	class MaskedSWOcclusionCullingStage
	{

	protected:
		MaskedSWOcclusionCulling& mMaskedSWOcclusionCulling;
		MaskedSWOcclusionCullingStage(MaskedSWOcclusionCulling& mOcclusionCulling)
			: mMaskedSWOcclusionCulling{ mOcclusionCulling }
		{

		}

	};
}


