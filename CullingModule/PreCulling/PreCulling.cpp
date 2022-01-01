#include "PreCulling.h"

void culling::PreCulling::DoPreCull
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
		else
		{
			entityBlock->UpdateBoundingSphereRadius(entityIndex);
		}		
	}
}

culling::PreCulling::PreCulling(EveryCulling* frotbiteCullingSystem)
	: CullingModule(frotbiteCullingSystem)
{
}

void culling::PreCulling::CullBlockEntityJob(const size_t cameraIndex)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);;

		if (nextEntityBlock != nullptr)
		{
			DoPreCull(cameraIndex, nextEntityBlock);
		}
		else
		{
			break;
		}
	}

}

const char* culling::PreCulling::GetCullingModuleName() const
{
	return "PreCulling";
}
