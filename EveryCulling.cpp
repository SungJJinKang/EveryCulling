#include "EveryCulling.h"

#include <cstring>
#include <chrono>

#include "DataType/EntityBlock.h"
#include <vector_erase_move_lastelement/vector_swap_popback.h>

#include "CullingModule/ViewFrustumCulling/ViewFrustumCulling.h"
#include "CullingModule/PreCulling/PreCulling.h"
#include "CullingModule/DistanceCulling/DistanceCulling.h"

#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
#include "CullingModule/ScreenSpaceBoundingSphereCulling/ScreenSpaceBoundingSphereCulling.h"
#endif

#include "CullingModule/MaskedSWOcclusionCulling/MaskedSWOcclusionCulling.h"

#ifdef ENABLE_QUERY_OCCLUSION
#include "CullingModule/QueryOcclusionCulling/QueryOcclusionCulling.h"
#endif

void culling::EveryCulling::FreeEntityBlock(EntityBlock* freedEntityBlock)
{
	assert(freedEntityBlock != nullptr);

	size_t freedEntityBlockIndex;
	const size_t entityBlockCount = mActiveEntityBlockList.size();
	bool IsSuccessToFind = false;
	for (size_t i = 0; i < entityBlockCount; i++)
	{
		//Freeing entity block happen barely
		//So this looping is acceptable
		if (mActiveEntityBlockList[i] == freedEntityBlock)
		{
			freedEntityBlockIndex = i;
			IsSuccessToFind = true;
			break;
		}
	}

	assert(IsSuccessToFind == true);

	mFreeEntityBlockList.push_back(freedEntityBlock);
	swap_popback::vector_find_swap_popback(mActiveEntityBlockList, freedEntityBlock);
}


culling::EntityBlock* culling::EveryCulling::GetNewEntityBlockFromPool()
{
	if (mFreeEntityBlockList.size() == 0)
	{
		AllocateEntityBlockPool();
	}

	assert(mFreeEntityBlockList.size() != 0);
	EntityBlock* entityBlock = mFreeEntityBlockList.back();
	mFreeEntityBlockList.pop_back();
	return entityBlock;
}

void culling::EveryCulling::ResetCullingModules()
{
	for (auto cullingModule : mUpdatedCullingModules)
	{
		cullingModule->ResetCullingModule();
	}
	
	mMaskedSWOcclusionCulling->ResetState();

}

void culling::EveryCulling::ResetEntityBlocks()
{
	//Maybe Compiler use SIMD or do faster than SIMD instruction
	for (auto entityBlock : mActiveEntityBlockList)
	{
		entityBlock->ResetEntityBlock();
	}
}

void culling::EveryCulling::AllocateEntityBlockPool()
{
	EntityBlock* newEntityBlockChunk = new EntityBlock[INITIAL_ENTITY_BLOCK_COUNT];
	for (std::uint32_t i = 0; i < INITIAL_ENTITY_BLOCK_COUNT; i++)
	{
		mFreeEntityBlockList.push_back(newEntityBlockChunk + i);
	}
	mAllocatedEntityBlockChunkList.push_back(newEntityBlockChunk);
}


void culling::EveryCulling::RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, std::uint32_t entityIndexInBlock)
{
	assert(ownerEntityBlock != nullptr);
	assert(entityIndexInBlock >= 0 && entityIndexInBlock < ENTITY_COUNT_IN_ENTITY_BLOCK);

	for(auto cullingModule : mUpdatedCullingModules)
	{
		cullingModule->ClearEntityData(ownerEntityBlock, entityIndexInBlock);
	}
	
	assert(ownerEntityBlock->mCurrentEntityCount != 0);
	ownerEntityBlock->mCurrentEntityCount--;
	if (ownerEntityBlock->mCurrentEntityCount == 0)
	{
		FreeEntityBlock(ownerEntityBlock);
	}
	
}

