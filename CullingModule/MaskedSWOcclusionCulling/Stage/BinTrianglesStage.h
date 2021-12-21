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

		std::atomic<bool> IsWorkingByAThread;

	
		/// <summary>
		/// Project Vertex To ScreenSpace
		/// X, Y, Z, W -> X/W , Y/W, Z/W, W/1
		/// </summary>
		/// <param name="outClipVertexX"></param>
		/// <param name="outClipVertexY"></param>
		/// <param name="outClipVertexZ"></param>
		/// <param name="outClipVertexW"></param>
		FORCE_INLINE void ConvertClipSpaceToNDCSpace
		(
			culling::M256F* outClipVertexX, 
			culling::M256F* outClipVertexY, 
			culling::M256F* outClipVertexZ, 
			const culling::M256F* oneDividedByW, 
			std::uint32_t& triangleCullMask
		);

		/// <summary>
		/// frustum culling in clip space
		/// </summary>
		/// <param name="clipspaceVertexX"></param>
		/// <param name="clipspaceVertexY"></param>
		/// <param name="triangleCullMask"></param>
		FORCE_INLINE void FrustumCulling
		(
			const culling::M256F* const clipspaceVertexX,
			const culling::M256F* const clipspaceVertexY,
			const culling::M256F* const clipspaceVertexZ,
			const culling::M256F* const clipspaceVertexW,
			std::uint32_t& triangleCullMask
		);

		/// <summary>
		/// Convert NDC Space X,Y To Screen Space X,Y according to Graphics API
		/// And Snap Screen Space X,Y To Pixel Coordinate in Buffer
		/// 
		/// Pixel Coordinate have integer value
		/// </summary>
		/// <param name="ndcSpaceVertexX"></param>
		/// <param name="ndcSpaceVertexY"></param>
		/// <param name="triangleCullMask"></param>
		/// <param name="outScreenPixelSpaceX"></param>
		/// <param name="outScreenPixelSpaceY"></param>
		FORCE_INLINE void ConvertNDCSpaceToScreenPixelSpace
		(
			const culling::M256F* ndcSpaceVertexX, const culling::M256F* ndcSpaceVertexY,
			culling::M256F* outScreenPixelSpaceX, culling::M256F* outScreenPixelSpaceY, std::uint32_t& triangleCullMask
		);

		/// <summary>
		/// Convert 4 TriangSles's Model Vertex to ClipSpace
		/// </summary>
		/// <param name="vertX">three culling::M256F -> 12 floating-point value</param>
		/// <param name="vertY">three culling::M256F -> 12 floating-point value</param>
		/// <param name="vertW">"W" !!!!!!!!! three culling::M256F -> 12 floating-point value</param>
		/// <param name="modelToClipspaceMatrix">column major 4x4 matrix</param>
		FORCE_INLINE void TransformVertexsToClipSpace
		(
			culling::M256F* outClipVertexX, 
			culling::M256F* outClipVertexY, 
			culling::M256F* outClipVertexZ,
			culling::M256F* outClipVertexW, 
			const float* const toClipspaceMatrix,
			std::uint32_t& triangleCullMask
		);

		/// <summary>
		/// BackFace Culling
		/// Result is stored in triangleCullMask
		/// </summary>
		/// <param name="screenPixelX"></param>
		/// <param name="screenPixelY"></param>
		/// <param name="triangleCullMask"></param>
		FORCE_INLINE void BackfaceCulling(const culling::M256F* screenPixelX, const culling::M256F* screenPixelY, std::uint32_t& triangleCullMask);

		
		
		/// <summary>
		/// Compute BoudingBox intersecting with triangles
		/// 
		/// if boudingBox intersect with multiple tiles,
		/// Min, Max coorridate of unioned bouding is returned.
		/// </summary>
		/// <param name="screenPixelX"></param>
		/// <param name="screenPixelY"></param>
		/// <param name="outBinBoundingBoxMinX">MinX of bounding box intersecting with 8 Triangles, Not Index. ex) 0, 32, 64, ..</param>
		/// <param name="outBinBoundingBoxMinY">MinY of bounding box intersecting with 8 Triangles, Not Index. ex) 0, 32, 64, ..</param>
		/// <param name="outBinBoundingBoxMaxX">MaxX of bounding box intersecting with 8 Triangles, Not Index. ex) 0, 32, 64, ..</param>
		/// <param name="outBinBoundingBoxMaxY">MaxY of bounding box intersecting with 8 Triangles, Not Index. ex) 0, 32, 64, ..</param>
		FORCE_INLINE void ComputeBinBoundingBox
		(
			const culling::M256F* screenPixelX,
			const culling::M256F* screenPixelY,
			culling::M256I& outBinBoundingBoxMinX,
			culling::M256I& outBinBoundingBoxMinY,
			culling::M256I& outBinBoundingBoxMaxX, 
			culling::M256I& outBinBoundingBoxMaxY
		);

		FORCE_INLINE void PassTrianglesToTileBin
		(
			const culling::M256F* screenPixelPosX,
			const culling::M256F* screenPixelPosY,
			const culling::M256F* ndcSpaceVertexZ,
			std::uint32_t& triangleCullMask,
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
			culling::M256F* outVerticesZ, 
			std::uint32_t& triangleCullMask
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
			const float* const vertices,
			const size_t verticeCount,
			const std::uint32_t* const vertexIndices,
			const size_t indiceCount, 
			const size_t vertexStrideByte, 
			const float* const modelToClipspaceMatrix
		);

		FORCE_INLINE void ConvertToPlatformDepth(culling::M256F* const depth);

	public:

		BinTrianglesStage(MaskedSWOcclusionCulling* mMOcclusionCulling);

		void ResetCullingModule() override;

		void CullBlockEntityJob(const size_t cameraIndex) override;
	};
}

