#pragma once

#include "../CullingModule.h"

namespace culling
{
	class DistanceCulling : CullingModule
	{
	private:

	public:

		//TODO : EntityBlock�� mPositions �̰� �׳� ����ϸ�ȴ�.
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex) final
		{

		}
	};
}