void culling::EveryCulling::CullBlockEntityJob(const size_t cameraIndex)
{
	const std::uint32_t entityBlockCount = static_cast<std::uint32_t>(GetActiveEntityBlockCount());
	if (entityBlockCount > 0)
	{
		for (size_t moduleIndex = 0; moduleIndex < mUpdatedCullingModules.size(); moduleIndex++)
		{
			culling::CullingModule* cullingModule = mUpdatedCullingModules[moduleIndex];
			assert(cullingModule != nullptr);

			OnStartCullingModule(cullingModule);

			if(cullingModule->IsEnabled == true)
			{
				cullingModule->ThreadCullJob(cameraIndex);

				std::atomic_thread_fence(std::memory_order_seq_cst);

				while (cullingModule->GetFinishedThreadCount(cameraIndex) < mThreadCount)
				{
					std::this_thread::yield();
				}
			}

			OnEndCullingModule(cullingModule);
		}


	}
}


void culling::EveryCulling::WaitToFinishCullJob(const std::uint32_t cameraIndex) const
{
	const CullingModule* lastEnabledCullingModule = GetLastEnabledCullingModule();
	if(lastEnabledCullingModule != nullptr)
	{
		while (lastEnabledCullingModule->GetFinishedThreadCount(cameraIndex) < mThreadCount)
		{
			std::this_thread::yield();
		}
	}
}

void culling::EveryCulling::WaitToFinishCullJobOfAllCameras() const
{
	for (std::uint32_t cameraIndex = 0; cameraIndex < mCameraCount; cameraIndex++)
	{
		WaitToFinishCullJob(cameraIndex);
	}
}

void culling::EveryCulling::ResetCullJobState()
{
	ResetEntityBlocks();
	ResetCullingModules();

	//release!
	std::atomic_thread_fence(std::memory_order_seq_cst);
}

const culling::CullingModule* culling::EveryCulling::GetLastEnabledCullingModule() const
{
	culling::CullingModule* lastEnabledCullingModule = nullptr;
	for(int cullingModuleIndex = mUpdatedCullingModules.size() - 1 ; cullingModuleIndex >= 0 ; cullingModuleIndex--)
	{
		if(mUpdatedCullingModules[cullingModuleIndex]->IsEnabled == true)
		{
			lastEnabledCullingModule = mUpdatedCullingModules[cullingModuleIndex];
			break;
		}
	}
	return lastEnabledCullingModule;
}

void culling::EveryCulling::SetEnabledCullingModule(const CullingModuleType cullingModuleType, const bool isEnabled)
{
	switch (cullingModuleType)
	{

	case CullingModuleType::_ViewFrustumCulling:

		mViewFrustumCulling->IsEnabled = isEnabled;

		break;


	case CullingModuleType::_MaskedSWOcclusionCulling:

		mMaskedSWOcclusionCulling->mSolveMeshRoleStage.IsEnabled = isEnabled;
		mMaskedSWOcclusionCulling->mBinTrianglesStage.IsEnabled = isEnabled;
		mMaskedSWOcclusionCulling->mRasterizeTrianglesStage.IsEnabled = isEnabled;
		mMaskedSWOcclusionCulling->mQueryOccludeeStage.IsEnabled = isEnabled;

		break;

	case CullingModuleType::_DistanceCulling:

		mDistanceCulling->IsEnabled = isEnabled;

		break;

#ifdef ENABLE_QUERY_OCCLUSION
	case CullingModuleType::_HwQueryOcclusionCulling:

		mQueryOcclusionCulling->IsEnabled = isEnabled;

		break;
#endif
	}
}

culling::EntityBlock* culling::EveryCulling::AllocateNewEntityBlockFromPool()
{
	EntityBlock* const newEntityBlock = GetNewEntityBlockFromPool();
	newEntityBlock->ClearEntityBlock();

	mActiveEntityBlockList.push_back(newEntityBlock);

	return newEntityBlock;
}





