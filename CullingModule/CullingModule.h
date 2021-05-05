#pragma once

#include "../FrotbiteCullingSystemCore.h"

#include "../DataType/EntityBlock.h"

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


	};

}