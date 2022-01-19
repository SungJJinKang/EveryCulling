#pragma once

#include <array>
#include <atomic>

#include "../EveryCullingCore.h"
#include "../DataType/EntityBlock.h"
#include "../EveryCulling.h"

namespace culling
{
	class EveryCulling;
	struct EntityBlock;

	struct CullJobState
	{
		char padding1[64];
		//static inline constexpr std::uint32_t M256_COUNT_OF_VISIBLE_ARRAY = 1 + ( (ENTITY_COUNT_IN_ENTITY_BLOCK * sizeof(decltype(*EntityBlock::mIsVisibleBitflag)) - 1) / 32 );
		/// <summary>
		/// will be used at ThreadCullJob
		/// </summary>
		//std::atomic<std::uint32_t> mAtomicCurrentBlockIndex;
		std::array<std::atomic<std::uint32_t>, MAX_CAMERA_COUNT> mCurrentCulledEntityBlockIndex;
		char padding2[64];
		std::array<std::atomic<std::uint32_t>, MAX_CAMERA_COUNT> mFinishedThreadCount;
		char padding4[64];
	};
	class CullingModule
	{
	private:
		
		CullJobState mCullJobState;

		/// <summary>
		/// return next entity block
		///	if consume all entity block, return nullptr
		/// </summary>
		/// <param name="cameraIndex"></param>
		/// <returns></returns>
		culling::EntityBlock* GetNextEntityBlock(const size_t cameraIndex, const bool forceOrdering = true);

	protected:

		EveryCulling* const mCullingSystem;

		culling::EntityBlock* GetNextEntityBlockForMultipleThreads(const size_t cameraIndex, const std::int32_t localThreadIndex = EVERYCULLING_INVALID_LOCAL_THREAD_INDEX);

		

		

	public:

		CullingModule(EveryCulling* cullingSystem);
		virtual ~CullingModule();

		bool IsEnabled;

		virtual void ResetCullingModule(const unsigned long long currentTickCount);
		EVERYCULLING_FORCE_INLINE std::uint32_t GetFinishedThreadCount(const size_t cameraIndex) const
		{
			return mCullJobState.mFinishedThreadCount[cameraIndex];
		}



		virtual void OnSetViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& cameraViewProjectionMatrix)
		{
		}
		virtual void OnSetModelViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& cameraModelViewProjectionMatrix)
		{
		}
		virtual void OnSetCameraWorldPosition(const size_t cameraIndex, const culling::Vec3& cameraWorldPosition)
		{
		}
		virtual void OnSetCameraRotation(const size_t cameraIndex, const culling::Vec4& cameraRotation)
		{
		}
		virtual void OnSetCameraCount(const size_t cameraCount)
		{
		}
		virtual void OnSetCameraFarClipPlaneDistance(const size_t cameraIndex, const float farClipPlaneDistance)
		{
		}
		virtual void OnSetCameraNearClipPlaneDistance(const size_t cameraIndex, const float nearClipPlaneDistance)
		{
		}
		virtual void OnSetCameraFieldOfView(const size_t cameraIndex, const float fieldOfView)
		{
		}

		virtual void ClearEntityData
		(
			EntityBlock* currentEntityBlock, 
			size_t entityIndex
		) {};

		virtual void CullBlockEntityJob
		(
			const size_t cameraIndex, const std::int32_t localThreadIndex, const unsigned long long currentTickCount
		) = 0;
		
		void ThreadCullJob(const size_t cameraIndex, const std::int32_t localThreadIndex, const unsigned long long currentTickCount);

		virtual const char* GetCullingModuleName() const = 0;
	};

}
