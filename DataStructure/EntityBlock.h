#pragma once

#include <Vector4.h>

#include "../LinearViewFrustumCulling_Core.h"

#include "TransformData.h"
#include "TrnasformHandle.h"


namespace doom
{
	namespace graphics
	{
		/// <summary>
		/// �̰� �ϸ� InFrustum256FSIMDWithTwoPoint�� ����
		/// �ٸ� �� ���� ��������
		/// </summary>
// 		struct alignas(32) TwoPosition
// 		{
// 			Position TwoPosition[2];
// 		};



		/// <summary>
		/// EntityBlock size should be less 4KB(Page size) for Block data being allocated in a page
		/// </summary>
		struct EntityBlock
		{
			//SoA (Structure of Array) !!!!!! for performance 
			
			/// <summary>
			/// x, y, z components is position of entity
			/// w component is radius of entity's sphere bound
			/// 
			/// This will be used for linearlly Frustum intersection check
			/// </summary>
			math::Vector4 mPositions[ENTITY_COUNT_IN_ENTITY_BLOCK];
			TrnasformHandle mHandles[ENTITY_COUNT_IN_ENTITY_BLOCK];
			TransformData mTransformDatas[ENTITY_COUNT_IN_ENTITY_BLOCK];

		};
	}
}