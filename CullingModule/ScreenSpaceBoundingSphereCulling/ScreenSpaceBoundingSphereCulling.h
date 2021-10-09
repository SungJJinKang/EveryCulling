#pragma once

#include "../../DataType/Math/Matrix.h"

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

		float mMinimumScreenSpaceAABBArea = 0.000005f;

		//For Calculating ScreenSpace AABB Area
		//AABB로 할지 아님 Sphere로 할지(https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space)...
		//
	

		ScreenSpaceBoundingSphereCulling(EveryCulling* everyCulling);

		void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex);

	public:

	
	};
}
