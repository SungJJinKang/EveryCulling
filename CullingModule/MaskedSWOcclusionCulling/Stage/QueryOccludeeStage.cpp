#include "QueryOccludeeStage.h"

#include "../MaskedSWOcclusionCulling.h"


void culling::QueryOccludeeStage::QueryOccludee
(
	const size_t cameraIndex, 
	culling::EntityBlock* const entityBlock
)
{
	for(size_t entityIndex = 0 ; entityIndex < entityBlock->mCurrentEntityCount ; entityIndex++)
	{
		if
		(
			entityBlock->GetIsCulled(entityIndex, cameraIndex) == false &&
			entityBlock->GetIsOccluder(entityIndex, cameraIndex) == false
		)
		{
			
		}
	}
}

/*
void culling::QueryOccludeeStage::DepthTestOccludee
(
	const AABB* worldAABBs,
	size_t aabbCount,
	char* visibleBitFlags
)
{
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

*/
culling::QueryOccludeeStage::QueryOccludeeStage
(
	MaskedSWOcclusionCulling* mOcclusionCulling
)
	: MaskedSWOcclusionCullingStage{ mOcclusionCulling }
{
}

void culling::QueryOccludeeStage::CullBlockEntityJob(const size_t cameraIndex)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);;

		if (nextEntityBlock != nullptr)
		{
			QueryOccludee(cameraIndex, nextEntityBlock);
		}
		else
		{
			break;
		}
	}
}

