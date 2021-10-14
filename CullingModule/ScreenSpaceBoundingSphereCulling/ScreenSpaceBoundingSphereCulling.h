#pragma once

#include "../CullingModule.h"

namespace culling
{
	/// <summary>
	/// Project Object's Bounding AABB to Screen Space(Actually NDC)
	/// And Area of Projected AABB is less than setting value, It's culled
	/// </summary>
	class DOOM_API ScreenSpaceBoundingSphereCulling : public CullingModule
	{
		friend class EveryCulling;

	private:

		FLOAT32 mMinimumScreenSpaceAABBArea = 0.000005f;

		//For Calculating ScreenSpace AABB Area
		//AABB�� ���� �ƴ� Sphere�� ����(https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space)...
		//
	

		ScreenSpaceBoundingSphereCulling(EveryCulling* everyCulling);

		void CullBlockEntityJob(EntityBlock* currentEntityBlock, SIZE_T entityCountInBlock, SIZE_T cameraIndex);

	public:

	
	};
}
