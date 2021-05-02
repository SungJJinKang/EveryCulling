#pragma once

#include <Vector4.h>

#include "../FrotbiteCullingSystemCore.h"

#include "AABB.h"
#include "TransformData.h"
#include "EntityHandle.h"



namespace culling
{
	/// <summary>
	/// 이거 하면 InFrustum256FSIMDWithTwoPoint도 가능
	/// 다만 좀 많이 복잡해짐
	/// </summary>
// 		struct alignas(32) TwoPosition
// 		{
// 			Position TwoPosition[2];
// 		};


#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	inline static constexpr size_t ENTITY_COUNT_IN_ENTITY_BLOCK = (4096 - sizeof(unsigned int)) /
		(
			sizeof(math::Vector4) +
			sizeof(char) +
			sizeof(void*) +
			sizeof(EntityHandle) +
			sizeof(AABB)
			) - 1;
#else
	inline static constexpr size_t ENTITY_COUNT_IN_ENTITY_BLOCK = (4096 - sizeof(unsigned int)) /
		(
			sizeof(math::Vector4) +
			sizeof(char) +
			sizeof(void*) +
			sizeof(EntityHandle)
			) - 3;
#endif
		/// <summary>
		/// EntityBlock size should be less 4KB(Page size) for Block data being allocated in a page
		/// </summary>
	struct EntityBlock
	{
		//SoA (Structure of Array) !!!!!! for performance 

		/// <summary>
		/// x, y, z : components is position of entity
		/// w : component is radius of entity's sphere bound, should be negative!!!!!!!!!!!!!!
		/// 
		/// This will be used for linearlly Frustum intersection check
		/// 
		/// IMPORTANT : you should pass nagative value of radius!!!!!!!!!!!!!!!!!!!!!!!!!!!
		/// </summary>
		math::Vector4 mPositions[ENTITY_COUNT_IN_ENTITY_BLOCK];

#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
		/// <summary>
		/// Size of AABB is 32 byte.
		/// 2 AABB is 64 byte.
		/// 
		/// i recommend AABB's point type is Vector4
		/// because when multiply Matrix4x4 with point for calculating ScreenSpace AABB, we can use SIMD if point is vector
		/// 
		/// It's cache friendlly!!
		/// </summary>
		AABB mWorldAABB[ENTITY_COUNT_IN_ENTITY_BLOCK];
#endif

		//math::Vector4 is aligned to 16 byte and ENTITY_COUNT_IN_ENTITY_BLOCK is even number
		//so maybe  bool mIsVisibleBitflag is at right next to mPositions
		//
		// first low bit have Is Visible from First Camera,
		// second low bit have Is Visible from Second Camera
		// ......
		// This variable should be aligned to 32 byte.
		// because In Viewfrustum Culling CullJob VisibleFlag is stored per 32byte(128bit * 2)
		alignas(32) char mIsVisibleBitflag[ENTITY_COUNT_IN_ENTITY_BLOCK];
		
		/// <summary>
		/// mIsVisibleBitflag is stored through two __m128
		/// 
		/// So if this variable isn't aligned to 256bit,
		/// To prevent that sttoring mIsVisibleBitflag code set value to this variable
		/// 
		/// </summary>
		alignas(32) void* mRenderer[ENTITY_COUNT_IN_ENTITY_BLOCK];
		EntityHandle mHandles[ENTITY_COUNT_IN_ENTITY_BLOCK];
		//TransformData mTransformDatas[ENTITY_COUNT_IN_ENTITY_BLOCK];

		/// <summary>
		/// this variable is only used to decide whether to free this EntityBlock
		/// </summary>
		unsigned int mCurrentEntityCount;
	};

	/// <summary>
	/// Size of Entity block should be less than 4kb(page size)
	/// </summary>
	static_assert(sizeof(EntityBlock) < 4096);
	/// <summary>
	/// ENTITY_COUNT_IN_ENTITY_BLOCK should be even number
	/// </summary>
	static_assert(ENTITY_COUNT_IN_ENTITY_BLOCK > 0 && ENTITY_COUNT_IN_ENTITY_BLOCK % 2 == 0);
}
