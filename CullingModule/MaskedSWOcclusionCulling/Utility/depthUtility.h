#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	namespace depthUtility
	{
		FORCE_INLINE extern void ComputeDepthPlane
		(
			const culling::M256F& vertexPoint1X,
			const culling::M256F& vertexPoint1Y,
			const culling::M256F& vertexPoint1Z,
			const culling::M256F& vertexPoint2X,
			const culling::M256F& vertexPoint2Y,
			const culling::M256F& vertexPoint2Z,
			const culling::M256F& vertexPoint3X,
			const culling::M256F& vertexPoint3Y,
			const culling::M256F& vertexPoint3Z,
			culling::M256F& outZPixelDx,
			culling::M256F& outZPixelDy
		)
		{
			//Point2, 3 should be located at same y

#ifdef DEBUG_CULLING
			const culling::M256F EQ_23Y = _mm256_cmp_ps(vertexPoint2Y, vertexPoint3Y, _CMP_EQ_OQ);

			assert(_mm256_testc_si256(*reinterpret_cast<const culling::M256I*>(&EQ_23Y), _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)) == 1);
#endif

			const culling::M256F x2 = culling::M256F_SUB(vertexPoint3X, vertexPoint1X);
			const culling::M256F x1 = culling::M256F_SUB(vertexPoint2X, vertexPoint1X);
			const culling::M256F y1 = culling::M256F_SUB(vertexPoint2Y, vertexPoint1Y);
			const culling::M256F y2 = culling::M256F_SUB(vertexPoint3Y, vertexPoint1Y);
			const culling::M256F z1 = culling::M256F_SUB(vertexPoint2Z, vertexPoint1Z);
			const culling::M256F z2 = culling::M256F_SUB(vertexPoint3Z, vertexPoint1Z);
			const culling::M256F d = culling::M256F_DIV(_mm256_set1_ps(1.0f), _mm256_fmsub_ps(x1, y2, culling::M256F_MUL(y1, x2)));
			outZPixelDx = culling::M256F_MUL(_mm256_fmsub_ps(z1, y2, culling::M256F_MUL(y1, z2)), d);
			outZPixelDy = culling::M256F_MUL(_mm256_fmsub_ps(x1, z2, culling::M256F_MUL(z1, x2)), d);
		}
	}
}
;