culling::EntityBlockViewer culling::EveryCulling::AllocateNewEntity()
{
	culling::EntityBlock* targetEntityBlock;
	if (mActiveEntityBlockList.size() == 0)
	{
		// if Any entityBlock isn't allocated yet
		targetEntityBlock = AllocateNewEntityBlockFromPool();
	}
	else
	{//When Allocated entity block count is at least one

		//Get last entityblock in active entities
		targetEntityBlock = { mActiveEntityBlockList.back() };

		if (targetEntityBlock->mCurrentEntityCount == ENTITY_COUNT_IN_ENTITY_BLOCK)
		{
			//if last entityblock in active entities is full of entities
			//alocate new entity block
			targetEntityBlock = AllocateNewEntityBlockFromPool();
		}
	}

	assert(targetEntityBlock->mCurrentEntityCount <= ENTITY_COUNT_IN_ENTITY_BLOCK); // something is weird........
	
	targetEntityBlock->mCurrentEntityCount++;
	
	return EntityBlockViewer(targetEntityBlock, targetEntityBlock->mCurrentEntityCount - 1);
}

void culling::EveryCulling::RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer)
{
	//Do nothing......

	entityBlockViewer.bmIsActive = false;
	RemoveEntityFromBlock(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock);
	//Don't decrement mEntityGridCell.AllocatedEntityCountInBlocks
	//Entities Indexs in EntityBlock should not be swapped because already allocated EntityBlockViewer can't see it
}

culling::EveryCulling::EveryCulling(const std::uint32_t resolutionWidth, const std::uint32_t resolutionHeight)
	:
	mPreCulling{ std::make_unique<PreCulling>(this) },
	mDistanceCulling{ std::make_unique<DistanceCulling>(this) },
	mViewFrustumCulling{ std::make_unique<ViewFrustumCulling>(this) }
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	, mScreenSpaceBoudingSphereCulling{ std::make_unique<ScreenSpaceBoundingSphereCulling>(this) }
#endif
	, mMaskedSWOcclusionCulling{ std::make_unique<MaskedSWOcclusionCulling>(this, resolutionWidth, resolutionHeight) }
#ifdef ENABLE_QUERY_OCCLUSION
	, mQueryOcclusionCulling{ std::make_unique<QueryOcclusionCulling>(this) }
#endif
	, mUpdatedCullingModules
		{
			mPreCulling.get(),
			mDistanceCulling.get(),
			mViewFrustumCulling.get(),
	#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
			& (mScreenSpaceBoudingSphereCulling),
	#endif	
			mMaskedSWOcclusionCulling.get(), // Choose Role Stage
			&(mMaskedSWOcclusionCulling->mSolveMeshRoleStage), // Choose Role Stage
			&(mMaskedSWOcclusionCulling->mBinTrianglesStage), // BinTriangles
			&(mMaskedSWOcclusionCulling->mRasterizeTrianglesStage), // DrawOccluderStage
			&(mMaskedSWOcclusionCulling->mQueryOccludeeStage) // QueryOccludeeStage
		}
#ifdef PROFILING_CULLING
	, mEveryCullingProfiler{}
#endif
{
	//to protect 
	mFreeEntityBlockList.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);
	mActiveEntityBlockList.reserve(INITIAL_ENTITY_BLOCK_RESERVED_SIZE);

	AllocateEntityBlockPool();

	//CacheCullBlockEntityJobs();
	bmIsEntityBlockPoolInitialized = true;

	for(size_t cameraIndex = 0 ; cameraIndex < MAX_CAMERA_COUNT ; cameraIndex++)
	{
		mSortedEntityInfo[cameraIndex].resize(2);
	}
}


culling::EveryCulling::~EveryCulling()
{
	for (culling::EntityBlock* allocatedEntityBlockChunk : mAllocatedEntityBlockChunkList)
	{
		delete[] allocatedEntityBlockChunk;
	}
}

void culling::EveryCulling::SetCameraCount(const size_t cameraCount)
{
	mCameraCount = cameraCount;

	for (auto updatedCullingModule : mUpdatedCullingModules)
	{
		updatedCullingModule->OnSetCameraCount(cameraCount);
	}
}

