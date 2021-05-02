#pragma once

#include <Matrix4x4.h>

#include "../../FrotbiteCullingSystemCore.h"
#include "Triangle.h"
#include "../../DataStructure/AABB.h"
#include "SWDepthBuffer.h"

#include "SIMD_Core.h"

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
		void ConverClipSpaceToNDCSpace(
			M256F* outClipVertexX, M256F* outClipVertexY,
 const M256F* oneDividedByW, int & triangleMask
		);
		
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
		void ConvertNDCSpaceToScreenPixelSpace(
			const M256F* ndcSpaceVertexX, const M256F* ndcSpaceVertexY,
 
			M256F* outScreenPixelSpaceX,
 M256F* outScreenPixelSpaceY, int & triangleMask
		);

		/// <summary>
		/// Convert 4 TriangSles's Model Vertex to ClipSpace
		/// </summary>
		/// <param name="vertX">three M256F -> 12 floating-point value</param>
		/// <param name="vertY">three M256F -> 12 floating-point value</param>
		/// <param name="vertW">"W" !!!!!!!!! three M256F -> 12 floating-point value</param>
		/// <param name="modelToClipspaceMatrix">column major 4x4 matrix</param>
		void TransformVertexsToClipSpace(
			M256F* outClipVertexX, M256F* outClipVertexY,
 M256F* outClipVertexW,
			const float* toClipspaceMatrix, int & triangleMask
		);

	

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
		void GatherVertex(
			const Vector3* vertices, const unsigned int* vertexIndices, const size_t indiceCount,
 const size_t currentIndiceIndex, 
			M256F* outVerticesX, M256F* outVerticesY, int & triangleMask//, unsigned int* triMask
		);

		/*
		int CullBackfaces(M256F* pVtxX, M256F* pVtxY, M256F* pVtxZ, const M256F& ccwMask, BackfaceWinding bfWinding)
		{
			// Reverse vertex order if non cw faces are considered front facing (rasterizer code requires CCW order)
			if (!(bfWinding & BACKFACE_CW))
			{
				__mw tmpX, tmpY, tmpZ;
				tmpX = _mmw_blendv_ps(pVtxX[2], pVtxX[0], ccwMask);
				tmpY = _mmw_blendv_ps(pVtxY[2], pVtxY[0], ccwMask);
				tmpZ = _mmw_blendv_ps(pVtxZ[2], pVtxZ[0], ccwMask);
				pVtxX[2] = _mmw_blendv_ps(pVtxX[0], pVtxX[2], ccwMask);
				pVtxY[2] = _mmw_blendv_ps(pVtxY[0], pVtxY[2], ccwMask);
				pVtxZ[2] = _mmw_blendv_ps(pVtxZ[0], pVtxZ[2], ccwMask);
				pVtxX[0] = tmpX;
				pVtxY[0] = tmpY;
				pVtxZ[0] = tmpZ;
			}

			// Return a lane mask with all front faces set
			return ((bfWinding & BACKFACE_CCW) ? 0 : _mmw_movemask_ps(ccwMask)) | ((bfWinding & BACKFACE_CW) ? 0 : ~_mmw_movemask_ps(ccwMask));
		}
		*/
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
		/// 
		/// reference : 
		/// https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation
		/// https://www.rose-hulman.edu/class/cs/csse351/m10/triangle_fill.pdf
		/// https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
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

		M256F ComputeMinimumDepths(const M256F* worldAABBVertX, const M256F* worldAABBVertY, const M256F* worldAABBVertZ)
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

			M256F aabbVertexX[3];
			M256F aabbVertexY[3];
			M256F aabbVertexZ[3];

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

