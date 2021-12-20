#pragma once

#include "EveryCullingCore.h"
#include "DataType/EntityGridCell.h"
#include "DataType/EntityBlockViewer.h"

#include <array>
#include <vector>
#include <atomic>
#include <functional>
#include <thread>



namespace culling
{
	class CullingModule;
	class ViewFrustumCulling;
	class ScreenSpaceBoundingSphereCulling;
	class MaskedSWOcclusionCulling;
	class QueryOcclusionCulling;
	/// <summary>
	/// 
	/// This is implementation of Data Oriented ViewFrustumCulling of Frostbite in 2011
	///
	/// [Slide Resource](https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice)        
	/// [GDC Talk Video](https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented)          
	/// [�ѱ��� ���α� ��] (https ://sungjjinkang.github.io/doom/2021/04/02/viewfrustumculling.html)  
	///
	/// 
	/// This culling use SIMD DotProduct, So Check LightMath_Cpp/Mat4x4Float_Aligned.inl
	///
	/// Feature :
	/// 1. Linear arrays scale great. All entity's position data is stored linearlly.
	/// 2. Tree based sturucture or other Acceleration strucutures isn;t required 
	/// 3. Frustum vs Sphere intersections check per loop
	/// 3. Threads solve intersections(blocks) parrallily
	/// 
	/// this library require math library : https://github.com/SungJJinKang/LightMath_Cpp/blob/main/Mat4x4Float_Aligned.inl
	/// 
	/// references : 
	/// https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented
	/// https://www.slideshare.net/DICEStudio/culling-the-battlefield-data-oriented-design-in-practice
	/// https://macton.smugmug.com/Other/2008-07-15-by-Eye-Fi/n-xmKDH/
	/// 
	/// </summary>
	class EveryCulling
	{
	private:

		size_t mThreadCount;
		size_t mCameraCount;
		std::array<culling::Mat4x4, MAX_CAMERA_COUNT> mCameraModelMatrixes;
		std::array<culling::Mat4x4, MAX_CAMERA_COUNT> mCameraViewProjectionMatrixes;
		std::array<culling::Vec3, MAX_CAMERA_COUNT> mCameraWorldPositions;
		std::array<culling::Vec4, MAX_CAMERA_COUNT> mCameraRotations;
		std::array<float, MAX_CAMERA_COUNT> mCameraFieldOfView;
		std::array<float, MAX_CAMERA_COUNT> mFarClipPlaneDistance;
		std::array<float, MAX_CAMERA_COUNT> mNearClipPlaneDistance;

		bool bmIsEntityBlockPoolInitialized{ false };

		/// <summary>
		/// List of EntityBlock with no entity ( maybe entity was destroyed)
		/// </summary>
		std::vector<EntityBlock*> mFreeEntityBlockList{};
		/// <summary>
		/// List of EntityBlock containing Entities
		/// </summary>
		std::vector<EntityBlock*> mActiveEntityBlockList{};
		/// <summary>
		/// Allocated EntityBlock Arrays
		/// This objects will be released at destructor
		/// </summary>
		std::vector<EntityBlock*> mAllocatedEntityBlockChunkList{};

		std::atomic<bool> mIsCullJobFinished;

		void AllocateEntityBlockPool();
		void ResetEntityBlock(culling::EntityBlock* entityBlock);
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
		void SetAllOneIsVisibleFlag();

	public:

		std::unique_ptr<ViewFrustumCulling> mViewFrustumCulling;
#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
		std::unique_ptr<ScreenSpaceBoundingSphereCulling> mScreenSpaceBoudingSphereCulling;
#endif
		std::unique_ptr<MaskedSWOcclusionCulling> mMaskedSWOcclusionCulling;
#ifdef ENABLE_QUERY_OCCLUSION
		std::unique_ptr<QueryOcclusionCulling> mQueryOcclusionCulling;
#endif

	private:

		std::vector<culling::CullingModule*> mUpdatedCullingModules;

		
		void SetViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& viewProjectionMatrix);
		void SetFieldOfViewInDegree(const size_t cameraIndex, const float fov);
		void SetCameraNearFarClipPlaneDistance(const size_t cameraIndex, const float nearPlaneDistance, const float farPlaneDistance);;
		void SetCameraWorldPosition(const size_t cameraIndex, const culling::Vec3& cameraWorldPos);
		void SetCameraRotation(const size_t cameraIndex, const culling::Vec4& cameraRotation);
		

	public:

		EveryCulling() = delete;
		EveryCulling(const std::uint32_t resolutionWidth, const std::uint32_t resolutionHeight);
		EveryCulling(const EveryCulling&) = delete;
		EveryCulling(EveryCulling&&) noexcept = delete;
		EveryCulling& operator=(const EveryCulling&) = delete;
		EveryCulling& operator=(EveryCulling&&) noexcept = delete;

		~EveryCulling();

		void SetCameraCount(const size_t cameraCount);
		void SetThreadCount(const size_t threadCount);

		struct SettingParameters
		{
			culling::Mat4x4 mViewProjectionMatrix;
			float mFieldOfViewInDegree;
			float mCameraNearPlaneDistance;
			float mCameraFarPlaneDistance;
			culling::Vec3 mCameraWorldPosition;
			culling::Vec4 mCameraRotation;
		};

		void Configure(const size_t cameraIndex, const SettingParameters settingParameters);

		FORCE_INLINE size_t GetCameraCount() const
		{
			return mCameraCount;
		}
		FORCE_INLINE const culling::Vec3& GetCameraWorldPosition(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mCameraWorldPositions[cameraIndex];
		}
		FORCE_INLINE const culling::Mat4x4& GetCameraModelMatrix(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mCameraModelMatrixes[cameraIndex];
		}
		FORCE_INLINE const culling::Mat4x4& GetCameraViewProjectionMatrix(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mCameraViewProjectionMatrixes[cameraIndex];
		}
		FORCE_INLINE float GetCameraFieldOfView(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mCameraFieldOfView[cameraIndex];
		}
		FORCE_INLINE float GetCameraFarClipPlaneDistance(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
			return mFarClipPlaneDistance[cameraIndex];
		}
		FORCE_INLINE float GetCameraNearClipPlaneDistance(const size_t cameraIndex) const
		{
			assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);
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
		EntityBlockViewer AllocateNewEntity(void* renderer, void* transform);

		/// <summary>
		/// You should call this function on your Transform Component or In your game engine
		/// 
		/// Remove Entity is nothing, Just decrement AllocatedEntityCountInBlocks
		/// And if AllocatedEntityCountInBlocks Of Block become zero, Remove the block using RemoveBlock function
		/// 
		/// Removing Entity isn't thread safe
		/// </summary>
		void RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer);
		
		void CullBlockEntityJob(const size_t cameraIndex);
		//void CullBlockEntityJob(const std::uint32_t threadIndex, const std::uint32_t threadCount);

		/// <summary>
		/// Caller thread will stall until cull job of all entity block is finished
		/// </summary>
		void WaitToFinishCullJob(const std::uint32_t cameraIndex) const;
		void WaitToFinishCullJobOfAllCameras() const;

		/// <summary>
		/// Reset cull job state before pushing cull jobs to job pool
		/// </summary>
		void ResetCullJobState();
		
		constexpr auto GetCullJobInLambda(const size_t cameraIndex)
		{
			return [this, cameraIndex]()
			{
				this->CullBlockEntityJob(cameraIndex);
			};
		}
	};
}


