#pragma once

#include "EveryCullingCore.h"

#include "DataType/EntityGridCell.h"
#include "DataType/EntityBlockViewer.h"
#include "DataType/Math/Vector.h"
#include "DataType/Math/Matrix.h"


#ifdef EVERYCULLING_PROFILING_CULLING
#include "EveryCullingProfiler.h"
#endif

#include <array>
#include <vector>
#include <memory>

namespace culling
{
	class CullingModule;
	class ViewFrustumCulling;
	class ScreenSpaceBoundingSphereCulling;
	class MaskedSWOcclusionCulling;
	class QueryOcclusionCulling;
	class PreCulling;
	class DistanceCulling;
	struct EntityBlock;

	class EveryCulling
	{
	private:

		std::atomic<std::uint32_t> mRunningThreadCount;
		
		size_t mCameraCount;
		std::array<culling::Mat4x4, EVERYCULLING_MAX_CAMERA_COUNT> mCameraModelMatrixes;
		std::array<culling::Mat4x4, EVERYCULLING_MAX_CAMERA_COUNT> mCameraViewProjectionMatrixes;
		std::array<culling::Vec3, EVERYCULLING_MAX_CAMERA_COUNT> mCameraWorldPositions;
		std::array<culling::Vec4, EVERYCULLING_MAX_CAMERA_COUNT> mCameraRotations;
		std::array<float, EVERYCULLING_MAX_CAMERA_COUNT> mCameraFieldOfView;
		std::array<float, EVERYCULLING_MAX_CAMERA_COUNT> mFarClipPlaneDistance;
		std::array<float, EVERYCULLING_MAX_CAMERA_COUNT> mNearClipPlaneDistance;

		bool bmIsEntityBlockPoolInitialized;

		/// <summary>
		/// List of EntityBlock with no entity ( maybe entity was destroyed)
		/// </summary>
		std::vector<EntityBlock*> mFreeEntityBlockList;
		/// <summary>
		/// List of EntityBlock containing Entities
		/// </summary>
		std::vector<EntityBlock*> mActiveEntityBlockList;
		/// <summary>
		/// Allocated EntityBlock Arrays
		/// This objects will be released at destructor
		/// </summary>
		std::vector<EntityBlock*> mAllocatedEntityBlockChunkList;
		
		void AllocateEntityBlockPool();
		culling::EntityBlock* AllocateNewEntityBlockFromPool();
		void RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, std::uint32_t entityIndexInBlock);
		/// <summary>
		/// Block Swap removedblock with last block, and return swapped lastblock to pool
		/// </summary>
		void FreeEntityBlock(EntityBlock* freedEntityBlock);
		EntityBlock* GetNewEntityBlockFromPool();

		void ResetCullingModules();
		/// <summary>
		/// Reset VisibleFlag
		/// </summary>
		void ResetEntityBlocks();

	public:

		std::unique_ptr<PreCulling> mPreCulling;
		std::unique_ptr<DistanceCulling> mDistanceCulling;
		std::unique_ptr<ViewFrustumCulling> mViewFrustumCulling;
		std::unique_ptr<MaskedSWOcclusionCulling> mMaskedSWOcclusionCulling;

#ifdef EVERYCULLING_PROFILING_CULLING
		EveryCullingProfiler mEveryCullingProfiler;
#endif

	private:

		unsigned long long mCurrentTickCount;

		std::vector<culling::CullingModule*> mUpdatedCullingModules;

		// this function is called by multiple threads
		void OnStartCullingModule(const culling::CullingModule* const cullingModule);
		// this function is called by multiple threads
		void OnEndCullingModule(const culling::CullingModule* const cullingModule);
		
