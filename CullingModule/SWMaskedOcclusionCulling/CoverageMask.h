#pragma once

#include <bitset>

#include "../../FrotbiteCullingSystemCore.h"

namespace culling
{
	using coverageMaskInternalType = typename std::bitset<TILE_WIDTH* TILE_HEIGHT>;
	struct CoverageMask
	{
		alignas(16) coverageMaskInternalType mBits;
	};
	
}