#include "DistanceCulling.h"

#include <algorithm>

void culling::DistanceCulling::AddCullDistanceVolume(culling::CullDistanceVolume cullDistanceVolume)
{
	mCullDistanceVolumes.push_back(cullDistanceVolume);
	std::sort(mCullDistanceVolumes.begin(), mCullDistanceVolumes.end(),
		[](const culling::CullDistanceVolume& A, const culling::CullDistanceVolume& B)
		{
			return A.mSize < B.mSize;
		}
	);
}

void culling::DistanceCulling::DeleteCullDistanceVolume(size_t index)
{
	if (index >= mCullDistanceVolumes.size())
	{
		return;
	}

	mCullDistanceVolumes.erase(mCullDistanceVolumes.begin() + index);
}
