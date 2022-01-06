#pragma once

#include "MaskedSWOcclusionCullingStage.h"

#include "../../../DataType/Math/AABB.h"
#include "../../../DataType/Math/Triangle.h"

#include "../SWDepthBuffer.h"

namespace culling
{
	
	class BinTrianglesStage : public MaskedSWOcclusionCullingStage
	{
	private:


		/// <summary>
		/// frustum culling in clip space
		/// </summary>
		/// <param name="clipspaceVertexX"></param>
		/// <param name="clipspaceVertexY"></param>
		/// <param name="triangleCullMask"></param>
		FORCE_INLINE void Clipping
		(
			const culling::M256F* const clipspaceVertexX,
			const culling::M256F* const clipspaceVertexY,
			const culling::M256F* const clipspaceVertexZ,
			const culling::M256F* const clipspaceVertexW,
			std::uint32_t& triangleCullMask
		);

		FORCE_INLINE culling::M256F ComputePositiveWMask
		(
			const culling::M256F* const clipspaceVertexW
		);
		
		
		/// <summary>
		/// BackFace Culling
		/// Result is stored in triangleCullMask
		/// </summary>
		/// <param name="screenPixelX"></param>
		/// <param name="screenPixelY"></param>
		/// <param name="ndcSpaceVertexZ"></param>
		/// <param name="triangleCullMask"></param>
		FORCE_INLINE void BackfaceCulling
		(
			culling::M256F* const screenPixelX, 
			culling::M256F* const screenPixelY, 
			std::uint32_t& triangleCullMask
		);

		

		FORCE_INLINE void PassTrianglesToTileBin
		(
			const size_t binnedTriangleIndex,
			const culling::M256F& pointAScreenPixelPosX,
			const culling::M256F& pointAScreenPixelPosY,
			const culling::M256F& pointANdcSpaceVertexZ,

			const culling::M256F& pointBScreenPixelPosX,
			const culling::M256F& pointBScreenPixelPosY,
			const culling::M256F& pointBNdcSpaceVertexZ,

			const culling::M256F& pointCScreenPixelPosX,
			const culling::M256F& pointCScreenPixelPosY,
			const culling::M256F& pointCNdcSpaceVertexZ,

			const std::uint32_t& triangleCullMask,
			const size_t triangleCountPerLoop,
			const culling::M256I& outBinBoundingBoxMinX,
			const culling::M256I& outBinBoundingBoxMinY,
			const culling::M256I& outBinBoundingBoxMaxX,
			const culling::M256I& outBinBoundingBoxMaxY
		);
		

		/// <summary>
		/// Gather Vertex from VertexList with IndiceList
		/// 
		/// first float of outVerticesX[0] have Triangle1's Point1 X
		/// first float of outVerticesX[1] have Triangle1's Point2 X
		/// first float of outVerticesX[2] have Triangle1's Point3 X
		/// 
		/// second float of outVerticesX[0] have Triangle2's Point1 X
		/// second float of outVerticesX[1] have Triangle2's Point2 X
		/// second float of outVerticesX[2] have Triangle2's Point3 X
		/// </summary>
		/// <param name="vertices"></param>
		/// <param name="vertexIndices"></param>
		/// <param name="indiceCount"></param>
		/// <param name="currentIndiceIndex"></param>
		/// <param name="vertexStrideByte">
		/// how far next vertex point is from current vertex point 
		/// ex) 
		/// 1.0f(Point1_X), 2.0f(Point2_Y), 0.0f(Point3_Z), 3.0f(Normal_X), 3.0f(Normal_Y), 3.0f(Normal_Z),  1.0f(Point1_X), 2.0f(Point2_Y), 0.0f(Point3_Z)
		/// --> vertexStride is 6 * 4(float)
		/// </param>
		/// <param name="fetchTriangleCount"></param>
		/// <param name="outVerticesX"></param>
		/// <param name="outVerticesY"></param>
		/// <param name="triangleCullMask"></param>
		FORCE_INLINE void GatherVertices
		(
			const float* const vertices,
			const size_t verticeCount,
			const std::uint32_t* const vertexIndices,
			const size_t indiceCount, 
			const size_t currentIndiceIndex, 
			const size_t vertexStrideByte, 
			const size_t fetchTriangleCount,
			culling::M256F* outVerticesX, 
			culling::M256F* outVerticesY, 
			culling::M256F* outVerticesZ
		);
		
		/// <summary>
		/// Bin Triangles
		/// </summary>
		/// <param name="vertices"></param>
		/// <param name="vertexIndices"></param>
		/// <param name="indiceCount"></param>
		/// <param name="vertexStrideByte">
		/// how far next vertex point is from current vertex point 
		/// ex) 
		/// 1.0f(Point1_X), 2.0f(Point2_Y), 0.0f(Point3_Z), 3.0f(Normal_X), 3.0f(Normal_Y), 3.0f(Normal_Z),  1.0f(Point1_X), 2.0f(Point2_Y), 0.0f(Point3_Z)
		/// --> vertexStride is 6 * 4(float)
		/// </param>
		/// <param name="modelToClipspaceMatrix"></param>
		FORCE_INLINE void BinTriangles
		(
			const size_t binnedTriangleIndex,
			const float* const vertices,
			const size_t verticeCount,
			const std::uint32_t* const vertexIndices,
			const size_t indiceCount, 
			const size_t vertexStrideByte, 
			const float* const modelToClipspaceMatrix
		);

		void ConvertToPlatformDepth(culling::M256F* const depth);

		//void BinTriangleThreadJob(const size_t cameraIndex);

		/// <summary>
		/// BinTriangle based on front to back ordering
		/// </summary>
		/// <param name="cameraIndex"></param>
		void BinTriangleThreadJobByObjectOrder(const size_t cameraIndex);

	public:

		BinTrianglesStage(MaskedSWOcclusionCulling* mMOcclusionCulling);

		void ResetCullingModule() override;

		void CullBlockEntityJob(const size_t cameraIndex) override;
		const char* GetCullingModuleName() const override;
	};
}

