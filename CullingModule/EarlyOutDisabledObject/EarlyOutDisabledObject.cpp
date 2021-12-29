#include "EarlyOutDisabledObject.h"

void culling::EarlyOutDisabledObject::DoEarlyOutDisabledObject
(
	const size_t cameraIndex,
	culling::EntityBlock* const entityBlock
)
{
	for(size_t entityIndex = 0 ; entityIndex < entityBlock->mCurrentEntityCount ; entityIndex++)
	{
		if(entityBlock->GetIsObjectEnabled(entityIndex) == false)
		{
			entityBlock->SetCulled(entityIndex, cameraIndex);
		}
	}
}

culling::EarlyOutDisabledObject::EarlyOutDisabledObject(EveryCulling* frotbiteCullingSystem)
	: CullingModule(frotbiteCullingSystem)
{
}

void culling::EarlyOutDisabledObject::CullBlockEntityJob(const size_t cameraIndex)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);;

		if (nextEntityBlock != nullptr)
		{
			DoEarlyOutDisabledObject(cameraIndex, nextEntityBlock);
		}
		else
		{
			break;
		}
	}

}
