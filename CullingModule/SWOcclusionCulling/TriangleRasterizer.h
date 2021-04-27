#pragma once

#include "../../FrotbiteCullingSystemCore.h"

#include <Vector2.h>
#include <Matrix4x4.h>

#include "SWFrameBuffer.h"
#include "../../DataStructure/AABB.h"

namespace culling
{
	class TriangleRasterizer
	{

	public:
		/// <summary>
		/// Cache-friendly
		/// </summary>
		struct alignas(64) Triangle
		{
			math::Vector<2, int> Point1;
			math::Vector<2, int> Point2;
			math::Vector<2, int> Point3;
		};

	private:

		math::Matrix4x4 mViewProjectionMatrix;
		/// <summary>
		/// this looks weird
		/// but for performance, we need this local variable
		/// </summary>
		static thread_local Triangle mAABBTriangles[12];

	public:

		static void StoreTrianglesFromAABB(const culling::AABB& aabb);

		FORCE_INLINE static int IsPointAtPositiveSideOfEdge(int edgePoint1X, int edgePoint1Y, int edgePoint2X, int edgePoint2Y, int pX, int pY)
		{
			return (edgePoint1X - pX) * (edgePoint2Y - pY) - (edgePoint2X - pX) * (edgePoint1Y - pY);
		}
		FORCE_INLINE static int IsPointAtPositiveSideOfEdge(const math::Vector<2, int>& edgePoint1, const math::Vector<2, int>& edgePoint2, const math::Vector<2, int>& p)
		{
			return (edgePoint1.x - p.x) * (edgePoint2.y - p.y) - (edgePoint2.x - p.x) * (edgePoint1.y - p.y);
		}

		

		static void FillTriangle(const SWFrameBuffer& frameBuffer, const Triangle& triangle)
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
					int p1 = IsPointAtPositiveSideOfEdge(triangle.Point1.x, triangle.Point1.y, triangle.Point2.x, triangle.Point2.y, x, y);
					int p2 = IsPointAtPositiveSideOfEdge(triangle.Point2.x, triangle.Point2.y, triangle.Point3.x, triangle.Point3.y, x, y);
					int p3 = IsPointAtPositiveSideOfEdge(triangle.Point3.x, triangle.Point3.y, triangle.Point1.x, triangle.Point1.y, x, y);
				
					if (p1 >= 0 && p2 >= 0 && p3 >= 0)
					{
						frameBuffer.mPixels[x + y * frameBuffer.mWidth] = 1.0f;
					}
				}
			}

		}
	};
}

