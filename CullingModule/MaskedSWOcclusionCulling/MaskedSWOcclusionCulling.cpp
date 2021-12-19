#include "MaskedSWOcclusionCulling.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeTrianglesStage.h"

void culling::MaskedSWOcclusionCulling::ResetDepthBuffer()
{
	const size_t tileCount = static_cast<size_t>(mDepthBuffer.mResolution.mTileCountInARow) * static_cast<size_t>(mDepthBuffer.mResolution.mTileCountInAColumn);
	Tile* const tiles = mDepthBuffer.mTiles;
	
	for (size_t i = 0; i < tileCount; i++)
	{
		tiles[i].mBinnedTriangles.mCurrentTriangleCount = 0;
		tiles[i].mHizDatas.depthPosition = _mm256_set1_epi32(0);
		tiles[i].mHizDatas.z0Max = _mm256_set1_ps(1.0f);
		tiles[i].mHizDatas.z1Max = _mm256_set1_ps(1.0f);
	}
	
}

void culling::MaskedSWOcclusionCulling::DrawOccluderTriangles
(
	const float* vertices, const std::uint32_t* vertexIndices,
	size_t indiceCount, bool vertexStrideByte, float* modelToClipspaceMatrix
)
{
	mBinTrianglesStage.BinTriangles(vertices, vertexIndices, indiceCount, vertexStrideByte, modelToClipspaceMatrix);

	//mRasterizeTrianglesStage.RasterizeBinnedTriangles();
}

void culling::MaskedSWOcclusionCulling::DepthTestOccludee
(
	const AABB* worldAABBs, 
	size_t aabbCount,
	char* visibleBitFlags
)
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

	culling::M256F aabbVertexX[3];
	culling::M256F aabbVertexY[3];
	culling::M256F aabbVertexZ[3];

	///Temporarily remove unused varaible warning
	(void)aabbVertexX, (void)aabbVertexY, (void)aabbVertexZ;

	// 3 AABB is checked per loop
	for (size_t i = 0; i < aabbCount; i += 3)
	{
		// ComputeMinimumDepths
	}



}

culling::MaskedSWOcclusionCulling::MaskedSWOcclusionCulling
(
	EveryCulling* everyCulling, 
	const std::uint32_t depthBufferWidth,
	const std::uint32_t depthBufferheight
)
	:
	culling::CullingModule{everyCulling}, 
	mDepthBuffer { depthBufferWidth, depthBufferheight },
	binCountInRow{ depthBufferWidth / SUB_TILE_WIDTH },
	binCountInColumn{ depthBufferheight / SUB_TILE_HEIGHT },
	mBinTrianglesStage{*this},
	mRasterizeTrianglesStage{*this},
	mSolveMeshRoleStage{ *this }
{
	assert(depthBufferWidth% TILE_WIDTH == 0);
	assert(depthBufferheight% TILE_HEIGHT == 0);
}

void culling::MaskedSWOcclusionCulling::SetNearFarClipPlaneDistance
(
	const size_t cameraIndex, 
	const float nearClipPlaneDis, 
	const float farClipPlaneDis
)
{
	mNearClipPlaneDistance[cameraIndex] = nearClipPlaneDis;
	mFarClipPlaneDistance[cameraIndex] = farClipPlaneDis;
}

void culling::MaskedSWOcclusionCulling::SetFieldOfViewInDegree(const size_t cameraIndex, const float fov)
{
	mFieldOfViewInDegree[cameraIndex] = fov;
}


void culling::MaskedSWOcclusionCulling::ResetState()
{
	ResetDepthBuffer();
	mCurrentStage = eMaskedOcclusionCullingStage::SolveMeshRole;
}

void culling::MaskedSWOcclusionCulling::MaskedSWOcclusionJob()
{
	//TODO : How to decide which objects is used as occluder
	//FIRST : Use EntityBlock::mPositionAndBoundingSpheres's w value(Bounding Sphere Radius), If Bounding Sphere Radius is too small, It is not valuable as Occluder
	//SECOND : Use EntityBlock::mPositionAndBoundingSpheres's x, y, z(Object's Position), If Object is close to camera, Use it Occluder
}

void culling::MaskedSWOcclusionCulling::CullBlockEntityJob
(
	EntityBlock* const currentEntityBlock, 
	const size_t entityCountInBlock,
	const size_t cameraIndex
)
{
	switch(mCurrentStage)
	{
	case eMaskedOcclusionCullingStage::SolveMeshRole:

		for (size_t entityIndex = 0; entityIndex < entityCountInBlock; entityIndex++)
		{
			if (currentEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false)
			{
				//Yes!! it pass previous culling modules
				mSolveMeshRoleStage.DoStageJob(currentEntityBlock, entityIndex, cameraIndex);
			}
		}

		break;

	case eMaskedOcclusionCullingStage::BinTriangles:

		for (size_t entityIndex = 0; entityIndex < entityCountInBlock; entityIndex++)
		{
			if
			(
				currentEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false && 
				currentEntityBlock->mIsOccluder[entityIndex] == true
			)
			{
				//Yes!! it pass previous culling modules
				mBinTrianglesStage.DoStageJob(currentEntityBlock, entityIndex, cameraIndex);
			}
		}

		break;


	case eMaskedOcclusionCullingStage::DrawOccluder:

		for (size_t entityIndex = 0; entityIndex < entityCountInBlock; entityIndex++)
		{
			if (currentEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false)
			{
				//Yes!! it pass previous culling modules
				//mBinTrianglesStage.DoStageJob(currentEntityBlock, entityIndex, cameraIndex);
			}
		}

		break;


	case eMaskedOcclusionCullingStage::QueryOccludee:

		for (size_t entityIndex = 0; entityIndex < entityCountInBlock; entityIndex++)
		{
			if (currentEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false)
			{
				//Yes!! it pass previous culling modules
				//mBinTrianglesStage.DoStageJob(currentEntityBlock, entityIndex, cameraIndex);
			}
		}
		
		break;
		
	case eMaskedOcclusionCullingStage::Finished:

		break;

	default:
		assert(0);
	}

	
	
}




	


