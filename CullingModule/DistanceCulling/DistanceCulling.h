#pragma once

#include "../CullingModule.h"

namespace culling
{
	class DistanceCulling : CullingModule
	{
	private:

	public:

		//TODO : EntityBlock의 mPositions 이거 그냥 사용하면된다.
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex) final
		{

		}
	};
}

