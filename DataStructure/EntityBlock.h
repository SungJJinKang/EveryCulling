#pragma once

#include <Vector4.h>

#include "../LinearTransformDataCulling_Core.h"

#include "TransformData.h"
#include "EntityHandle.h"


namespace doom
{
	namespace graphics
	{
		/// <summary>
		/// 이거 하면 InFrustum256FSIMDWithTwoPoint도 가능
		/// 다만 좀 많이 복잡해짐
		/// </summary>
// 		struct alignas(32) TwoPosition
// 		{
// 			Position TwoPosition[2];
// 		};
		
		/// <summary>
		/// ENTITY_COUNT_IN_ENTITY_BLOCK should be even number
		/// </summary>
		static_assert(ENTITY_COUNT_IN_ENTITY_BLOCK != 0 && ENTITY_COUNT_IN_ENTITY_BLOCK % 2 == 0);

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
			
			//math::Vector4 is aligned to 16 byte and ENTITY_COUNT_IN_ENTITY_BLOCK is even number
			//so maybe  bool mIsVisibleBitflag is at right next to mPositions
			//
			// first low bit have Is Visible from First Camera,
			// second low bit have Is Visible from Second Camera
			// ......
			alignas(32) char mIsVisibleBitflag[ENTITY_COUNT_IN_ENTITY_BLOCK];

			EntityHandle mHandles[ENTITY_COUNT_IN_ENTITY_BLOCK];
			TransformData mTransformDatas[ENTITY_COUNT_IN_ENTITY_BLOCK];

			/// <summary>
			/// this variable is only used to decide whether to free this EntityBlock
			/// </summary>
			unsigned int mCurrentEntityCount;
		};
	}
}