#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	namespace depthUtility
	{
		FORCE_INLINE extern void ComputeDepthPlane
		(
			const culling::M256F* const pVtxX, // TriangleVertex1X, TriangleVertex2X, TriangleVertex3X
			const culling::M256F* const pVtxY,
			const culling::M256F* const pVtxZ,
			culling::M256F& outZPixelDx,
			culling::M256F& outZPixelDy
		)
		{
			const culling::M256F x2 = culling::M256F_SUB(pVtxX[2], pVtxX[0]);
			const culling::M256F x1 = culling::M256F_SUB(pVtxX[1], pVtxX[0]);
			const culling::M256F y1 = culling::M256F_SUB(pVtxY[1], pVtxY[0]);
			const culling::M256F y2 = culling::M256F_SUB(pVtxY[2], pVtxY[0]);
			const culling::M256F z1 = culling::M256F_SUB(pVtxZ[1], pVtxZ[0]);
			const culling::M256F z2 = culling::M256F_SUB(pVtxZ[2], pVtxZ[0]);
			const culling::M256F d = culling::M256F_DIV(_mm256_set1_ps(1.0f), culling::M256F_SUB(culling::M256F_MUL(x1, y2), culling::M256F_MUL(y1, x2)));
			outZPixelDx = culling::M256F_MUL(culling::M256F_SUB(culling::M256F_MUL(z1, y2), culling::M256F_MUL(y1, z2)), d);
			outZPixelDy = culling::M256F_MUL(culling::M256F_SUB(culling::M256F_MUL(x1, z2), culling::M256F_MUL(z1, x2)), d);
		}
	}
}
;

