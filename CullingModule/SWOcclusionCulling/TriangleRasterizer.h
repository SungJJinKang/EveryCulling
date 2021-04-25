#pragma once

#include "../../FrotbiteCullingSystemCore.h"

#include <Vector2.h>

#include "SWFrameBuffer.h"

namespace culling
{
	class TriangleRasterizer
	{
	private:

	public:
		FORCE_INLINE static int IsPointAtPositiveSideOfEdge(const math::Vector2& edgePoint1, const math::Vector2& edgePoint2, const math::Vector2& p)
		{
			return (edgePoint1.x - p.x) * (edgePoint2.y - p.y) - (edgePoint2.x - p.x) * (edgePoint1.y - p.y);
		}

		/// <summary>
		/// Cache-friendly
		/// </summary>
		struct alignas(64) Triangle
		{
			math::Vector2 Point1;
			math::Vector2 Point2;
			math::Vector2 Point3;
		};

		static void FillTriangle(const Triangle& triangle)
		{

		}
	};
}

