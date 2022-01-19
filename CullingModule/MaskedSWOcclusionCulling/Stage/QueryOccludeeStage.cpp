#include "QueryOccludeeStage.h"

#include <limits>

#include "../MaskedSWOcclusionCulling.h"
#include "../Utility/vertexTransformationHelper.h"
#include "../Utility/depthBufferTileHelper.h"

#include <Rendering/Renderer/Renderer.h>

EVERYCULLING_FORCE_INLINE float culling::QueryOccludeeStage::MinFloatFromM256F(const culling::M256F& data)
{
	float min = FLT_MAX;
	for (size_t i = 0; i < 8; i++)
	{
		min = MIN(min, reinterpret_cast<const float*>(&data)[i]);
	}
	return min;
}

EVERYCULLING_FORCE_INLINE float culling::QueryOccludeeStage::MaxFloatFromM256F(const culling::M256F& data)
{
	float max = -FLT_MAX;
	for (size_t i = 0; i < 8; i++)
	{
		max = MAX(max, reinterpret_cast<const float*>(&data)[i]);
	}
	return max;
}

EVERYCULLING_FORCE_INLINE void culling::QueryOccludeeStage::ComputeBinBoundingBoxFromVertex
(
	const culling::M256F& screenPixelX,
	const culling::M256F& screenPixelY,
	std::uint32_t& outBinBoundingBoxMinX,
	std::uint32_t& outBinBoundingBoxMinY,
	std::uint32_t& outBinBoundingBoxMaxX,
	std::uint32_t& outBinBoundingBoxMaxY,
	SWDepthBuffer& depthBuffer
)
{
	float minScreenPixelX, minScreenPixelY, maxScreenPixelX, maxScreenPixelY;
	
	minScreenPixelX = MAX(0, (float)MinFloatFromM256F(screenPixelX));
	minScreenPixelY = MAX(0, (float)MinFloatFromM256F(screenPixelY));
	maxScreenPixelX = MAX(0, (float)MaxFloatFromM256F(screenPixelX));
	maxScreenPixelY = MAX(0, (float)MaxFloatFromM256F(screenPixelY));

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

EVERYCULLING_FORCE_INLINE void culling::QueryOccludeeStage::ComputeBinBoundingBoxFromVertex
(
	const float minScreenPixelX,
	const float minScreenPixelY,
	const float maxScreenPixelX,
	const float maxScreenPixelY,
	std::uint32_t& outBinBoundingBoxMinX,
	std::uint32_t& outBinBoundingBoxMinY,
	std::uint32_t& outBinBoundingBoxMaxX,
	std::uint32_t& outBinBoundingBoxMaxY,
	SWDepthBuffer& depthBuffer
)
{
	static const int WIDTH_MASK = ~(TILE_WIDTH - 1);
	static const int HEIGHT_MASK = ~(TILE_HEIGHT - 1);

	const int clampedMinScreenPixelX = culling::CLAMP((int)minScreenPixelX, (int)0, (int)(depthBuffer.mResolution.mWidth));
	const int clampedMinScreenPixelY = culling::CLAMP((int)minScreenPixelY, (int)0, (int)(depthBuffer.mResolution.mHeight));
	const int clampedMaxScreenPixelX = culling::CLAMP((int)maxScreenPixelX, (int)0, (int)(depthBuffer.mResolution.mWidth));
	const int clampedMaxScreenPixelY = culling::CLAMP((int)maxScreenPixelY, (int)0, (int)(depthBuffer.mResolution.mHeight));

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
		std::uint32_t outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY;
		
		if
		(
			entityBlock->GetIsCulled(entityIndex, cameraIndex) == false && 
			entityBlock->GetIsAllAABBClipPointWPositive(entityIndex) == true // if IsMinNDCZDataUsedForQuery is true, mIsAnyAABBClipPointWNegative is also true
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

			const std::uint32_t intersectingMinBoxX = outBinBoundingBoxMinX; // this is screen space coordinate
			const std::uint32_t intersectingMinBoxY = outBinBoundingBoxMinY;
			const std::uint32_t intersectingMaxBoxX = outBinBoundingBoxMaxX;
			const std::uint32_t intersectingMaxBoxY = outBinBoundingBoxMaxY;

			assert(intersectingMinBoxX <= intersectingMaxBoxX);
			assert(intersectingMinBoxY <= intersectingMaxBoxY);

			const std::uint32_t startBoxIndexX = MIN((std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMinBoxX / (std::uint32_t)TILE_WIDTH);
			const std::uint32_t startBoxIndexY = MIN((std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMinBoxY / (std::uint32_t)TILE_HEIGHT);
			const std::uint32_t endBoxIndexX = MIN((std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount - 1), intersectingMaxBoxX / (std::uint32_t)TILE_WIDTH);
			const std::uint32_t endBoxIndexY = MIN((std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount - 1), intersectingMaxBoxY / (std::uint32_t)TILE_HEIGHT);

			assert(startBoxIndexX >= 0 && startBoxIndexX < (std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(startBoxIndexY >= 0 && startBoxIndexY < (std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));

			assert(endBoxIndexX >= 0 && endBoxIndexX <= (std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mColumnTileCount));
			assert(endBoxIndexY >= 0 && endBoxIndexY <= (std::uint32_t)(mMaskedOcclusionCulling->mDepthBuffer.mResolution.mRowTileCount));

			const float aabbMinDepthValue = entityBlock->mAABBMinNDCZ[entityIndex];

			bool isCullded = true;

			for (std::uint32_t y = startBoxIndexY; y <= endBoxIndexY; y++)
			{
				for (std::uint32_t x = startBoxIndexX; x <= endBoxIndexX; x++)
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

void culling::QueryOccludeeStage::CullBlockEntityJob(const size_t cameraIndex, const std::int32_t localThreadIndex, const unsigned long long currentTickCount)
{
	if(mMaskedOcclusionCulling->GetIsOccluderExist() == true)
	{
		while (true)
		{
			culling::EntityBlock* const nextEntityBlock = GetNextEntityBlockForMultipleThreads(cameraIndex, localThreadIndex);

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
}

const char* culling::QueryOccludeeStage::GetCullingModuleName() const
{
	return "QueryOccludeeStage";
}

