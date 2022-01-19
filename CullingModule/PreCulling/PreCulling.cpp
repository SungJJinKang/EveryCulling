#include "PreCulling.h"

#include "../MaskedSWOcclusionCulling/Utility/vertexTransformationHelper.h"
#include "../MaskedSWOcclusionCulling/MaskedSWOcclusionCulling.h"

#define SCREEN_SPACE_MIN_VALUE (float)-50000.0f
#define SCREEN_SPACE_MAX_VALUE (float)50000.0f

EVERYCULLING_FORCE_INLINE void culling::PreCulling::ComputeScreenSpaceMinMaxAABBAndMinZ
(
	const size_t cameraIndex,
	culling::EntityBlock* const entityBlock, 
	const size_t entityIndex
)
{
	const culling::Vec4& aabbMinWorldPoint = entityBlock->mAABBMinWorldPoint[entityIndex];
	const culling::Vec4& aabbMaxWorldPoint = entityBlock->mAABBMaxWorldPoint[entityIndex];

	const culling::Mat4x4& worldToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex);
	
	culling::M256F aabbVertexX = _mm256_setr_ps(aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0]);
	culling::M256F aabbVertexY = _mm256_setr_ps(aabbMinWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMaxWorldPoint.values[1]);
	culling::M256F aabbVertexZ = _mm256_setr_ps(aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2]);
	culling::M256F aabbVertexW;
	
	// Convert world space to clip space
	culling::vertexTransformationHelper::TransformVertexToClipSpace
	(
		aabbVertexX,
		aabbVertexY,
		aabbVertexZ,
		aabbVertexW,
		worldToClipSpaceMatrix.data()
	);

	const culling::M256F isHomogeneousWNegative = _mm256_cmp_ps(aabbVertexW, _mm256_set1_ps(std::numeric_limits<float>::epsilon()), _CMP_LT_OQ);

	const culling::M256F oneDividedByW = culling::M256F_DIV(_mm256_set1_ps(1.0f), aabbVertexW);

	// Convert clip space to ndc space
	culling::vertexTransformationHelper::ConvertClipSpaceVertexToNDCSpace
	(
		aabbVertexX,
		aabbVertexY,
		aabbVertexZ,
		oneDividedByW
	);

	culling::M256F screenPixelPosX, screenPixelPosY;
	culling::vertexTransformationHelper::ConvertNDCSpaceVertexToScreenPixelSpace
	(
		aabbVertexX,
		aabbVertexY,
		screenPixelPosX, 
		screenPixelPosY, 
		mCullingSystem->mMaskedSWOcclusionCulling->mDepthBuffer
	);




	// Compute min, max sreen space X, Y

	
	// Do clamp min, max screen space
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = -std::numeric_limits<float>::max();
	float maxY = -std::numeric_limits<float>::max();

	// set max value to invalid vertex index
	// this is for branchless codes
	
	screenPixelPosX = _mm256_blendv_ps(screenPixelPosX, _mm256_set1_ps(std::numeric_limits<float>::max()), isHomogeneousWNegative);
	screenPixelPosY = _mm256_blendv_ps(screenPixelPosY, _mm256_set1_ps(std::numeric_limits<float>::max()), isHomogeneousWNegative);
	for(int i = 0 ; i < 8 ; i++)
	{
		minX = MIN(minX, reinterpret_cast<const float*>(&screenPixelPosX)[i]);
		minY = MIN(minY, reinterpret_cast<const float*>(&screenPixelPosY)[i]);
	}
	
	screenPixelPosX = _mm256_blendv_ps(screenPixelPosX, _mm256_set1_ps(-std::numeric_limits<float>::max()), isHomogeneousWNegative);
	screenPixelPosY = _mm256_blendv_ps(screenPixelPosY, _mm256_set1_ps(-std::numeric_limits<float>::max()), isHomogeneousWNegative);
	for (int i = 0; i < 8; i++)
	{
		maxX = MAX(maxX, reinterpret_cast<const float*>(&screenPixelPosX)[i]);
		maxY = MAX(maxY, reinterpret_cast<const float*>(&screenPixelPosY)[i]);
	}

	entityBlock->mAABBMinScreenSpacePointX[entityIndex] = minX;
	entityBlock->mAABBMinScreenSpacePointY[entityIndex] = minY;

	entityBlock->mAABBMaxScreenSpacePointX[entityIndex] = maxX;
	entityBlock->mAABBMaxScreenSpacePointY[entityIndex] = maxY;
	

	// Compute min depth value

	aabbVertexZ = _mm256_blendv_ps(aabbVertexZ, _mm256_set1_ps(std::numeric_limits<float>::max()), isHomogeneousWNegative);
	float aabbMinDepthValue = std::numeric_limits<float>::max();
	for (size_t i = 0; i < 8; i++)
	{
		aabbMinDepthValue = MIN(aabbMinDepthValue, reinterpret_cast<const float*>(&aabbVertexZ)[i]);
	}

	entityBlock->mAABBMinNDCZ[entityIndex] = aabbMinDepthValue;

	const int isHomogeneousWNegativeMask = _mm256_movemask_ps(isHomogeneousWNegative);
	entityBlock->SetIsAllAABBClipPointWPositive(entityIndex, (isHomogeneousWNegativeMask == 0x00000000));
	entityBlock->SetIsAllAABBClipPointWNegative(entityIndex, (isHomogeneousWNegativeMask == 0x000000FF));

	// If All vertex's w of clip space aabb is negative, it should be culled!
	entityBlock->UpdateIsCulled(entityIndex, cameraIndex, isHomogeneousWNegativeMask == 0x000000FF);
}

void culling::PreCulling::DoPreCull
(
	const size_t cameraIndex,
	culling::EntityBlock* const entityBlock
)
{
	for(size_t entityIndex = 0 ; entityIndex < entityBlock->mCurrentEntityCount ; entityIndex++)
	{
		if(entityBlock->GetIsObjectEnabled(entityIndex) == false)
		{
			entityBlock->SetCulled(entityIndex, cameraIndex);
		}	
	}

	for (size_t entityIndex = 0; entityIndex < entityBlock->mCurrentEntityCount; entityIndex++)
	{
		if (entityBlock->GetIsObjectEnabled(entityIndex) == true)
		{
			entityBlock->UpdateBoundingSphereRadius(entityIndex);
		}
	}

	for (size_t entityIndex = 0; entityIndex < entityBlock->mCurrentEntityCount; entityIndex++)
	{
		if (entityBlock->GetIsObjectEnabled(entityIndex) == true)
		{
			ComputeScreenSpaceMinMaxAABBAndMinZ(cameraIndex, entityBlock, entityIndex);
		}
	}
}

culling::PreCulling::PreCulling(EveryCulling* frotbiteCullingSystem)
	: CullingModule(frotbiteCullingSystem)
{
}

void culling::PreCulling::CullBlockEntityJob(const size_t cameraIndex, const std::int32_t localThreadIndex, const unsigned long long currentTickCount)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlockForMultipleThreads(cameraIndex, localThreadIndex);

		if (nextEntityBlock != nullptr)
		{
			DoPreCull(cameraIndex, nextEntityBlock);
		}
		else
		{
			break;
		}
	}

}

const char* culling::PreCulling::GetCullingModuleName() const
{
	return "PreCulling";
}
