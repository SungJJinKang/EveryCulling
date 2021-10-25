#pragma once

#include <bitset>

#include "../../EveryCullingCore.h"
#include <SIMD_Core.h>

namespace culling
{
	/// <summary>
	/// 32byte
	/// 
	/// </summary>
	struct CoverageMask
	{
		/// <summary>
		/// For using SIMD, variable should be aligned to 16 byte(AVX1) or 32 byte(AVX2)
		/// </summary>
#if TILE_WIDTH == 16
		static_assert(false, "You need to check alignment");
		M128I mBits; // 16byte
#elif TILE_WIDTH == 32
		M256I mBits; // 32byte
#endif	 

	};

	static_assert(sizeof(CoverageMask) == 32);

	/// <summary>
	/// 
	/// False sharing Safety CoverageMask
	/// 
	/// How this type is uesd.
	/// 
	/// In Masked SW Occlusion culling,
	/// Multiple thread works on tile(32 * 8)
	/// Size of CoverageMask is 256bit(32byte) 
	/// Then Each Thread should works on different cache line ( False Sharing!!! )
	/// 
	///     64   ......   128   ......   192   .......  256    ......   320
	///     CoverageMask1.CoverageMask2..CoverageMask3..CoverageMask4...
	///	CachceLine1----------------->CachceLine2------------------->
	///     Thread1--------------------->Thread2----------------------->
	/// 
	/// </summary>
	struct CoverageMaskPair
	{
		alignas(64) CoverageMask CoverageMask[2];
	};
	
}
