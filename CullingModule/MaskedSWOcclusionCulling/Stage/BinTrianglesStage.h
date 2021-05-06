#pragma once

#include "MaskedSWOcclusionCullingStage.h"

#include "../../../SIMD_Core.h"
#include "../../../DataType/Math/AABB.h"
#include "../../../DataType/Math/Triangle.h"

#include "../SWDepthBuffer.h"

namespace culling
{
	
	class BinTrianglesStage : public MaskedSWOcclusionCullingStage
	{
		friend class MaskedSWOcclusionCulling;

	private:

	
		/// <summary>
		/// Project Vertex To ScreenSpace
		/// X, Y, Z, W -> X/W , Y/W, Z/W, W/1
		/// </summary>
		/// <param name="outClipVertexX"></param>
		/// <param name="outClipVertexY"></param>
		/// <param name="outClipVertexZ"></param>
		/// <param name="outClipVertexW"></param>
		void ConverClipSpaceToNDCSpace(
			M256F* outClipVertexX, M256F* outClipVertexY, const M256F* oneDividedByW, unsigned int& triangleCullMask
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
		void ConvertNDCSpaceToScreenPixelSpace(
			const M256F* ndcSpaceVertexX, const M256F* ndcSpaceVertexY,
			M256F* outScreenPixelSpaceX, M256F* outScreenPixelSpaceY, unsigned int& triangleCullMask
		);

		/// <summary>
		/// Convert 4 TriangSles's Model Vertex to ClipSpace
		/// </summary>
		/// <param name="vertX">three M256F -> 12 floating-point value</param>
		/// <param name="vertY">three M256F -> 12 floating-point value</param>
		/// <param name="vertW">"W" !!!!!!!!! three M256F -> 12 floating-point value</param>
		/// <param name="modelToClipspaceMatrix">column major 4x4 matrix</param>
		void TransformVertexsToClipSpace(
			M256F* outClipVertexX, M256F* outClipVertexY, M256F* outClipVertexW,
			const float* toClipspaceMatrix, unsigned int& triangleCullMask
		);

		/// <summary>
		/// BackFace Culling
		/// Result is stored in triangleCullMask
		/// </summary>
		/// <param name="screenPixelX"></param>
		/// <param name="screenPixelY"></param>
		/// <param name="triangleCullMask"></param>
		void CullBackfaces(const M256F* screenPixelX, const M256F* screenPixelY, unsigned int& triangleCullMask);

		
		
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
		void ComputeBinBoundingBox(const M256F* screenPixelX, const M256F* screenPixelY, M256I& outBinBoundingBoxMinX, M256I& outBinBoundingBoxMinY, M256I& outBinBoundingBoxMaxX, M256I& outBinBoundingBoxMaxY);
		void PassTrianglesToTileBin(const M256F* screenPixelX, const M256F* screenPixelY, unsigned int& triangleCullMask, TriangleList& tileBin, const M256F& outBinBoundingBoxMinX, const M256F& outBinBoundingBoxMinY, const M256F& outBinBoundingBoxMaxX, const M256F& outBinBoundingBoxMaxY);

		inline float GetAreaOfTriangle(const TwoDTriangle& triangle)
		{

		}

		inline float GetAreaOfAAABB(const AABB& worldAABB)
		{

		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Binning Triangles to Tile(Sub Tile)
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////




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
		void GatherVertex(
			const float* vertices, const unsigned int* vertexIndices, const size_t indiceCount, const size_t currentIndiceIndex,
			const size_t vertexStrideByte,

			const size_t fetchTriangleCount,
			M256F* outVerticesX, M256F* outVerticesY, unsigned int& triangleCullMask
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
		void BinTriangles(const float* vertices, const unsigned int* vertexIndices, size_t indiceCount, size_t vertexStrideByte, float* modelToClipspaceMatrix);

	public:

		BinTrianglesStage(MaskedSWOcclusionCulling& mMOcclusionCulling);

	};
}

