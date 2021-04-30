#pragma once

#include <Matrix4x4.h>

#include "../../FrotbiteCullingSystemCore.h"
#include "Triangle.h"
#include "../../DataStructure/AABB.h"
#include "SWDepthBuffer.h"

namespace culling
{
	class MaskedSWOcclusionCulling
	{
		
	private:

		unsigned int mScreenWidth;
		unsigned int mScreenHeight;
		SWDepthBuffer mDepthBuffer;

		float mNearPlaneDis, mFarPlaneDis;
		math::Matrix4x4 mViewProjectionMatrix;

		/// <summary>
		/// Convert 4 TriangSles's Model Vertex to ClipSpace
		/// </summary>
		/// <param name="vertX">three M128F -> 12 floating-point value</param>
		/// <param name="vertY">three M128F -> 12 floating-point value</param>
		/// <param name="vertZ">three M128F -> 12 floating-point value</param>
		/// <param name="modelToClipspaceMatrix">column major 4x4 matrix</param>
		void TransformVertexs(M128F* vertX, M128F* vertY, M128F* vertZ, const float* modelToClipspaceMatrix);

		inline float GetAreaOfTriangle(const Triangle& triangle)
		{

		}

		inline float GetAreaOfAAABB(const AABB& worldAABB)
		{

		}

		/// <summary>
		/// Gather Vertex from VertexList with IndiceList
		/// </summary>
		void GatherVertex(
			const float* vertices, const unsigned int* vertexIndices, size_t indiceCount, size_t currentIndiceCount,
			M128F* verticesX, M128F* verticesY, M128F* verticesZ//, unsigned int* triMask
		);

		/// <summary>
		/// Bin Triangles
		/// </summary>
		/// <param name="vertices"></param>
		/// <param name="vertexIndices"></param>
		/// <param name="indiceCount"></param>
		void BinTriangles(const float* vertices, const unsigned int* vertexIndices, size_t indiceCount, float* modelToClipspaceMatrix);
		
	public:

		inline void SetViewProjectionMatrix(const math::Matrix4x4& viewProjectionMatrix)
		{
			this->mViewProjectionMatrix = viewProjectionMatrix;
		}
		
		inline void DrawOccluderTriangles(const float* vertices, const unsigned int* vertexIndices, size_t indiceCount)
		{
			while (true)
			{
				


				//Compute BoundingBox

				//Bin Triangles 



				//Pass Triangles to Rasterizer
			}

	
		}


		inline void DrawOccludeeTriangles(const AABB& worldAABB)
		{
			//Check Area of Triangle
			//if Area is smaller than setting, Dont draw that occluder
			//Occluder should be huge to occlude occludee
		}

		inline void DrawOccluderTriangles()
		{
			 
		}
	};
}

