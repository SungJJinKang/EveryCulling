#pragma once

#include <Matrix4x4.h>

#include "../../FrotbiteCullingSystemCore.h"
#include "Triangle.h"
#include "../../DataStructure/AABB.h"
#include "SWDepthBuffer.h"

namespace culling
{
	/// <summary>
	/// 
	/// Masked SW Occlusion Culling
	/// 
	/// 
	/// Supported SIMD Version : AVX1
	/// 
	/// 
	/// references : https://software.intel.com/content/www/us/en/develop/articles/masked-software-occlusion-culling.html
	/// </summary>
	class MaskedSWOcclusionCulling
	{
		
	private:


		SWDepthBuffer mDepthBuffer;

		float mNearClipPlaneDis, mFarClipPlaneDis;
		float* mViewProjectionMatrix;

		/// <summary>
		/// Project Vertex To ScreenSpace
		/// X, Y, Z, W -> X/W , Y/W, Z/W, W/1
		/// </summary>
		/// <param name="outClipVertexX"></param>
		/// <param name="outClipVertexY"></param>
		/// <param name="outClipVertexZ"></param>
		/// <param name="outClipVertexW"></param>
		void ConverClipSpaceToNDCSpace(M128F* outClipVertexX, M128F* outClipVertexY, const M128F* oneDividedByW, unsigned int& triangleMask);
		
		/// <summary>
		/// Convert NDC Space X,Y To Screen Space X,Y according to Graphics API
		/// And Snap Screen Space X,Y To Pixel Coordinate in Buffer
		/// 
		/// Pixel Coordinate have integer value
		/// </summary>
		/// <param name="ndcSpaceVertexX"></param>
		/// <param name="ndcSpaceVertexY"></param>
		/// <param name="triangleMask"></param>
		/// <param name="outScreenPixelSpaceX"></param>
		/// <param name="outScreenPixelSpaceY"></param>
		void ConvertNDCSpaceToScreenPixelSpace(const M128F* ndcSpaceVertexX, const M128F* ndcSpaceVertexY, M128I* outScreenPixelSpaceX, M128I* outScreenPixelSpaceY, unsigned int& triangleMask);

		/// <summary>
		/// Convert 4 TriangSles's Model Vertex to ClipSpace
		/// </summary>
		/// <param name="vertX">three M128F -> 12 floating-point value</param>
		/// <param name="vertY">three M128F -> 12 floating-point value</param>
		/// <param name="vertW">"W" !!!!!!!!! three M128F -> 12 floating-point value</param>
		/// <param name="modelToClipspaceMatrix">column major 4x4 matrix</param>
		void TransformVertexsToClipSpace(
			M128F* outClipVertexX, M128F* outClipVertexY,
 M128F* outClipVertexW,

 const float* toClipspaceMatrix, 
			unsigned int& triangleMask);

		/// <summary>
		/// Sort TwoDTriangle Points y ascending.
		/// Point1 is TopMost Vector2
		/// </summary>
		/// <param name="triangle"></param>
		void SortTriangle(TwoDTriangle& triangle);

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
		/// </summary>
		void GatherVertex(
			const Vector3* vertices, const unsigned int* vertexIndices, const size_t indiceCount,
 const size_t currentIndiceIndex, 
			M128F * outVerticesX, M128F * outVerticesY, unsigned int& triangleMask//, unsigned int* triMask
		);

		/// <summary>
		/// Bin Triangles
		/// </summary>
		/// <param name="vertices"></param>
		/// <param name="vertexIndices"></param>
		/// <param name="indiceCount"></param>
		void BinTriangles(const Vector3* vertices, const unsigned int* vertexIndices, size_t indiceCount, float* modelToClipspaceMatrix);
		

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Hierarchical Depth Buffer
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		/// <summary>
		/// 
		/// </summary>
		void UpdateHierarchicalDepthBuffer()
		{
			/*
			// Discard working layer heuristic
			dist1t = tile.zMax1 - tri.zMax
			dist01 = tile.zMax0 - tile.zMax1
			if (dist1t > dist01)
				tile.zMax1 = 0
				tile.mask = 0
			// Merge current triangle into working layer
			tile.zMax1 = max(tile.zMax1, tri.zMax)
			tile.mask |= tri.coverageMask
			// Overwrite ref. layer if working layer full
			if (tile.mask == ~0)
				tile.zMax0 = tile.zMax1
				tile.zMax1 = 0
				tile.mask = 0
			*/
		}

		/// <summary>
		/// Real Time Redering 3rd ( 873p )
		/// </summary>
		void ComputeTrianglesDepthValueInTile();

		/// <summary>
		/// Compute Depth in Bin of Tile(Sub Tile)
		/// 
		/// CoverageMask, z0DepthMax, z1DepthMax, Triangle Max Depth
		/// </summary>
		void RasterizeBinnedTriangles()
		{
			// Compute Min Depth In Tile(SubTile)
			// ComputeTrianglesDepthValueInTile

			// UpdateHierarchicalDepthBuffer
		}
		

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Occludee Depth Test
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		M128F ComputeMinimumDepths(const M128F* worldAABBVertX, const M128F* worldAABBVertY, const M128F* worldAABBVertZ)
		{
			//mViewProjectionMatrix
		}

	public:


		MaskedSWOcclusionCulling(unsigned int width, unsigned int height, float nearClipPlaneDis, float farClipPlaneDis, float* viewProjectionMatrix);
	
		void SetNearFarClipPlaneDistance(float nearClipPlaneDis, float farClipPlaneDis);
		void SetViewProjectionMatrix(float* viewProjectionMatrix);
		
		inline void DrawOccluderTriangles(const culling::Vector3* vertices, const unsigned int* vertexIndices, size_t indiceCount, float* modelToClipspaceMatrix)
		{
			while (true)
			{
				
				//Gather Vertex

				this->BinTriangles(vertices, vertexIndices, indiceCount, modelToClipspaceMatrix);

				this->RasterizeBinnedTriangles();
			}

	
		}


		/// <summary>
		/// Depth Test Multiple Occludees
		/// </summary>
		/// <param name="worldAABBs"></param>
		inline void DepthTestOccludee(const AABB* worldAABBs, size_t aabbCount, char* visibleBitFlags)
		{
			//Check Area of TwoDTriangle
			//if Area is smaller than setting, Dont draw that occluder
			//Occluder should be huge to occlude occludee

			//Compute BoundingBoxs(Sub Tile)
			//

			//!!!!!!!
			//We Just need Minimum Depth Of AABB ( regardless with Tile )
			//And Just Compare this minimum depth with Max depth of Tiles
			//Dont Try drawing triangles of AABB, Just Compute MinDepth of AABB

			M128F aabbVertexX[3];
			M128F aabbVertexY[3];
			M128F aabbVertexZ[3];

			// 3 AABB is checked per loop
			for (size_t i = 0; i < aabbCount; i += 3)
			{
				// ComputeMinimumDepths
			}

		

		}

		inline void DrawOccluderTriangles()
		{
			 
		}
	};
}