void culling::EveryCulling::SetThreadCount(const size_t threadCount)
{
	mThreadCount = threadCount;
}

FORCE_INLINE void culling::EveryCulling::OnStartCullingModule(const culling::CullingModule* const cullingModule)
{
#ifdef PROFILING_CULLING
	mEveryCullingProfiler.SetStartTime(cullingModule->GetCullingModuleName());
#endif
}

FORCE_INLINE void culling::EveryCulling::OnEndCullingModule(const culling::CullingModule* const cullingModule)
{
#ifdef PROFILING_CULLING
	mEveryCullingProfiler.SetEndTime(cullingModule->GetCullingModuleName());
#endif
}

void culling::EveryCulling::SetViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& viewProjectionMatrix)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	IS_ALIGNED_ASSERT(reinterpret_cast<size_t>(&viewProjectionMatrix), 32);

	mCameraViewProjectionMatrixes[cameraIndex] = viewProjectionMatrix;
	
	if (cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetViewProjectionMatrix(cameraIndex, viewProjectionMatrix);
		}
	}
}

void culling::EveryCulling::SetFieldOfViewInDegree(const size_t cameraIndex, const float fov)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
	assert(fov > 0.0f);

	mCameraFieldOfView[cameraIndex] = fov;

	if (cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraFieldOfView(cameraIndex, fov);
		}
	}
}

void culling::EveryCulling::SetCameraNearFarClipPlaneDistance
(
	const size_t cameraIndex,
	const float nearPlaneDistance, 
	const float farPlaneDistance
)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
	assert(nearPlaneDistance > 0.0f);
	assert(farPlaneDistance > 0.0f);
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);


	mNearClipPlaneDistance[cameraIndex] = nearPlaneDistance;
	mFarClipPlaneDistance[cameraIndex] = farPlaneDistance;

	if (cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraNearClipPlaneDistance(cameraIndex, nearPlaneDistance);
			updatedCullingModule->OnSetCameraFarClipPlaneDistance(cameraIndex, farPlaneDistance);
		}
	}
}

void culling::EveryCulling::SetCameraWorldPosition(const size_t cameraIndex, const culling::Vec3& cameraWorldPos)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	mCameraWorldPositions[cameraIndex] = cameraWorldPos;

	if (cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraWorldPosition(cameraIndex, cameraWorldPos);
		}
	}
}

void culling::EveryCulling::SetCameraRotation(const size_t cameraIndex, const culling::Vec4& cameraRotation)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	mCameraRotations[cameraIndex] = cameraRotation;

	if (cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraRotation(cameraIndex, cameraRotation);
		}
	}
}

void culling::EveryCulling::Configure(const size_t cameraIndex, const SettingParameters& settingParameters)
{
	SetViewProjectionMatrix(cameraIndex, settingParameters.mViewProjectionMatrix);
	SetFieldOfViewInDegree(cameraIndex, settingParameters.mFieldOfViewInDegree);
	SetCameraNearFarClipPlaneDistance(cameraIndex, settingParameters.mCameraNearPlaneDistance, settingParameters.mCameraFarPlaneDistance);
	SetCameraWorldPosition(cameraIndex, settingParameters.mCameraWorldPosition);
	SetCameraRotation(cameraIndex, settingParameters.mCameraRotation);
}

const std::vector<culling::EntityBlock*>& culling::EveryCulling::GetActiveEntityBlockList() const
{
	return mActiveEntityBlockList;
}

size_t culling::EveryCulling::GetActiveEntityBlockCount() const
{
	return GetActiveEntityBlockList().size();
}

void culling::EveryCulling::ResetSortedEntityCount()
{
	mSortedEntityCount = 0;
}

std::vector<culling::EntityInfoInEntityBlock>& culling::EveryCulling::GetSortedEntityInfo
(
	const size_t cameraIndex
)
{
	return mSortedEntityInfo[cameraIndex];
}
