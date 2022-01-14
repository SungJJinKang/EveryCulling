#include "DistanceCulling.h"

static_assert(culling::ENTITY_COUNT_IN_ENTITY_BLOCK % 2 == 0);

void culling::DistanceCulling::DoDistanceCulling
(
	const size_t cameraIndex, 
	culling::EntityBlock* const entityBlock
)
{
	const culling::Vec3 cameraWorldPos = mCullingSystem->GetCameraWorldPosition(cameraIndex);
	const culling::M256F vectorizedCameraWorldPos = _mm256_setr_ps(cameraWorldPos.x, cameraWorldPos.y, cameraWorldPos.z, 0.0f, cameraWorldPos.x, cameraWorldPos.y, cameraWorldPos.z, 0.0f);

	const size_t entityCount = entityBlock->mCurrentEntityCount;
	for(size_t entityIndex = 0 ; entityIndex < entityCount ; entityIndex += 2)
	{
		const culling::M256F& vectorizedEntityWolrdPos = *reinterpret_cast<const culling::M256F*>(entityBlock->mWorldPositionAndWorldBoundingSphereRadius + entityIndex); // Bounding Sphere's Radius is located at index 4, 7

		const culling::M256F subOfCameraWorldPosAndEntityPos = _mm256_sub_ps(vectorizedCameraWorldPos, vectorizedEntityWolrdPos);
		const culling::M256F sqrOfSubOfCameraWorldPosAndEntityPos = _mm256_mul_ps(subOfCameraWorldPosAndEntityPos, subOfCameraWorldPosAndEntityPos);

		const float sqrDistanceOfFirstEntity = reinterpret_cast<const float*>(&sqrOfSubOfCameraWorldPosAndEntityPos)[0] + reinterpret_cast<const float*>(&sqrOfSubOfCameraWorldPosAndEntityPos)[1] + reinterpret_cast<const float*>(&sqrOfSubOfCameraWorldPosAndEntityPos)[2];
		const float sqrDistanceOfSecondEntity = reinterpret_cast<const float*>(&sqrOfSubOfCameraWorldPosAndEntityPos)[4] + reinterpret_cast<const float*>(&sqrOfSubOfCameraWorldPosAndEntityPos)[5] + reinterpret_cast<const float*>(&sqrOfSubOfCameraWorldPosAndEntityPos)[6];

		entityBlock->UpdateIsCulled(entityIndex, cameraIndex, sqrDistanceOfFirstEntity > entityBlock->GetDesiredMaxDrawDistance(entityIndex) * entityBlock->GetDesiredMaxDrawDistance(entityIndex));
		entityBlock->UpdateIsCulled(entityIndex + 1, cameraIndex, sqrDistanceOfSecondEntity > entityBlock->GetDesiredMaxDrawDistance(entityIndex + 1) * entityBlock->GetDesiredMaxDrawDistance(entityIndex + 1));
	}

	
}

culling::DistanceCulling::DistanceCulling(EveryCulling* const everyCulling)
	: CullingModule(everyCulling)
{
}

const char* culling::DistanceCulling::GetCullingModuleName() const
{
	return "DistanceCulling";
}

void culling::DistanceCulling::CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);;

		if (nextEntityBlock != nullptr)
		{
			DoDistanceCulling(cameraIndex, nextEntityBlock);
		}
		else
		{
			break;
		}
	}
}
