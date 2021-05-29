#pragma once

#include "EveryCullingCore.h"
#include "DataType/EntityGridCell.h"
#include "DataType/EntityBlockViewer.h"

#include <array>
#include <memory>
#include <vector>
#include <atomic>
#include <functional>
#include <thread>

#include "CullingModule/ViewFrustumCulling/ViewFrustumCulling.h"

#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
#include "CullingModule/ScreenSpaceBoundingSphereCulling/ScreenSpaceBoundingSphereCulling.h"
#endif

#include "CullingModule/MaskedSWOcclusionCulling/MaskedSWOcclusionCulling.h"

#ifdef ENABLE_QUERY_OCCLUSION
#include "CullingModule/QueryOcclusionCulling/QueryOcclusionCulling.h"
#endif

namespace culling
{
	/// <summary>
	/// 
	/// This is implementation of Data Oriented ViewFrustumCulling of Frostbite in 2011
	///
	/// [Slide Resource](https://www.ea.com/frostbite/news/culling-the-battlefield-data-oriented-design-in-practice)        
	/// [GDC Talk Video](https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented)          
	/// [한국어 블로그 글] (https ://sungjjinkang.github.io/doom/2021/04/02/viewfrustumculling.html)  
	///
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
	/// references : 
	/// https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented
	/// https://www.slideshare.net/DICEStudio/culling-the-battlefield-data-oriented-design-in-practice
	/// https://macton.smugmug.com/Other/2008-07-15-by-Eye-Fi/n-xmKDH/
	/// 
	/// </summary>
	class EveryCulling
	{
		friend class ScreenSpaceBoundingSphereCulling;
		friend class ViewFrustumCulling;
		friend class QueryOcclusionCulling;

	private:

		unsigned int mCameraCount;
		
		bool bmIsEntityBlockPoolInitialized{ false };

		/// <summary>
		/// List of EntityBlock with no Entity ( maybe entity was destroyed)
		/// </summary>
		std::vector<EntityBlock*> mFreeEntityBlockList{};
		/// <summary>
		/// List of EntityBlock with Entities
		/// </summary>
		std::vector<EntityBlock*> mActiveEntityBlockList{};
		/// <summary>
		/// Allocated EntityBlock Arrays
		/// This objects will be released at destructor
		/// </summary>
		std::vector<EntityBlock*> mAllocatedEntityBlockChunkList{};
		EntityGridCell mEntityGridCell{};

		std::atomic<bool> mIsCullJobFinished;

		culling::Matrix4X4 mViewProjectionMatrix;

		void AllocateEntityBlockPool();
		void ResetEntityBlock(culling::EntityBlock* entityBlock);
		std::pair<culling::EntityBlock*, unsigned int*> AllocateNewEntityBlockFromPool();
		void RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock);
		/// <summary>
		/// Block Swap removedblock with last block, and return swapped lastblock to pool
		/// </summary>
		void FreeEntityBlock(EntityBlock* freedEntityBlock);
		EntityBlock* GetNewEntityBlockFromPool();

		void ResetCullJobStateVariable();
		/// <summary>
		/// Reset VisibleFlag
		/// </summary>
		void SetAllOneIsVisibleFlag();

	public:

		ViewFrustumCulling mViewFrustumCulling;
#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
		ScreenSpaceBoundingSphereCulling mScreenSpaceBoudingSphereCulling;
#endif
		MaskedSWOcclusionCulling mMaskedSWOcclusionCulling;
#ifdef ENABLE_QUERY_OCCLUSION
		QueryOcclusionCulling mQueryOcclusionCulling;
#endif

	private:

		std::array<culling::CullingModule*, 
#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
			3
#else
			2
#endif
		> mUpdatedCullingModules
		{
			&(this->mViewFrustumCulling),
#ifdef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
			&(this->mScreenSpaceBoudingSphereCulling),
#endif	
			&(this->mMaskedSWOcclusionCulling)
		};

	public:

		EveryCulling(unsigned int resolutionWidth, unsigned int resolutionHeight);
		~EveryCulling();
		void SetCameraCount(unsigned int cameraCount);
		void SetViewProjectionMatrix(const culling::Matrix4X4& viewProjectionMatrix);
		unsigned int GetCameraCount() const;
		/// <summary>
		/// Get EntityBlock List with entities
		/// </summary>
		/// <returns></returns>
		std::vector<EntityBlock*> GetActiveEntityBlockList() const;

