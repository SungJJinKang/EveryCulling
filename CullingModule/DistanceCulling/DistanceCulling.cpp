#include "DistanceCulling.h"

#include <algorithm>

void culling::DistanceCulling::AddCullDistanceVolume(culling::CullDistanceVolume cullDistanceVolume)
{
	this->mCullDistanceVolumes.push_back(cullDistanceVolume);
	std::sort(this->mCullDistanceVolumes.begin(), this->mCullDistanceVolumes.end(),
		[](const culling::CullDistanceVolume& A, const culling::CullDistanceVolume& B)
		{
			return A.mSize < B.mSize;
		}
	);
}

void culling::DistanceCulling::DeleteCullDistanceVolume(size_t index)
{
	if (index >= this->mCullDistanceVolumes.size())
	{
		return;
	}

	this->mCullDistanceVolumes.erase(this->mCullDistanceVolumes.begin() + index);
}
