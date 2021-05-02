#pragma once

/*
#include "../../../FrotbiteCullingSystemCore.h"

#include <Vector2.h>
#include <Matrix4x4.h>

#include "../SWDepthBuffer.h"
#include "../../../DataStructure/AABB.h"
#include "../Triangle.h"

namespace culling
{
	class TriangleRasterizer
	{


	private:

		math::Matrix4x4 mViewProjectionMatrix;

	public:

		static void StoreTrianglesFromAABB(const culling::AABB& aabb);

		FORCE_INLINE static int PerpDot(int edgePoint1X, int edgePoint1Y, int edgePoint2X, int edgePoint2Y)
		{
			return edgePoint1X * edgePoint2Y - edgePoint2X * edgePoint1Y;
		}
		FORCE_INLINE static int PerpDot(const math::Vector<2, int>& edgePoint1, const math::Vector<2, int>& edgePoint2)
		{
			return edgePoint1.x * edgePoint2.y - edgePoint2.x * edgePoint1.y;
		}
		/*
		FORCE_INLINE static int PerpDot(int edgePoint1X, int edgePoint1Y, int edgePoint2X, int edgePoint2Y, int pX, int pY)
		{
			return (edgePoint1X - pX) * (edgePoint2Y - pY) - (edgePoint2X - pX) * (edgePoint1Y - pY);
		}
		FORCE_INLINE static int PerpDot(const math::Vector<2, int>& edgePoint1, const math::Vector<2, int>& edgePoint2, const math::Vector<2, int>& p)
		{
			return (edgePoint1.x - p.x) * (edgePoint2.y - p.y) - (edgePoint2.x - p.x) * (edgePoint1.y - p.y);
		}
		
		

		static void FillTriangle(const SWDepthBuffer& frameBuffer, const TwoDTriangle& triangle)
		{
			int minX = math::Min(math::Min(triangle.Point1.x, triangle.Point2.x), triangle.Point3.x);
			int minY = math::Min(math::Min(triangle.Point1.y, triangle.Point2.y), triangle.Point3.y);
			int maxX = math::Max(math::Max(triangle.Point1.x, triangle.Point2.x), triangle.Point3.x);
			int maxY = math::Max(math::Max(triangle.Point1.y, triangle.Point2.y), triangle.Point3.y);

			minX = math::clamp(minX, 0, frameBuffer.mWidth);
			maxX = math::clamp(minX, 0, frameBuffer.mWidth);
			minY = math::clamp(minX, 0, frameBuffer.mHeight);
			maxY = math::clamp(minX, 0, frameBuffer.mHeight);

			for (int y = minY; y < maxY; y++)
			{
				for (int x = minX; y < maxX; x++)
				{
					//using barycentric algorithm, http://www.sunshine2k.de/coding/java/PointInTriangle/PointInTriangle.html#cramer
					auto s = PerpDot(x - triangle.Point1.x, triangle.Point2.x, y - triangle.Point1.y, triangle.Point2.y) / PerpDot(triangle.Point1, triangle.Point2);
					auto t = PerpDot(triangle.Point1.x, triangle.Point2.x, y - triangle.Point1.y, triangle.Point2.y) / PerpDot(triangle.Point1, triangle.Point2);

				
				}
			}

		}
	};
}

*/