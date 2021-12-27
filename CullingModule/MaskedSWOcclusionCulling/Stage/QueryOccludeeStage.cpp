#include "QueryOccludeeStage.h"

#include "../MaskedSWOcclusionCulling.h"
#include "../Utility/vertexTransformationHelper.h"
#include "../Utility/depthBufferTileHelper.h"

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
			const culling::Vec4& aabbMinPoint = entityBlock->mAABBMinLocalPoint[entityIndex];
			const culling::Vec4& aabbMaxPoint = entityBlock->mAABBMaxLocalPoint[entityIndex];

			const culling::Mat4x4 modelToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex) * *reinterpret_cast<const culling::Mat4x4*>(entityBlock->GetModelMatrix(entityIndex));



			culling::M256F aabbVertexX = _mm256_setr_ps(aabbMinPoint.values[0], aabbMinPoint.values[0], aabbMinPoint.values[0], aabbMinPoint.values[0], aabbMaxPoint.values[0], aabbMaxPoint.values[0], aabbMaxPoint.values[0], aabbMaxPoint.values[0]);
			culling::M256F aabbVertexY = _mm256_setr_ps(aabbMinPoint.values[1], aabbMinPoint.values[1], aabbMaxPoint.values[1], aabbMaxPoint.values[1], aabbMinPoint.values[1], aabbMinPoint.values[1], aabbMaxPoint.values[1], aabbMaxPoint.values[1]);
			culling::M256F aabbVertexZ = _mm256_setr_ps(aabbMinPoint.values[2], aabbMaxPoint.values[2], aabbMinPoint.values[2], aabbMaxPoint.values[2], aabbMinPoint.values[2], aabbMaxPoint.values[2], aabbMinPoint.values[2], aabbMaxPoint.values[2]);
			culling::M256F aabbVertexW;

			culling::vertexTransformationHelper::TransformVertexToClipSpace
			(
				aabbVertexX,
				aabbVertexY,
				aabbVertexZ,
				aabbVertexW,
				modelToClipSpaceMatrix.data()
			);
			//Now ClipSpace !!

			//oneDividedByW finally become oneDividedByW
			const culling::M256F oneDividedByW = culling::M256F_DIV(_mm256_set1_ps(1.0f), aabbVertexW);

			culling::vertexTransformationHelper::ConvertClipSpaceVertexToNDCSpace
			(
				aabbVertexX,
				aabbVertexY,
				aabbVertexZ,
				oneDividedByW
			);
			//Now NDC!!

			//Get Min Z
			// if min z of occludee is larger than depth buffer, it's culled!!
			float aabbMinDepthValue = 1.0f;

			for(size_t i = 0 ; i < 8 ; i++)
			{
				aabbMinDepthValue = MIN(aabbMinDepthValue, reinterpret_cast<const float*>(&aabbVertexZ)[i]);
			}

			culling::M256F aabbScreenSpaceVertexX;
			culling::M256F aabbScreenSpaceVertexY;

			culling::vertexTransformationHelper::ConvertNDCSpaceVertexToScreenPixelSpace
			(
				aabbVertexX,
				aabbVertexY,
				aabbScreenSpaceVertexX,
				aabbScreenSpaceVertexY,
				mMaskedOcclusionCulling->mDepthBuffer
			);

			int outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY;
				
			culling::depthBufferTileHelper::ComputeBinBoundingBoxFromVertex
			(
				aabbScreenSpaceVertexX,
				aabbScreenSpaceVertexY,
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
						break;
					}
				}
			}

			if(isCullded == true)
			{
				entityBlock->SetCulled(entityIndex, cameraIndex);
			}
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

