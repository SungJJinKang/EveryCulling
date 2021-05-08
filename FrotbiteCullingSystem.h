#pragma once

#include "FrotbiteCullingSystemCore.h"
#include "DataType/EntityGridCell.h"
#include "DataType/EntityBlockViewer.h"

#include <array>
#include <memory>
#include <vector>
#include <atomic>
#include <functional>


#include "CullingModule/ViewFrustumCulling/ViewFrustumCulling.h"

#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
#include "CullingModule/ScreenSpaceAABBCulling/ScreenSpaceAABBCulling.h"
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
	class FrotbiteCullingSystem
	{
		friend class ScreenSpaceAABBCulling;
		friend class ViewFrustumCulling;
		
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

		//static inline constexpr unsigned int M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
		/// <summary>
		/// will be used at CullBlockEntityJob
		/// </summary>
		//std::atomic<unsigned int> mAtomicCurrentBlockIndex;
		std::atomic<size_t> mCullJobFinishedBlockCount;

		/// <summary>
		/// Updating atomic variable is slow
		/// Add this value to mCachedCullBlockEntityJobs after CullJob is finished
		/// </summary>
		inline static thread_local size_t mThreadCullJobFinishedBlockCount{ 0 };
		std::function<void()> mCullJobCache;
		std::array<std::vector<std::function<void()>>, MAX_CAMERA_COUNT> mCachedCullBlockEntityJobs{};


		void AllocateEntityBlockPool();
		std::pair<culling::EntityBlock*, unsigned int*> AllocateNewEntityBlockFromPool();
		void RemoveEntityFromBlock(EntityBlock* ownerEntityBlock, unsigned int entityIndexInBlock);
		/// <summary>
		/// Block Swap removedblock with last block, and return swapped lastblock to pool
		/// </summary>
		void FreeEntityBlock(EntityBlock* freedEntityBlock);
		EntityBlock* GetNewEntityBlockFromPool();

		void CacheCullJob(size_t currentEntityBlockCount);
		//void CacheCullBlockEntityJobs();
		/// <summary>
		/// Release atomic variable
		/// </summary>
		void ReleaseFinishedBlockCount();
		/// <summary>
		/// Reset VisibleFlag
		/// </summary>
		void SetAllOneIsVisibleFlag();

	public:

		ViewFrustumCulling mViewFrustumCulling;
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
		ScreenSpaceAABBCulling mScreenSpaceAABBCulling;
#endif

		FrotbiteCullingSystem();
		~FrotbiteCullingSystem();
		void SetCameraCount(unsigned int cameraCount);
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
		FORCE_INLINE void CullBlockEntityJob(size_t blockIndex, size_t cameraIndex)
		{
			EntityBlock* currentEntityBlock = this->mEntityGridCell.mEntityBlocks[blockIndex];
			unsigned int entityCountInBlock = this->mEntityGridCell.AllocatedEntityCountInBlocks[blockIndex]; // don't use mCurrentEntityCount

			this->mViewFrustumCulling.CullBlockEntityJob(currentEntityBlock, entityCountInBlock, blockIndex, cameraIndex);

#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
			this->mScreenSpaceAABBCulling.CullBlockEntityJob(currentEntityBlock, entityCountInBlock, blockIndex, cameraIndex);
#endif

			this->mThreadCullJobFinishedBlockCount++;
		}

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
		std::vector<std::function<void()>> GetCullBlockEnityJobs(unsigned int cameraIndex);

		/// <summary>
		/// Get Is All block's culling job is finished.
		/// if true, you can check object's visibility(culled)
		/// </summary>
		/// <returns></returns>
		FORCE_INLINE bool GetIsCullJobFinished() const
		{
			return this->mCullJobFinishedBlockCount.load(std::memory_order_relaxed) == this->mEntityGridCell.mEntityBlocks.size();
		}

		/// <summary>
		/// Caller thread will stall until cull job of all entity block is finished
		/// </summary>
		FORCE_INLINE void WaitToFinishCullJobs() const
		{
			while (this->GetIsCullJobFinished() == false) {} // busy wait!
		}

		/// <summary>
		/// Reset cull job state before pushing cull jobs to job pool
		/// </summary>
		FORCE_INLINE void ResetCullJobState()
		{
			this->mCullJobFinishedBlockCount = 0;
			this->SetAllOneIsVisibleFlag();
		}

		/// <summary>
		/// Return Cached cull job std::function 
		/// </summary>
		/// <returns></returns>
		std::function<void()> GetCullJobs() const;
	};
}


