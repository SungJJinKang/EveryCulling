#pragma once

#include "../FrotbiteCullingSystemCore.h"

#include "../DataStructure/EntityBlock.h"

namespace culling
{
	class FrotbiteCullingSystem;
	struct EntityBlock;

	class CullingModule
	{
	protected:

		FrotbiteCullingSystem* mFrotbiteCullingSystem;

		unsigned int mCameraCount;

		CullingModule(FrotbiteCullingSystem* frotbiteCullingSystem)
			:mFrotbiteCullingSystem{ frotbiteCullingSystem }, mCameraCount{ 0 }
		{

		}
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, unsigned int entityCountInBlock, unsigned int blockIndex, unsigned int cameraIndex) = 0;
	public:

		//static virtual void CullEntityBlockJob = 0;
		//virtual void ResetCullState() = 0;
	};

}