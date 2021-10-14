#pragma once

#include <array>
#include <atomic>

#include "../EveryCullingCore.h"
#include "../DataType/EntityBlock.h"

namespace culling
{
	class EveryCulling;
	struct EntityBlock;

	class DOOM_API CullingModule
	{
		friend class EveryCulling;

	protected:

		EveryCulling* mCullingSystem;

		//static inline constexpr UINT32 M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
		/// <summary>
		/// will be used at CullBlockEntityJob
		/// </summary>
		//std::atomic<UINT32> mAtomicCurrentBlockIndex;
		std::array<std::atomic<UINT32>, MAX_CAMERA_COUNT> mCurrentCulledEntityBlockIndex;
		std::array<std::atomic<UINT32>, MAX_CAMERA_COUNT> mFinishedCullEntityBlockCount;
		
		UINT32 mCameraCount;
		std::array<culling::Mat4x4, MAX_CAMERA_COUNT> mCameraViewProjectionMatrixs;

		CullingModule(EveryCulling* frotbiteCullingSystem)
			:mCullingSystem{ frotbiteCullingSystem }, mCameraCount{ 0 }
		{

		}
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, SIZE_T entityCountInBlock, SIZE_T cameraIndex) {}
		virtual void ClearEntityData(EntityBlock* currentEntityBlock, UINT32 entityIndex) {};

	public:

		virtual void SetViewProjectionMatrix(const UINT32 cameraIndex, const Mat4x4& viewProjectionMatrix);

	};

}