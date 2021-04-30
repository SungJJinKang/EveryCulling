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
		M128I mBits;
	};
	
}