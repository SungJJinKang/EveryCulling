#pragma once

#include "../../DataType/Math/Matrix.h"

#include "../CullingModule.h"

namespace culling
{
	/// <summary>
	/// Project Object's Bounding AABB to Screen Space(Actually NDC)
	/// And Area of Projected AABB is less than setting value, It's culled
	/// </summary>
	class ScreenSpaceAABBCulling : public CullingModule
	{
		friend class FrotbiteCullingSystem;

	private:

		float mMinimumScreenSpaceAABBArea = 0.00015f;

		//For Calculating ScreenSpace AABB Area
		//AABB로 할지 아님 Sphere로 할지(https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space)...
		//
		Matrix4X4 mViewProjectionMatrix;

		ScreenSpaceAABBCulling(FrotbiteCullingSystem* frotbiteCullingSystem)
			:CullingModule{ frotbiteCullingSystem }
		{

		}

		void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t blockIndex, size_t cameraIndex) final;

	public:

		void SetViewProjectionMatrix(const Matrix4X4& viewProjectionMatrix);
	};
}
