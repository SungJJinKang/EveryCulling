#include "EntityBlock.h"

void culling::EntityBlock::ClearEntityBlock()
{
	mCurrentEntityCount = 0;

	std::memset(mIsOccluder, 0x00, ENTITY_COUNT_IN_ENTITY_BLOCK);

	for(size_t entityIndex = 0 ; entityIndex < ENTITY_COUNT_IN_ENTITY_BLOCK ; entityIndex++)
	{
		mDesiredMaxDrawDistance[entityIndex] = (float)DEFAULT_DESIRED_MAX_DRAW_DISTANCE;
;	}
	
}
