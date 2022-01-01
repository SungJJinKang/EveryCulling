#pragma once

#include "../../EveryCullingCore.h"

#include "../CullingModule.h"

namespace culling
{
	class PreCulling : public CullingModule
	{
	private:

		void DoPreCull
		(
			const size_t cameraIndex,
			culling::EntityBlock* const entityBlock
		);

	public:

		PreCulling(EveryCulling* frotbiteCullingSystem);
		void CullBlockEntityJob(const size_t cameraIndex) override;
		const char* GetCullingModuleName() const override;
	};
}


