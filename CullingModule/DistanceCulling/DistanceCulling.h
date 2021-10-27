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
		float mSize;

		/// <summary>
		/// 
		/// </summary>
		float mCullDistance;
	};

	/// <summary>
	/// https://docs.unrealengine.com/4.26/en-US/RenderingAndGraphics/VisibilityCulling/CullDistanceVolume/
	/// Unreal Engine Source Code : https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Engine/Private/CullDistanceVolume.cpp
	/// Objects within the volume that have a size closest to CullDistanceVolume.mSize
	/// will be culled from view 
	/// when they are CullDistanceVolume.mCullDistance or more from the Camera.
	/// </summary>
	class DistanceCulling : CullingModule
	{

	private:

		std::vector<culling::CullDistanceVolume> mCullDistanceVolumes;

	public:

		// TODO : EntityBlock�� mPositions �̰� �׳� ����ϸ�ȴ�.
		// �Ÿ� ���� �� sqrt�������� �׳� mCullDistance * mCullDistance�� ������
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex) final
		{
			if (mCullDistanceVolumes.size() == 0)
			{
				return;
			}


		}

		void AddCullDistanceVolume(culling::CullDistanceVolume cullDistanceVolume);
		void DeleteCullDistanceVolume(size_t index);

	};
}

