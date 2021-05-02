#pragma once

#include <bitset>

#include "../../FrotbiteCullingSystemCore.h"
#include "../../SIMD_Core.h"
namespace culling
{
	/// <summary>
	/// 16byte
	/// </summary>
	struct CoverageMask
	{
		/// <summary>
		/// For using SIMD, variable should be aligned to 16 byte(AVX1) or 32 byte(AVX2)
		/// </summary>
#if TILE_WIDTH == 16
		M128I mBits;
#elif TILE_WIDTH == 32
		M256I mBits;
#endif	 

	};
	
}