#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	namespace rasterizerHelper
	{
		extern void GetMiddlePointOfTriangle
		(
			const culling::M256F& TriPointA_X,
			const culling::M256F& TriPointA_Y,
			const culling::M256F& TriPointA_Z,

			const culling::M256F& TriPointB_X,
			const culling::M256F& TriPointB_Y,
			const culling::M256F& TriPointB_Z,

			const culling::M256F& TriPointC_X,
			const culling::M256F& TriPointC_Y,
			const culling::M256F& TriPointC_Z,

			culling::M256F& outLeftMiddlePointC_X,
			culling::M256F& outLeftMiddlePointC_Y,
			culling::M256F& outLeftMiddlePointC_Z,

			culling::M256F& outRightMiddlePointC_X,
			culling::M256F& outRightMiddlePointC_Y,
			culling::M256F& outRightMiddlePointC_Z
		);
	};
}
