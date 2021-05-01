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

		unsigned int mScreenWidth;
		unsigned int mScreenHeight;
		SWDepthBuffer mDepthBuffer;

		float mNearPlaneDis, mFarPlaneDis;
		float* mViewProjectionMatrix;

		/// <summary>
		/// Convert 4 TriangSles's Model Vertex to ClipSpace
		/// </summary>
		/// <param name="vertX">three M128F -> 12 floating-point value</param>
		/// <param name="vertY">three M128F -> 12 floating-point value</param>
		/// <param name="vertW">"W" !!!!!!!!! three M128F -> 12 floating-point value</param>
		/// <param name="modelToClipspaceMatrix">column major 4x4 matrix</param>
		void TransformVertexsToClipSpace(
			M128F* outClipVertexX, M128F* outClipVertexY, M128F* outClipVertexZ, M128F* outClipVertexW, 
			const float* toClipspaceMatrix);

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
			const Vector3* vertices, const unsigned int* vertexIndices, const size_t indiceCount, const size_t currentIndiceIndex, 
			M128F * outVerticesX, M128F * outVerticesY, M128F * outVerticesZ, unsigned int* triangleMask//, unsigned int* triMask
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
		/// </summary>
		void DrawBinnedTriangles()
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

		void SetViewProjectionMatrix(math::Matrix4x4& viewProjectionMatrix);
		
		inline void DrawOccluderTriangles(const float* vertices, const unsigned int* vertexIndices, size_t indiceCount)
		{
			while (true)
			{
				
				//Gather Vertex

				//Convert Model Space Vertex to Clip space Vertex


				//Convert Clip space Vertex to Integer Pixel Position
				//Check "RenderTriangles" Function of https://github.com/GameTechDev/MaskedOcclusionCulling/blob/master/MaskedOcclusionCullingCommon.inl

				//Backface Culling

				//Compute BoundingBox
				//Compute Sub Tile intersecting with triangles

				//Bin Triangles 

				
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

