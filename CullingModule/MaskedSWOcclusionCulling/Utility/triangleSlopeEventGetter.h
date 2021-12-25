#pragma once

#include "../../../EveryCullingCore.h"

#include "../../../DataType/Math/Vector.h"
namespace culling
{
	namespace triangleSlopeHelper
	{
		extern void GatherBottomFlatTriangleSlopeEvent
		(
			const size_t triangleCount,
			const culling::Vec2& TileLeftBottomOriginPoint,
			culling::M256I* const leftFaceEvent,
			culling::M256I* const rightFaceEvent,
			const culling::M256F& TriPointA_X,
			const culling::M256F& TriPointA_Y,

			const culling::M256F& TriPointB_X,
			const culling::M256F& TriPointB_Y,

			const culling::M256F& TriPointC_X,
			const culling::M256F& TriPointC_Y

		);

		extern void GatherTopFlatTriangleSlopeEvent
		(
			const size_t triangleCount,
			const culling::Vec2& TileLeftBottomOriginPoint,
			culling::M256I* const leftFaceEvent,
			culling::M256I* const rightFaceEvent,
			const culling::M256F& TriPointA_X,
			const culling::M256F& TriPointA_Y,

			const culling::M256F& TriPointB_X,
			const culling::M256F& TriPointB_Y,

			const culling::M256F& TriPointC_X,
			const culling::M256F& TriPointC_Y

		);
	};
}
