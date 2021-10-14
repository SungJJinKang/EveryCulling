#pragma once

#include <vector>

#include "../CullingModule.h"

namespace culling
{
	struct CullDistanceVolume
	{
		/// <summary>
		/// Volume size
		/// </summary>
		FLOAT32 mSize;

		/// <summary>
		/// 
		/// </summary>
		FLOAT32 mCullDistance;
	};

	/// <summary>
	/// https://docs.unrealengine.com/4.26/en-US/RenderingAndGraphics/VisibilityCulling/CullDistanceVolume/
	/// Unreal Engine Source Code : https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Engine/Private/CullDistanceVolume.cpp
	/// Objects within the volume that have a size closest to CullDistanceVolume.mSize
	/// will be culled from view 
	/// when they are CullDistanceVolume.mCullDistance or more from the Camera.
	/// </summary>
	class DOOM_API DistanceCulling : CullingModule
	{

	private:

		std::vector<culling::CullDistanceVolume> mCullDistanceVolumes;

	public:

		// TODO : EntityBlock의 mPositions 이거 그냥 사용하면된다.
		// 거리 비교할 때 sqrt하지말고 그냥 mCullDistance * mCullDistance로 비교하자
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, SIZE_T entityCountInBlock, SIZE_T cameraIndex) final
		{
			if (mCullDistanceVolumes.size() == 0)
			{
				return;
			}


		}

		void AddCullDistanceVolume(culling::CullDistanceVolume cullDistanceVolume);
		void DeleteCullDistanceVolume(SIZE_T index);

	};
}

