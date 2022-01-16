#include "OccluderListManager.h"

#include <algorithm>

#include "Graphics/Acceleration/LinearData_ViewFrustumCulling/DataType/EntityBlock.h"

culling::OccluderListManager::OccluderListManager()
{
	ResetOccluderList();
}

void culling::OccluderListManager::AddOccluder(EntityBlock* const entityBlock, const size_t entityIndexInEntityBlock)
{
	const size_t occluderIndex = mOccluderCount++;

	if(occluderIndex < OCCLUDER_LIST_POOL_SIZE)
	{
		mOccluderList[occluderIndex].mEntityBlock = entityBlock;
		mOccluderList[occluderIndex].mEntityIndexInEntityBlock = entityIndexInEntityBlock;
	}
}

std::vector<culling::OccluderData> culling::OccluderListManager::GetSortedOccluderList() const
{
	const size_t occluderCount = mOccluderCount;

	std::vector<culling::OccluderData> occluderList;
	occluderList.reserve(MIN(occluderCount, OCCLUDER_LIST_POOL_SIZE));

	for (size_t occluderIndex = 0; occluderIndex < MIN(occluderCount, OCCLUDER_LIST_POOL_SIZE) ; occluderIndex++)
	{
		occluderList.emplace_back(mOccluderList[occluderIndex]);
	}

	std::sort
	(
		occluderList.begin(),
		occluderList.end(),
		[](const culling::OccluderData& left, const culling::OccluderData& right)
		{
			return left.mEntityBlock->GetFrontToBackSortingOrder(left.mEntityIndexInEntityBlock) < right.mEntityBlock->GetFrontToBackSortingOrder(right.mEntityIndexInEntityBlock);
		}
	);


	return occluderList;
}

void culling::OccluderListManager::ResetOccluderList()
{
	mOccluderCount = 0;
}
