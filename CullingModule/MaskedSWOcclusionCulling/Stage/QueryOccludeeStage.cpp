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

	static const int WIDTH_MASK = ~(TILE_WIDTH - 1);
	static const int HEIGHT_MASK = ~(TILE_HEIGHT - 1);

	minScreenPixelX = MAX(0, (int)MinFloatFromM256F(screenPixelX));
	minScreenPixelY = MAX(0, (int)MinFloatFromM256F(screenPixelY));
	maxScreenPixelX = MAX(0, (int)MaxFloatFromM256F(screenPixelX));
	maxScreenPixelY = MAX(0, (int)MaxFloatFromM256F(screenPixelY));

	outBinBoundingBoxMinX = minScreenPixelX & WIDTH_MASK;
	outBinBoundingBoxMinY = minScreenPixelY & HEIGHT_MASK;
	outBinBoundingBoxMaxX = maxScreenPixelX & WIDTH_MASK;
	outBinBoundingBoxMaxY = maxScreenPixelY & HEIGHT_MASK;

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
	// TODO : Query with spatial splits aabb

	for(size_t entityIndex = 0 ; entityIndex < entityBlock->mCurrentEntityCount ; entityIndex++)
	{
		if(entityBlock->GetIsCulled(entityIndex, cameraIndex) == false)
		{
			const culling::Vec4& aabbMinWorldPoint = entityBlock->mAABBMinWorldPoint[entityIndex];
			const culling::Vec4& aabbMaxWorldPoint = entityBlock->mAABBMaxWorldPoint[entityIndex];

			const culling::Mat4x4 worldToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex);



			culling::M256F aabbVertexX = _mm256_setr_ps(aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0]);
			culling::M256F aabbVertexY = _mm256_setr_ps(aabbMinWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMaxWorldPoint.values[1]);
			culling::M256F aabbVertexZ = _mm256_setr_ps(aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2]);
			culling::M256F aabbVertexW;

			// TODO : use Line Clipping Algorithm ( Cohen-Sutherland algorithm )
			// Interpolate z value between vertex and clipped vertex 

			//Transform To ClipSpace !!

			std::uint32_t aabbPointMask = (1 << 8) - 1;



			// if any vertex is out of volume, we can't do accurate query. So Just draw the triangle.
			aabbVertexW = culling::M256F_MUL_AND_ADD(aabbVertexX, _mm256_set1_ps(worldToClipSpaceMatrix.data()[3]), culling::M256F_MUL_AND_ADD(aabbVertexY, _mm256_set1_ps(worldToClipSpaceMatrix.data()[7]), culling::M256F_MUL_AND_ADD(aabbVertexZ, _mm256_set1_ps(worldToClipSpaceMatrix.data()[11]), _mm256_set1_ps(worldToClipSpaceMatrix.data()[15]))));
			aabbPointMask &= _mm256_movemask_ps(_mm256_cmp_ps(aabbVertexW, _mm256_set1_ps(std::numeric_limits<float>::epsilon()), _CMP_GE_OQ));
			if(aabbPointMask != 0x000000FF)
			{
				continue;
			}

			culling::vertexTransformationHelper::TransformVertexToClipSpace
			(
				aabbVertexX,
				aabbVertexY,
				aabbVertexZ,
				worldToClipSpaceMatrix.data()
			);

			
			/* Clipping isn't required. Just clip only vertex with negative homogeneous w
			//Now ClipSpace !!
			Clipping(aabbVertexX, aabbVertexY, aabbVertexZ, aabbVertexW, aabbPointMask);
			if (aabbPointMask != 0x000000FF)
			{
				// if any vertex is out of volume, object is never culled.
				continue;
			}
			*/


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

			// Get Min Z
			// if min z of occludee is larger than depth buffer, it's culled!!
			float aabbMinDepthValue = std::numeric_limits<float>::max();

			for(size_t i = 0 ; i < 8 ; i++)
			{
				if ((aabbPointMask & (1 << i)) != 0x00000000)
				{
					aabbMinDepthValue = MIN(aabbMinDepthValue, reinterpret_cast<const float*>(&aabbVertexZ)[i]);
				}
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
				
			ComputeBinBoundingBoxFromVertex
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

