#include "QueryOccludeeStage.h"

#include <limits>

#include "../MaskedSWOcclusionCulling.h"
#include "../Utility/vertexTransformationHelper.h"
#include "../Utility/depthBufferTileHelper.h"

#include <Rendering/Renderer/Renderer.h>

FORCE_INLINE float culling::QueryOccludeeStage::MinFloatFromM256F(const culling::M256F& data)
{
	float min = FLT_MAX;
	for (size_t i = 0; i < 8; i++)
	{
		min = MIN(min, reinterpret_cast<const float*>(&data)[i]);
	}
	return min;
}

FORCE_INLINE float culling::QueryOccludeeStage::MaxFloatFromM256F(const culling::M256F& data)
{
	float max = -FLT_MAX;
	for (size_t i = 0; i < 8; i++)
	{
		max = MAX(max, reinterpret_cast<const float*>(&data)[i]);
	}
	return max;
}

FORCE_INLINE void culling::QueryOccludeeStage::ComputeBinBoundingBoxFromVertex
(
	const culling::M256F& screenPixelX,
	const culling::M256F& screenPixelY,
	int& outBinBoundingBoxMinX,
	int& outBinBoundingBoxMinY,
	int& outBinBoundingBoxMaxX,
	int& outBinBoundingBoxMaxY,
	SWDepthBuffer& depthBuffer
)
{
	int minScreenPixelX, minScreenPixelY, maxScreenPixelX, maxScreenPixelY;
	
	minScreenPixelX = MAX(0, (int)MinFloatFromM256F(screenPixelX));
	minScreenPixelY = MAX(0, (int)MinFloatFromM256F(screenPixelY));
	maxScreenPixelX = MAX(0, (int)MaxFloatFromM256F(screenPixelX));
	maxScreenPixelY = MAX(0, (int)MaxFloatFromM256F(screenPixelY));

	ComputeBinBoundingBoxFromVertex
	(
		minScreenPixelX,
		minScreenPixelX,
		maxScreenPixelX,
		maxScreenPixelY,
		outBinBoundingBoxMinX,
		outBinBoundingBoxMinY,
		outBinBoundingBoxMaxX,
		outBinBoundingBoxMaxY,
		depthBuffer
	);
}

FORCE_INLINE void culling::QueryOccludeeStage::ComputeBinBoundingBoxFromVertex
(
	const int minScreenPixelX,
	const int minScreenPixelY,
	const int maxScreenPixelX,
	const int maxScreenPixelY,
	int& outBinBoundingBoxMinX,
	int& outBinBoundingBoxMinY, 
	int& outBinBoundingBoxMaxX, 
	int& outBinBoundingBoxMaxY, 
	SWDepthBuffer& depthBuffer
)
{
	static const int WIDTH_MASK = ~(TILE_WIDTH - 1);
	static const int HEIGHT_MASK = ~(TILE_HEIGHT - 1);

	const int clampedMinScreenPixelX = culling::CLAMP(minScreenPixelX, 0, (int)(depthBuffer.mResolution.mWidth));
	const int clampedMinScreenPixelY = culling::CLAMP(minScreenPixelY, 0, (int)(depthBuffer.mResolution.mHeight));
	const int clampedMaxScreenPixelX = culling::CLAMP(maxScreenPixelX, 0, (int)(depthBuffer.mResolution.mWidth));
	const int clampedMaxScreenPixelY = culling::CLAMP(maxScreenPixelY, 0, (int)(depthBuffer.mResolution.mHeight));

	outBinBoundingBoxMinX = clampedMinScreenPixelX & WIDTH_MASK;
	outBinBoundingBoxMinY = clampedMinScreenPixelY & HEIGHT_MASK;
	outBinBoundingBoxMaxX = clampedMaxScreenPixelX & WIDTH_MASK;
	outBinBoundingBoxMaxY = clampedMaxScreenPixelY & HEIGHT_MASK;

	outBinBoundingBoxMinX = MIN(depthBuffer.mResolution.mRightTopTileOrginX, MAX(outBinBoundingBoxMinX, depthBuffer.mResolution.mLeftBottomTileOrginX));
	outBinBoundingBoxMinY = MIN(depthBuffer.mResolution.mRightTopTileOrginY, MAX(outBinBoundingBoxMinY, depthBuffer.mResolution.mLeftBottomTileOrginY));
	outBinBoundingBoxMaxX = MAX(depthBuffer.mResolution.mLeftBottomTileOrginX, MIN(outBinBoundingBoxMaxX, depthBuffer.mResolution.mRightTopTileOrginX));
	outBinBoundingBoxMaxY = MAX(depthBuffer.mResolution.mLeftBottomTileOrginY, MIN(outBinBoundingBoxMaxY, depthBuffer.mResolution.mRightTopTileOrginY));

	assert(outBinBoundingBoxMinX <= outBinBoundingBoxMaxX);
	assert(outBinBoundingBoxMinY <= outBinBoundingBoxMaxY);
}


