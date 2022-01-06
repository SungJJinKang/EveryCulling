#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	namespace depthUtility
	{
		FORCE_INLINE extern void ComputeDepthPlane
		(
			const float vertexPoint1X,
			const float vertexPoint1Y,
			const float vertexPoint1Z,
			const float vertexPoint2X,
			const float vertexPoint2Y,
			const float vertexPoint2Z,
			const float vertexPoint3X,
			const float vertexPoint3Y,
			const float vertexPoint3Z,
			float& outZPixelDx,
			float& outZPixelDy
		)
		{
			// Point1, 2, 3 should be counter clock wise!!!

			const float x2 = (vertexPoint3X - vertexPoint1X);
			const float x1 = (vertexPoint2X - vertexPoint1X);
			const float y1 = (vertexPoint2Y - vertexPoint1Y);
			const float y2 = (vertexPoint3Y - vertexPoint1Y);
			const float z1 = (vertexPoint2Z - vertexPoint1Z);
			const float z2 = (vertexPoint3Z - vertexPoint1Z);
			const float d = 1.0f / ((x1 * y2) - (y1 * x2));
			outZPixelDx = ((z1 * y2) - (y1 * z2)) * d;
			outZPixelDy = ((x1 * z2) - (z1 * x2)) * d;
		}
	}
}
;

