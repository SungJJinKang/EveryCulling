#include "MaskedSWOcclusionCulling.h"

#include "Stage/BinTrianglesStage.h"
#include "Stage/RasterizeTrianglesStage.h"

void culling::MaskedSWOcclusionCulling::ResetDepthBuffer()
{
	mDepthBuffer.Reset();
	
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
	CullingModule{ everyCulling},
	mEveryCulling{everyCulling},
	mDepthBuffer { depthBufferWidth, depthBufferheight },
	binCountInRow{ depthBufferWidth / SUB_TILE_WIDTH },
	binCountInColumn{ depthBufferheight / SUB_TILE_HEIGHT },
	mBinTrianglesStage{this},
	mRasterizeTrianglesStage{this},
	mSolveMeshRoleStage{ this }
{
	assert(depthBufferWidth% TILE_WIDTH == 0);
	assert(depthBufferheight% TILE_HEIGHT == 0);
}

void culling::MaskedSWOcclusionCulling::ResetState()
{
	ResetDepthBuffer();
}

void culling::MaskedSWOcclusionCulling::CullBlockEntityJob(const size_t cameraIndex)
{
}


	


