#pragma once

#include "../../EveryCullingCore.h"

#include <array>
#include <vector>
#include <atomic>

#include "../../DataType/Math/Vector.h"

#define OCCLUDER_LIST_POOL_SIZE 50

namespace culling
{
	struct EntityBlock;

	struct OccluderData
	{
		EntityBlock* mEntityBlock;
		size_t mEntityIndexInEntityBlock;
	};

	class OccluderListManager
	{
	private:

		std::atomic<size_t> mOccluderCount;
		//char padding[64];
		std::array<OccluderData, OCCLUDER_LIST_POOL_SIZE> mOccluderList;

	public:

		OccluderListManager();
		void AddOccluder(EntityBlock* const entityBlock, const size_t entityIndexInEntityBlock);

		std::vector<OccluderData> GetSortedOccluderList(const culling::Vec3& cameraWorldPos) const;

		void ResetOccluderList();
	};
}

