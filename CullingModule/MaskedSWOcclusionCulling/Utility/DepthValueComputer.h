#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	namespace DepthValueComputer
	{
		enum class eDepthType
		{
			MinDepth,
			MaxDepth
		};

		extern void ComputeFlatBottomDepthValue
		(
			const size_t triangleCount,
			const eDepthType targetDepthType,
			culling::M256F* const subTileMaxValues,
			const std::uint32_t tileOriginX, // 32x8 tile
			const std::uint32_t tileOriginY, // 32x8 tile

			const culling::M256F& pVtx1X,
			const culling::M256F& pVtx1Y,
			const culling::M256F& pVtx1Z,

			const culling::M256F& pVtx2X,
			const culling::M256F& pVtx2Y,
			const culling::M256F& pVtx2Z,

			const culling::M256F& pVtx3X,
			const culling::M256F& pVtx3Y,
			const culling::M256F& pVtx3Z,

			const culling::M256I* const leftFaceEvent,
			const culling::M256I* const rightFaceEvent
		);

		extern void ComputeFlatTopDepthValue
		(
			const size_t triangleCount,
			const eDepthType targetDepthType,
			culling::M256F* const subTileMaxValues,
			const std::uint32_t tileOriginX, // 32x8 tile
			const std::uint32_t tileOriginY, // 32x8 tile

			const culling::M256F& pVtx1X,
			const culling::M256F& pVtx1Y,
			const culling::M256F& pVtx1Z,

			const culling::M256F& pVtx2X,
			const culling::M256F& pVtx2Y,
			const culling::M256F& pVtx2Z,

			const culling::M256F& pVtx3X,
			const culling::M256F& pVtx3Y,
			const culling::M256F& pVtx3Z,

			const culling::M256I* const leftFaceEvent,
			const culling::M256I* const rightFaceEvent
		);
		
		/*
		 *static void ComputeDepthValue
		(
			culling::M256F* const subTileMaxValues,
			const std::uint32_t tileOriginX, // 32x8 tile
			const std::uint32_t tileOriginY, // 32x8 tile
			const culling::M256F& pVtx1X,
			const culling::M256F& pVtx1Y,
			const culling::M256F& pVtx1Z,
			const culling::M256F& pVtx2X,
			const culling::M256F& pVtx2Y,
			const culling::M256F& pVtx2Z,
			const culling::M256F& pVtx3X,
			const culling::M256F& pVtx3Y,
			const culling::M256F& pVtx3Z
		);
		*/

	};
}


