#pragma once

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
	class DistanceCulling : public CullingModule
	{

	private:

		void DoDistanceCulling
		(
			const size_t cameraIndex,
			culling::EntityBlock* const entityBlock
		);

	public:

		DistanceCulling(EveryCulling* const everyCulling);

		const char* GetCullingModuleName() const override;
		void CullBlockEntityJob(const size_t cameraIndex) override;
	};
}

