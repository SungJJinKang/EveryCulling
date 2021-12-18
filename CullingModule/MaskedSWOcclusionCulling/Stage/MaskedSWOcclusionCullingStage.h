#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	struct EntityBlock;
	class MaskedSWOcclusionCulling;
	class MaskedSWOcclusionCullingStage
	{

	protected:
		MaskedSWOcclusionCulling& mMaskedOcclusionCulling;
		MaskedSWOcclusionCullingStage(MaskedSWOcclusionCulling& mOcclusionCulling)
			: mMaskedOcclusionCulling{ mOcclusionCulling }
		{

		}

	public:

		virtual void DoStageJob
		(
			EntityBlock* const currentEntityBlock,
			const size_t entityIndex,
			const size_t cameraIndex
		) = 0;

	};
}


