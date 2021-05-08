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
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t blockIndex, size_t cameraIndex) = 0;
	
	public:


	};

}