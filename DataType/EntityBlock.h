#pragma once

#include "../EveryCullingCore.h"

#include "Math/AABB.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Position_BoundingSphereRadius.h"

#ifdef ENABLE_QUERY_OCCLUSION
#include "QueryObject.h"

#define QUERY_OBJECT_PTR_SIZE sizeof(culling::QueryObject*)

#elif
#define QUERY_OBJECT_PTR_SIZE 0

#endif


#include "VertexData.h"

#define MAKE_EVEN_NUMBER(X) (X - (X%2))

namespace culling
{

	//This code doesn't consider Memory alignment optimzation.
	inline constexpr size_t ENTITY_COUNT_IN_ENTITY_BLOCK =
		MAKE_EVEN_NUMBER(
			(4096 - sizeof(unsigned int)) /
			(
				sizeof(Vector4)
				+ sizeof(char)
				+ sizeof(void*)
				+ QUERY_OBJECT_PTR_SIZE
				+ sizeof(culling::VertexData)
		)) 
		- 2;

		/// <summary>
		/// EntityBlock size should be less 4KB(Page size) for Block data being allocated in a page
		/// </summary>
	struct EntityBlock
	{
		/// <summary>
		/// Why align to 32byte?
		/// To set mIsVisibleBitflag, We use _m256
		/// 
		/// You don't need to worry about false sharing.
		/// void* mRenderer[ENTITY_COUNT_IN_ENTITY_BLOCK] and mCurrentEntityCount isn't read during CullJob
		/// </summary>
		char mIsVisibleBitflag[ENTITY_COUNT_IN_ENTITY_BLOCK];
	
		//SoA (Structure of Array) !!!!!! for performance 

		/// <summary>
		/// x, y, z : components is position of entity
		/// w : component is radius of entity's sphere bound, should be negative!!!!!!!!!!!!!!
		/// 
		/// This will be used for linearlly Frustum intersection check
		/// 
		/// IMPORTANT : you should pass nagative value of radius!!!!!!!!!!!!!!!!!!!!!!!!!!!
		/// 
		/// Why align to 32byte?
		/// To set mIsVisibleBitflag, We use _m256
		/// 
		/// If Size of mIsVisibleBitflag isn't multiples of 256bit,
		/// Setting mIsVisibleBitflag make mPositions value dirty
		/// </summary>
		alignas(32) culling::Position_BoundingSphereRadius mPositions[ENTITY_COUNT_IN_ENTITY_BLOCK];

#ifdef ENABLE_QUERY_OCCLUSION
		culling::QueryObject* mQueryObjects[ENTITY_COUNT_IN_ENTITY_BLOCK];
#endif
		/// <summary>
		/// mIsVisibleBitflag is stored through two __m128
		/// 
		/// So if this variable isn't aligned to 256bit,
		/// To prevent that sttoring mIsVisibleBitflag code set value to this variable
		/// 
		/// </summary>
		void* mRenderer[ENTITY_COUNT_IN_ENTITY_BLOCK];

		VertexData mVertexDatas[ENTITY_COUNT_IN_ENTITY_BLOCK];
		
		//EntityHandle mHandles[ENTITY_COUNT_IN_ENTITY_BLOCK];

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
