#pragma once

#include "../../../EveryCullingCore.h"
#include "../../../CullingModule/CullingModule.h"

namespace culling
{
	struct EntityBlock;
	class MaskedSWOcclusionCulling;
	class MaskedSWOcclusionCullingStage : public CullingModule
	{

	public:

		MaskedSWOcclusionCulling* const mMaskedOcclusionCulling;
		MaskedSWOcclusionCullingStage(MaskedSWOcclusionCulling* const mOcclusionCulling);

		virtual void CullBlockEntityJob
		(
			const size_t cameraIndex, const unsigned long long currentTickCount
		) = 0;
		const char* GetCullingModuleName() const override;
	};
}


