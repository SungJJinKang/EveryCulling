#pragma once

#include "FrotbiteCullingSystemCore.h"
#include "DataStructure/EntityGridCell.h"
#include "DataStructure/EntityBlockViewer.h"

#include <array>
#include <memory>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>

#include <Vector4.h>
#include <Matrix4x4.h>

#include "CullingModule/ViewFrustumCulling.h"

#ifndef DISABLE_SCREEN_SAPCE_AABB_CULLING
#include "CullingModule/ScreenSpaceAABBCulling.h"
#endif

namespace culling
{


	/// <summary>
	/// This is implementation of Data Oriented ViewFrustumCulling of Frostbite in 2011
	/// 
	/// This culling use SIMD DotProduct, So Check LightMath_Cpp/Matrix4x4Float_Aligned.inl
	///
	/// Feature :
	/// 1. Linear arrays scale great. All entity's position data is stored linearlly.
	/// 2. Tree based sturucture or other Acceleration strucutures isn;t required 
	/// 3. Frustum vs Sphere intersections check per loop
	/// 3. Threads solve intersections(blocks) parrallily
	/// 
	/// this library require math library : https://github.com/SungJJinKang/LightMath_Cpp/blob/main/Matrix4x4Float_Aligned.inl
	/// 
	/// Example : 
	/// 
	/// FrotbiteCullingSystem mFrotbiteCullingSystem;
	/// 
	/// std::vector<Camera> CameraList = ~~~~;
	// 	for (unsigned int i = 0; i < CameraList.size(); i++)
	// 	{
	// 		this->mFrotbiteCullingSystem.UpdateFrustumPlane(i, CameraList[i]->GetViewProjectionMatrix());
	// 	}
	//
	//  this->mFrotbiteCullingSystem.SetCameraCount(CameraList.size());
	// 	this->mFrotbiteCullingSystem.ResetCullJobState();S
	// 	auto CullingBlockJobs = this->mFrotbiteCullingSystem.GetCullBlockEnityJobs();
	//
	//	//Put pushing job to jobpool at FORMOST OF REDNERING LOOP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 	JobSystem.PushBackJobChunkToJobPool(std::move(CullingBlockJobs));
	//
	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// 	this->mFrotbiteCullingSystem.WaitToFinishCullJobs();
	// 
	//  for(unsigned int cameraIndex = 0 ; cameraIndex < MAX_CAMERA_COUNT ; cameraIndex++)
	// 	{
	// 		for (size_t i = 0; i < EntityCount; ++i)
	// 		{
	// 			if (Entity[i]->EntityBlockViewer.GetIsVisibleBitflag(cameraIndex) != 0) 
	// 			{
	// 				Entity[i]->Draw(); 
	// 			}
	// 		}
	// 	}
	// 	
	/// 
	/// 
	/// 
	/// 
	/// 
	/// references : 
	/// https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented
	/// https://www.slideshare.net/DICEStudio/culling-the-battlefield-data-oriented-design-in-practice
	/// 
	/// 
	/// https://macton.smugmug.com/Other/2008-07-15-by-Eye-Fi/n-xmKDH/
	/// 
	/// </summary>
	class FrotbiteCullingSystem
	{
		friend class ScreenSpaceAABBCulling;
		friend class ViewFrustumCulling;
	private:

		unsigned int mCameraCount;
		
		bool bmIsInitializedEntityBlockPool{ false };
		std::unique_ptr<EntityBlock[]> mEntityBlockPool;
		std::vector<EntityBlock*> mFreeEntityBlockList{};
		std::vector<EntityBlock*> mActiveEntityBlockList{};
		EntityGridCell mEntityGridCell{};

		//static inline constexpr unsigned int M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
		/// <summary>
		/// will be used at CullBlockEntityJob
		/// </summary>
		//std::atomic<unsigned int> mAtomicCurrentBlockIndex;
		std::atomic<unsigned int> mFinishedCullJobBlockCount;

		std::array<std::vector<std::function<void()>>, MAX_CAMERA_COUNT> mCachedCullBlockEntityJobs{};

		/// <summary>
		/// 
		/// </summary>
		std::condition_variable mCullJobConditionVaraible;
		std::mutex mCullJobMutex;

		void SetAllOneIsVisibleFlag();

		void RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock);

		void AllocateEntityBlockPool();
		EntityBlock* AllocateNewEntityBlockFromPool();

		/// <summary>
		/// Block Swap removedblock with last block, and return swapped lastblock to pool
		/// </summary>
		void FreeEntityBlock(EntityBlock* freedEntityBlock);
		EntityBlock* GetNewEntityBlockFromPool();

		void CacheCullBlockEntityJobs();

	public:

		ViewFrustumCulling mViewFrustumCulling;
#ifndef DISABLE_SCREEN_SAPCE_AABB_CULLING
		ScreenSpaceAABBCulling mScreenSpaceAABBCulling;
#endif
		FrotbiteCullingSystem();

		/// <summary>
		/// You should call this function on your Transform Component or In your game engine
		///
		/// increment EntityCountInBlock of TargetBlock.
		/// If All blocks is full, Get new block from Block Pool
		/// 
		/// Allocating New Entity isn't thread safe
		/// </summary>
		EntityBlockViewer AllocateNewEntity(void* renderer, const math::Vector3& position, float radius);

		/// <summary>
		/// You should call this function on your Transform Component or In your game engine
		/// 
		/// Remove Entity is nothing, Just decrement AllocatedEntityCountInBlocks
		/// And if AllocatedEntityCountInBlocks Of Block become zero, Remove the block using RemoveBlock function
		/// 
		/// Removing Entity isn't thread safe
		/// </summary>
		void RemoveEntityFromBlock(EntityBlockViewer& entityBlockViewer);

		/// <summary>
		/// Solve View Frustum Culling from multiple threads
		/// Call this before start draw
		/// 
		/// for pushing to job pool , this function is declared with static
		/// 
		/// CullBlockEntityJob never access to shared variable.
		/// So CullBlockEntityJob is thread safe.
		/// </summary>
		void CullBlockEntityJob(unsigned int blockIndex, unsigned int cameraIndex);

		/// <summary>
		/// Get Culling BlockEntity Jobs
		/// 
		/// Push returned all std::function to JobPool!!!!!!!!!
		/// Put pushing culljob to jobpool at foremost of rendering loop!!!!!!!!!!!!
		/// 
		/// return type is std::vector<std::function<void()>>
		/// 
		/// 
		/// </summary>
		/// <returns></returns>
		FORCE_INLINE std::vector<std::function<void()>> GetCullBlockEnityJobs(unsigned int cameraIndex)
		{
			std::vector<std::function<void()>> cullJobs{};
			cullJobs.assign(this->mCachedCullBlockEntityJobs[cameraIndex].begin(), this->mCachedCullBlockEntityJobs[cameraIndex].begin() + this->mEntityGridCell.mBlockCount);
			return cullJobs;
		}

		/// <summary>
		/// Get Is All block's culling job is finished.
		/// if true, you can check object's visibility(culled)
		/// </summary>
		/// <returns></returns>
		bool GetIsCullJobFinished();

		/// <summary>
		/// Caller thread will stall until cull job of all entity block is finished
		/// </summary>
		void WaitToFinishCullJobs();


		/// <summary>
		/// Reset cull job state before pushing cull jobs to job pool
		/// </summary>
		void ResetCullJobState();
	
		void SetCameraCount(unsigned int cameraCount);
		unsigned int GetCameraCount();

		std::vector<EntityBlock*> GetActiveEntityBlockList() const
		{
			return this->mActiveEntityBlockList;
		}
	};
}


