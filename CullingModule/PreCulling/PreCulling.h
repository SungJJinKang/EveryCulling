#pragma once

#include "../../EveryCullingCore.h"

#include "../CullingModule.h"

namespace culling
{
	class PreCulling : public CullingModule
	{
	private:

		EVERYCULLING_FORCE_INLINE void ComputeScreenSpaceMinMaxAABBAndMinZ
		(
			const size_t cameraIndex,
			culling::EntityBlock* const entityBlock,
			const size_t entityIndex
		);

		void DoPreCull
		(
			const size_t cameraIndex,
			culling::EntityBlock* const entityBlock
		);

	public:

		PreCulling(EveryCulling* frotbiteCullingSystem);
		void CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount) override;
		const char* GetCullingModuleName() const override;
	};
}


