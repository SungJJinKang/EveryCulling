#pragma once

#include <Matrix4x4.h>

#include "../SWDepthBuffer.h"

namespace culling
{
	class DepthRasterizer
	{

	private:
		math::Matrix4x4 mViewProjectionMatrix;

	public:

		void SetViewProjectionMatrix(const math::Matrix4x4& viewProjectionMatrix);
		float GetDepthInTile(int tileMinX, int tileMinY);
		void DrawDepth(SWDepthBuffer* depthBuffer, void* meshTriangles, size_t meshTriangleCount);
	};
}

