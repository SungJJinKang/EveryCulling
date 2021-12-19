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
	private:

		std::uint32_t mCameraCount;
		std::array<culling::Mat4x4, MAX_CAMERA_COUNT> mCameraViewProjectionMatrixs;
		std::array<culling::Vec3, MAX_CAMERA_COUNT> mCameraWorldPosition;

	protected:

		EveryCulling* mCullingSystem;

		

		CullingModule(EveryCulling* frotbiteCullingSystem)
			:mCullingSystem{ frotbiteCullingSystem }, mCameraCount{ 0 }
		{

		}
		
	public:

		//static inline constexpr std::uint32_t M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
		/// <summary>
		/// will be used at CullBlockEntityJob
		/// </summary>
		//std::atomic<std::uint32_t> mAtomicCurrentBlockIndex;
		std::array<std::atomic<std::uint32_t>, MAX_CAMERA_COUNT> mCurrentCulledEntityBlockIndex;
		std::array<std::atomic<std::uint32_t>, MAX_CAMERA_COUNT> mFinishedCullEntityBlockCount;


		void SetViewProjectionMatrix(const size_t cameraIndex, const Mat4x4& viewProjectionMatrix);
		FORCE_INLINE const culling::Mat4x4& GetViewProjectionMatrix(const size_t cameraIndex) const
		{
			return mCameraViewProjectionMatrixs[cameraIndex];
		}

		void SetCameraWorldPosition(const size_t cameraIndex, const Vec3& cameraWorldPosition);
		FORCE_INLINE const culling::Vec3& GetCameraWorldPosition(const size_t cameraIndex) const
		{
			return mCameraWorldPosition[cameraIndex];
		}

		void SetCameraCount(const size_t cameraCount);
		FORCE_INLINE size_t GetCameraWorldPosition() const
		{
			return mCameraCount;
		}

		virtual void ClearEntityData
		(
			EntityBlock* currentEntityBlock, 
			size_t entityIndex
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