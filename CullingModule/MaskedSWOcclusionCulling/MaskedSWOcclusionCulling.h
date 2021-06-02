#pragma once

#include <atomic>

#include "../../EveryCullingCore.h"
#include "SIMD_Core.h"
#include "SWDepthBuffer.h"

#include "../../DataType/Math/Triangle.h"
#include "../../DataType/Math/AABB.h"
#include "../../DataType/Math/Matrix.h"

#include "../CullingModule.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeTrianglesStage.h"


namespace culling
{
	/// <summary>
	/// 
	/// Masked SW Occlusion Culling
	/// 
	/// 
	/// Supported SIMD Version : AVX2
	/// 
	/// How Wokrs ? : 
	/// Please read "MaskedSWOcclusionCulling_HowWorks.md" file
	/// 
	/// references : 
	/// https://www.slideshare.net/IntelSoftware/masked-software-occlusion-culling
	/// https://www.slideshare.net/IntelSoftware/masked-occlusion-culling
	/// </summary>
	class MaskedSWOcclusionCulling : public CullingModule
	{
		friend class BinTrianglesStage;
		friend class MaskedSWOcclusionCullingStage;

		enum class eMaskedOcclusionCullingStage
		{
			BinTriangles,
			DrawOccluder,

		};

	private:

		BinTrianglesStage mBinTrianglesStage;
		RasterizeTrianglesStage mRasterizeTrianglesStage;

		SWDepthBuffer mDepthBuffer;
		
		const unsigned int binCountInRow, binCountInColumn;
		float mNearClipPlaneDis, mFarClipPlaneDis;
		float* mViewProjectionMatrix;

		void ResetDepthBuffer();




		/// <summary>
		/// 
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
		FORCE_INLINE void DrawOccluderTriangles
		(
			const float* vertices, const unsigned int* vertexIndices, size_t indiceCount, bool vertexStrideByte,
			float* modelToClipspaceMatrix
		)
		{
			this->mBinTrianglesStage.BinTriangles(vertices, vertexIndices, indiceCount, vertexStrideByte, modelToClipspaceMatrix);

			//this->mRasterizeTrianglesStage.RasterizeBinnedTriangles();
		}


		/// <summary>
		/// Depth Test Multiple Occludees
		/// </summary>
		/// <param name="worldAABBs"></param>
		FORCE_INLINE void DepthTestOccludee(const AABB* worldAABBs, size_t aabbCount, char* visibleBitFlags)
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

			///Temporarily remove unused varaible warning
			(void)aabbVertexX, (void)aabbVertexY, (void)aabbVertexZ;

			// 3 AABB is checked per loop
			for (size_t i = 0; i < aabbCount; i += 3)
			{
				// ComputeMinimumDepths
			}



		}

	public:

		MaskedSWOcclusionCulling(
			EveryCulling* frotbiteCullingSystemunsigned, 
			unsigned int depthBufferWidth, unsigned int depthBufferheight
		);
	
		void SetNearFarClipPlaneDistance(float nearClipPlaneDis, float farClipPlaneDis);
		void SetViewProjectionMatrix(float* viewProjectionMatrix);

		void ResetState();
		
		

		FORCE_INLINE void MaskedSWOcclusionJob()
		{
			 
		}

		// Inherited via CullingModule
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex) override;
	};
}

