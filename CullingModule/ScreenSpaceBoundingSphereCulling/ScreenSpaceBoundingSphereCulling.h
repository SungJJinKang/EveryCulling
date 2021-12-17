#pragma once

#include "../CullingModule.h"

namespace culling
{
	/// <summary>
	/// Project Object's Bounding AABB to Screen Space(Actually NDC)
	/// And Area of Projected AABB is less than setting value, It's culled
	/// </summary>
	class ScreenSpaceBoundingSphereCulling : public CullingModule
	{
		friend class EveryCulling;

	private:

		float mMinimumScreenSpaceAABBArea = 0.000005f;

		//For Calculating ScreenSpace AABB Area
		//AABB�� ���� �ƴ� Sphere�� ����(https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space)...
		//
	

		ScreenSpaceBoundingSphereCulling(EveryCulling* everyCulling);

	public:
		
		void CullBlockEntityJob
		(
			EntityBlock* const currentEntityBlock,
			const size_t entityCountInBlock,
			const size_t cameraIndex
		);

	};
}
