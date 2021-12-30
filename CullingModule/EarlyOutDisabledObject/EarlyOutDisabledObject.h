#pragma once

#include "../../EveryCullingCore.h"

#include "../CullingModule.h"

namespace culling
{
	class EarlyOutDisabledObject : public CullingModule
	{
	private:

		void DoEarlyOutDisabledObject
		(
			const size_t cameraIndex,
			culling::EntityBlock* const entityBlock
		);

	public:

		EarlyOutDisabledObject(EveryCulling* frotbiteCullingSystem);
		void CullBlockEntityJob(const size_t cameraIndex) override;
		const char* GetCullingModuleName() const override;
	};
}