		void SetViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& viewProjectionMatrix);
		void SetFieldOfViewInDegree(const size_t cameraIndex, const float fov);
		void SetCameraNearFarClipPlaneDistance(const size_t cameraIndex, const float nearPlaneDistance, const float farPlaneDistance);;
		void SetCameraWorldPosition(const size_t cameraIndex, const culling::Vec3& cameraWorldPos);
		void SetCameraRotation(const size_t cameraIndex, const culling::Vec4& cameraRotation);

	public:

		enum class CullingModuleType
		{
			_PreCulling,
			_ViewFrustumCulling,
			_MaskedSWOcclusionCulling,
			_DistanceCulling
		};

		EveryCulling() = delete;
		EveryCulling(const std::uint32_t resolutionWidth, const std::uint32_t resolutionHeight);
		EveryCulling(const EveryCulling&) = delete;
		EveryCulling& operator=(const EveryCulling&) = delete;

		~EveryCulling();

		void SetCameraCount(const size_t cameraCount);

		unsigned long long GetTickCount() const;
		
		struct GlobalDataForCullJob
		{
			culling::Mat4x4 mViewProjectionMatrix;
			float mFieldOfViewInDegree;
			float mCameraNearPlaneDistance;
			float mCameraFarPlaneDistance;
			culling::Vec3 mCameraWorldPosition;
			culling::Vec4 mCameraRotation;
		};

		/**
		 * \brief Update global data for cull job. Should be called every frame.
		 * \param cameraIndex 
		 * \param settingParameters 
		 */
		void UpdateGlobalDataForCullJob(const size_t cameraIndex, const GlobalDataForCullJob& settingParameters);

		EVERYCULLING_FORCE_INLINE size_t GetCameraCount() const
		{
			return mCameraCount;
		}
		EVERYCULLING_FORCE_INLINE const culling::Vec3& GetCameraWorldPosition(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraWorldPositions[cameraIndex];
		}
		EVERYCULLING_FORCE_INLINE const culling::Mat4x4& GetCameraModelMatrix(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraModelMatrixes[cameraIndex];
		}
		EVERYCULLING_FORCE_INLINE const culling::Mat4x4& GetCameraViewProjectionMatrix(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraViewProjectionMatrixes[cameraIndex];
		}
		EVERYCULLING_FORCE_INLINE float GetCameraFieldOfView(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraFieldOfView[cameraIndex];
		}
		EVERYCULLING_FORCE_INLINE float GetCameraFarClipPlaneDistance(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mFarClipPlaneDistance[cameraIndex];
		}
		EVERYCULLING_FORCE_INLINE float GetCameraNearClipPlaneDistance(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mNearClipPlaneDistance[cameraIndex];
		}
		
		/// <summary>
		/// Get EntityBlock List with entities
		/// </summary>
		/// <returns></returns>
		const std::vector<EntityBlock*>& GetActiveEntityBlockList() const;
		size_t GetActiveEntityBlockCount() const;

		/// <summary>
		/// You should call this function on your Transform Component or In your game engine
		///
		/// increment EntityCountInBlock of TargetBlock.
		/// If All blocks is full, Get new block from Block Pool
		/// 
		/// Allocating New Entity isn't thread safe
		/// </summary>
		EntityBlockViewer AllocateNewEntity();

		/// <summary>
		/// You should call this function on your Transform Component or In your game engine
		/// 
		/// Remove Entity is nothing, Just decrement AllocatedEntityCountInBlocks
		/// And if AllocatedEntityCountInBlocks Of Block become zero, Remove the block using RemoveBlock function
		/// 
		/// Removing Entity isn't thread safe
		/// </summary>
		void RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer);
		
		void ThreadCullJob(const size_t cameraIndex, const unsigned long long tickCount);
		//void ThreadCullJob(const std::uint32_t threadIndex, const std::uint32_t threadCount);

		/// <summary>
		/// Caller thread will stall until cull job of all entity block is finished
		/// </summary>
		void WaitToFinishCullJob(const std::uint32_t cameraIndex) const;
		void WaitToFinishCullJobOfAllCameras() const;


		/**
		 * \brief Reset cull job. Should be called every frame after finish cull job
		 */
		void PreCullJob();
		
		auto GetThreadCullJob(const size_t cameraIndex, const unsigned long long tickCount)
		{
			return [this, cameraIndex, tickCount]()
			{
				this->ThreadCullJob(cameraIndex, tickCount);
			};
		}

		const culling::CullingModule* GetLastEnabledCullingModule() const;
		void SetEnabledCullingModule(const CullingModuleType cullingModuleType, const bool isEnabled);
		std::uint32_t GetRunningThreadCount() const;

	};
}


