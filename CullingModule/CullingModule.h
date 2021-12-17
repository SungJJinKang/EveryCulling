#pragma once

#include <array>
#include <atomic>

#include "../EveryCullingCore.h"
#include "../DataType/EntityBlock.h"

namespace culling
{
	class EveryCulling;
	struct EntityBlock;

	class CullingModule
	{
	protected:

		EveryCulling* mCullingSystem;

		//static inline constexpr std::uint32_t M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
		/// <summary>
		/// will be used at CullBlockEntityJob
		/// </summary>
		//std::atomic<std::uint32_t> mAtomicCurrentBlockIndex;
		std::array<std::atomic<std::uint32_t>, MAX_CAMERA_COUNT> mCurrentCulledEntityBlockIndex;
		std::array<std::atomic<std::uint32_t>, MAX_CAMERA_COUNT> mFinishedCullEntityBlockCount;
		
		std::uint32_t mCameraCount;
		std::array<culling::Mat4x4, MAX_CAMERA_COUNT> mCameraViewProjectionMatrixs;

		CullingModule(EveryCulling* frotbiteCullingSystem)
			:mCullingSystem{ frotbiteCullingSystem }, mCameraCount{ 0 }
		{

		}
	
	public:

		virtual void SetViewProjectionMatrix(const std::uint32_t cameraIndex, const Mat4x4& viewProjectionMatrix);
		FORCE_INLINE const culling::Mat4x4& GetViewProjectionMatrix(const std::uint32_t cameraIndex)
		{
			return mCameraViewProjectionMatrixs[cameraIndex];
		}

		virtual void ClearEntityData
		(
			EntityBlock* currentEntityBlock, 
			std::uint32_t entityIndex
		) {};
		virtual void CullBlockEntityJob
		(
			EntityBlock* const currentEntityBlock, 
			const size_t entityCountInBlock, 
			const size_t cameraIndex
		)
		{}

	};

}