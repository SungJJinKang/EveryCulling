#pragma once

#include <vector>

namespace culling
{
	struct EntityBlock;
	struct EntityGridCell
	{
		/// <summary>
		/// AllocatedEntityCountInBlocks[1] -> Stored Entity Count in mEntityBlocks[1]
		/// if Entity is removed, swap removed entity block with last valid block and pop last block 
		/// 
		/// pointer should be set from mEntityBlockPool
		/// 
		/// 
		/// Why mEntityBlocks is array of EntityBlock pointer??
		/// In game, when early created entity can be destroyed earlier and a EntityBlock is empty, 
		/// the block should be return to BlockPool.
		/// So we use array of pointers
		/// </summary>
		std::vector<EntityBlock*> mEntityBlocks;
		
	};
}