void culling::QueryOccludeeStage::QueryOccludee
(
	const size_t cameraIndex, 
	culling::EntityBlock* const entityBlock
)
{
	for(size_t entityIndex = 0 ; entityIndex < entityBlock->mCurrentEntityCount ; entityIndex++)
	{
		int outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY;
		
		if
		(
			entityBlock->GetIsCulled(entityIndex, cameraIndex) == false && 
			entityBlock->GetIsMinNDCZDataUsedForQuery(entityIndex) == true // if IsMinNDCZDataUsedForQuery is true, mIsAABBScreenSpacePointValid is also true
		)
		{
			ComputeBinBoundingBoxFromVertex
			(
				entityBlock->mAABBMinScreenSpacePointX[entityIndex],
				entityBlock->mAABBMinScreenSpacePointY[entityIndex],
				entityBlock->mAABBMaxScreenSpacePointX[entityIndex],
				entityBlock->mAABBMaxScreenSpacePointY[entityIndex],
				outBinBoundingBoxMinX,
				outBinBoundingBoxMinY,
				outBinBoundingBoxMaxX,
				outBinBoundingBoxMaxY,
				mMaskedOcclusionCulling->mDepthBuffer
			);

			const int intersectingMinBoxX = outBinBoundingBoxMinX; // this is screen space coordinate
			const int intersectingMinBoxY = outBinBoundingBoxMinY;
			const int intersectingMaxBoxX = outBinBoundingBoxMaxX;
			const int intersectingMaxBoxY = outBinBoundingBoxMaxY;

			assert(intersectingMinBoxX <= intersectingMaxBoxX);
			assert(intersectingMinBoxY <= intersectingMaxBoxY);

			const int startBoxIndexX = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMinBoxX / TILE_WIDTH);
			const int startBoxIndexY = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMinBoxY / TILE_HEIGHT);
			const int endBoxIndexX = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMaxBoxX / TILE_WIDTH);
			const int endBoxIndexY = MIN((int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMaxBoxY / TILE_HEIGHT);

			assert(startBoxIndexX >= 0 && startBoxIndexX < (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(startBoxIndexY >= 0 && startBoxIndexY < (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));

			assert(endBoxIndexX >= 0 && endBoxIndexX <= (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(endBoxIndexY >= 0 && endBoxIndexY <= (int)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));

			const float aabbMinDepthValue = entityBlock->mAABBMinNDCZ[entityIndex];

			bool isCullded = true;

			for (size_t y = startBoxIndexY; y <= endBoxIndexY; y++)
			{
				for (size_t x = startBoxIndexX; x <= endBoxIndexX; x++)
				{
					const culling::Tile* const tile = mMaskedOcclusionCulling->mDepthBuffer.GetTile(y, x);

					if(aabbMinDepthValue < tile->mHizDatas.L0MaxDepthValue)
					{
						// occludee is not culled!
						isCullded = false;

						goto escapeNestedLoop;
					}
				}
			}
			
			entityBlock->SetCulled(entityIndex, cameraIndex);

			escapeNestedLoop:
			do 
			{}
			while (false);
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
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);

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

const char* culling::QueryOccludeeStage::GetCullingModuleName() const
{
	return "QueryOccludeeStage";
}

