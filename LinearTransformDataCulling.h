#pragma once

#include "LinearTransformDataCulling_Core.h"
#include "DataStructure/EntityGridCell.h"
#include "DataStructure/EntityBlockViewer.h"

#include <memory>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>

#include <Vector4.h>
#include <Matrix4x4.h>

#include <Simple_SingleTon/Singleton.h>

namespace doom
{
	namespace graphics
	{
		struct SIMDFrustumPlanes
		{
			math::Vector4 mFrustumPlanes[8];
		};

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
		/// LinearTransformDataCulling mLinearTransformDataCulling;
		/// 
		/// std::vector<Camera> CameraList = ~~~~;
		// 	for (unsigned int i = 0; i < CameraList.size(); i++)
		// 	{
		// 		this->mLinearTransformDataCulling.UpdateFrustumPlane(i, CameraList[i]->GetViewProjectionMatrix());
		// 	}
		//
		//  this->mLinearTransformDataCulling.SetCameraCount(CameraList.size());
		// 	this->mLinearTransformDataCulling.ResetCullJobState();S
		// 	auto CullingBlockJobs = this->mLinearTransformDataCulling.GetCullBlockEnityJobs();
		//
		//	//Put pushing job to jobpool at FORMOST OF REDNERING LOOP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// 	JobSystem.PushBackJobChunkToJobPool(std::move(CullingBlockJobs));
		//
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//
		// 	this->mLinearTransformDataCulling.WaitToFinishCullJobs();
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
		class LinearTransformDataCulling
		{
		private:

			unsigned int mCameraCount;
			SIMDFrustumPlanes mSIMDFrustumPlanes[MAX_CAMERA_COUNT];

			bool bmIsInitializedEntityBlockPool{ false };
			std::unique_ptr<EntityBlock[]> mEntityBlockPool;
			std::vector<EntityBlock*> mFreeEntityBlockList{};
			EntityGridCell mEntityGridCell{};

			//static inline constexpr unsigned int M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
			/// <summary>
			/// will be used at CullBlockEntityJob
			/// </summary>
			//std::atomic<unsigned int> mAtomicCurrentBlockIndex;
			unsigned int mFinishedCullJobBlockCount;

			std::vector<std::function<void()>> mCachedCullBlockEntityJobs{};

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

			LinearTransformDataCulling();

		

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
			/// You should call this function on your Transform Component or In your game engine
			///
			/// increment EntityCountInBlock of TargetBlock.
			/// If All blocks is full, Get new block from Block Pool
			/// 
			/// Allocating New Entity isn't thread safe
			/// </summary>
			EntityBlockViewer AllocateNewEntity(const math::Vector3& position, float radius);


			// ////////////////////////////////////////////
			

			
			/// <summary>
			/// Solve View Frustum Culling from multiple threads
			/// Call this before start draw
			/// 
			/// for pushing to job pool , this function is declared with static
			/// 
			/// CullBlockEntityJob never access to shared variable.
			/// So CullBlockEntityJob is thread safe.
			/// </summary>
			void CullBlockEntityJob(unsigned int blockIndex);
			
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
			FORCE_INLINE std::vector<std::function<void()>> GetCullBlockEnityJobs()
			{
				std::vector<std::function<void()>> cullJobs{};
				cullJobs.assign(this->mCachedCullBlockEntityJobs.begin(), this->mCachedCullBlockEntityJobs.begin() + this->mEntityGridCell.mBlockCount);
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
			bool WaitToFinishCullJobs();

		
			/// <summary>
			/// Reset cull job state before pushing cull jobs to job pool
			/// </summary>
			void ResetCullJobState();
			/// <summary>
			/// before Start solving culling, Update Every Camera's frustum plane
			/// Do this at main thread
			/// </summary>
			/// <param name="frustumPlaneIndex"></param>
			/// <param name="viewProjectionMatix"></param>
			void UpdateFrustumPlane(unsigned int frustumPlaneIndex, const math::Matrix4x4& viewProjectionMatix);

			void SetCameraCount(unsigned int cameraCount);
			SIMDFrustumPlanes* GetSIMDPlanes();
		};
	}
}



