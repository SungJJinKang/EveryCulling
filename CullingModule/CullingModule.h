#pragma once

#include <array>
#include <atomic>

#include "../FrotbiteCullingSystemCore.h"
#include "../DataType/EntityBlock.h"

namespace culling
{
	class FrotbiteCullingSystem;
	struct EntityBlock;

	class CullingModule
	{
		friend class FrotbiteCullingSystem;

	protected:

		FrotbiteCullingSystem* mCullingSystem;

		//static inline constexpr unsigned int M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
		/// <summary>
		/// will be used at CullBlockEntityJob
		/// </summary>
		//std::atomic<unsigned int> mAtomicCurrentBlockIndex;
		std::array<std::atomic<unsigned int>, MAX_CAMERA_COUNT> mCurrentCullEntityBlockIndex;
		std::array<std::atomic<unsigned int>, MAX_CAMERA_COUNT> mFinishedCullEntityBlockCount;
		
		unsigned int mCameraCount;

		CullingModule(FrotbiteCullingSystem* frotbiteCullingSystem)
			:mCullingSystem{ frotbiteCullingSystem }, mCameraCount{ 0 }
		{

		}
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex) = 0;
	
	public:


	};

}