		/// <summary>
		/// You should call this function on your Transform Component or In your game engine
		///
		/// increment EntityCountInBlock of TargetBlock.
		/// If All blocks is full, Get new block from Block Pool
		/// 
		/// Allocating New Entity isn't thread safe
		/// </summary>
		EntityBlockViewer AllocateNewEntity(void* renderer);

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
		FORCE_INLINE void CullBlockEntityJob()
		{
			const unsigned int entityBlockCount = static_cast<unsigned int>(this->mEntityGridCell.mEntityBlocks.size());
			if (entityBlockCount > 0)
			{
				for (unsigned int cameraIndex = 0; cameraIndex < this->mCameraCount; cameraIndex++)
				{
					for (size_t moduleIndex = 0; moduleIndex < this->mUpdatedCullingModules.size(); moduleIndex++)
					{
						// TODO : Don't use pointer, Just use specific object(3 module) 
						// Why? : virtual funtion call should reference virtual function table,
						// We need really fast computation at here, 
						// referencing virtual function table make it slow

						CullingModule* cullingModule = this->mUpdatedCullingModules[moduleIndex];
						//TODO : ON X64, X84, memory_order_relaxed also do acquire memory
						//So This codes is too slow, FIX IT!!!!!!!!!!!
						//
						//
						//

						//TODO:
						//HOW works? 
						//
						//Each module is executed after other module
						//At each module, Every threads works on a EntityBlock
						//
						//EntityBlock a thread works on is decided by thread index
						//If a thread finished all assigned blocks, it steal block from other unfinished thread
						//
						//Example : 5 Threads
						//
						//Thread 1 : EntityBlock 1, 6, 11
						//Thread 2 : EntityBlock 2, 7, 12
						//Thread 3 : EntityBlock 3, 8, 13
						//Thread 4 : EntityBlock 4, 9, 14
						//Thread 5 : EntityBlock 5, 10, 14
						//
						//

						while (cullingModule->mFinishedCullEntityBlockCount[cameraIndex].load(std::memory_order_relaxed) < entityBlockCount)
						{
							if (cullingModule->mCurrentCullEntityBlockIndex[cameraIndex].load(std::memory_order_relaxed) >= entityBlockCount)
							{
								continue;
							}
							const unsigned int currentEntityBlockIndex = cullingModule->mCurrentCullEntityBlockIndex[cameraIndex].fetch_add(1, std::memory_order_release);
							if (currentEntityBlockIndex >= entityBlockCount)
							{
								continue;
							}

							EntityBlock* currentEntityBlock = this->mEntityGridCell.mEntityBlocks[currentEntityBlockIndex];
							const unsigned int entityCountInBlock = this->mEntityGridCell.AllocatedEntityCountInBlocks[currentEntityBlockIndex]; // don't use mCurrentEntityCount

							cullingModule->CullBlockEntityJob(currentEntityBlock, entityCountInBlock, cameraIndex);

							cullingModule->mFinishedCullEntityBlockCount[cameraIndex].fetch_add(1, std::memory_order_release);
						}

					}
				}

			}
			
		}

		/// <summary>
		/// Get Is All block's culling job is finished.
		/// if true, you can check object's visibility(culled)
		/// </summary>
		/// <returns></returns>
		FORCE_INLINE bool GetIsCullJobFinished(const std::atomic<unsigned int>& mFinishedCullEntityBlockCount, unsigned int entityBlockCount) const
		{
			return mFinishedCullEntityBlockCount.load(std::memory_order_relaxed) >= entityBlockCount;
		}

		/// <summary>
		/// Caller thread will stall until cull job of all entity block is finished
		/// </summary>
		FORCE_INLINE void WaitToFinishCullJobs() const
		{
			const unsigned int entityBlockCount = static_cast<unsigned int>(this->mEntityGridCell.mEntityBlocks.size());
			const size_t lastCameraIndex = this->mCameraCount - 1;
			const size_t lastModuleIndex = this->mUpdatedCullingModules.size() - 1;
			const CullingModule* lastCullingModule = this->mUpdatedCullingModules[lastModuleIndex];
			while (this->GetIsCullJobFinished(lastCullingModule->mFinishedCullEntityBlockCount[lastCameraIndex], entityBlockCount) == false) 
			{
				std::this_thread::yield();
			} 
		}

		/// <summary>
		/// Reset cull job state before pushing cull jobs to job pool
		/// </summary>
		FORCE_INLINE void ResetCullJobState()
		{
			this->SetAllOneIsVisibleFlag();
			this->ResetCullJobStateVariable();
		}

		/// <summary>
		/// Return Cached cull job std::function 
		/// </summary>
		/// <returns></returns>
		std::function<void()> GetCullJob();
	};
}


