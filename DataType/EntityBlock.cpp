#include "EntityBlock.h"

void culling::EntityBlock::ClearEntityBlock()
{
	mCurrentEntityCount = 0;
	
	for(size_t entityIndex = 0 ; entityIndex < ENTITY_COUNT_IN_ENTITY_BLOCK ; entityIndex++)
	{
		mDesiredMaxDrawDistance[entityIndex] = (float)DEFAULT_DESIRED_MAX_DRAW_DISTANCE;
;	}
	
}
