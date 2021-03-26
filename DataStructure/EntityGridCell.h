#pragma once

#include "../LinearViewFrustumCulling_Core.h"

namespace doom
{
	namespace graphics
	{
		struct EntityBlock;
		struct EntityGridCell
		{
			/// <summary>
			/// EntityCountInBlocks[1] -> Stored Entity Count in mEntityBlocks[1]
			/// if Entity is removed, swap removed entity block with last valid block and pop last block 
			/// 
			/// pointer should be set from EntityBlockPool
			/// 
			/// Why mEntityBlocks is array of EntityBlock pointer??
			/// In game, when early created entity can be destroied earlier and A entityBlock is empty, 
			/// the block should be return to BlockPool.
			/// So we use array of pointers
			/// </summary>
			EntityBlock* mEntityBlocks[MAX_ENTITY_BLOCK_COUNT];
			size_t EntityCountInBlocks[MAX_ENTITY_BLOCK_COUNT];
			unsigned long int TotalEntityCount;
		};
	}
}