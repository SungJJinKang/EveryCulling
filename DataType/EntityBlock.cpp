#include "EntityBlock.h"

void culling::EntityBlock::ClearEntityBlock()
{
	mCurrentEntityCount = 0;
	
	for(size_t entityIndex = 0 ; entityIndex < EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK ; entityIndex++)
	{
		mDesiredMaxDrawDistance[entityIndex] = (float)EVERYCULLING_DEFAULT_DESIRED_MAX_DRAW_DISTANCE;
	}
	
}
