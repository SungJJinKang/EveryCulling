#pragma once
#include <assert.h>

#if defined(_MSC_VER)
#  define COMPILER_MSVC
#elif defined(__GNUC__)
#  define COMPILER_GCC
#endif

#if defined(COMPILER_GCC)
#  define FORCE_INLINE inline __attribute__ ((always_inline))
#elif defined(COMPILER_MSVC)
#  define FORCE_INLINE __forceinline
#endif

///////////////////////////////////////////////////////////////////////////////////////
//Math
#define MAX(A, B) A > B ? A : B
#define MAX3(A, B, C) MAX(A, MAX(B, C))
#define MIN(A, B) A < B ? A : B
#define MIN3(A, B) MIN(A, MIN(B, C))

///////////////////////////////////////////////////////////////////////////////////////
//Graphics API
#define OPENGL
//#define DIRECTX

#ifdef OPENGL
#define NDC_RANGE MINUS_ONE_TO_POSITIVE_ONE
#elif DIRECTX
#define NDC_RANGE ZERO_TO_POSITIVE_ONE
#endif



///////////////////////////////////////////////////////////////////////////////////////
//EntityBlock
#define INITIAL_ENTITY_BLOCK_COUNT 10
#define INITIAL_ENTITY_BLOCK_RESERVED_SIZE 100
#define MAX_CAMERA_COUNT 3


///////////////////////////////////////////////////////////////////////////////////////
//ViewFrustum Culling
#define BOUNDING_SPHRE_RADIUS_MARGIN 0.001f


///////////////////////////////////////////////////////////////////////////////////////
//Masked SW Occlusion Culling
#define TILE_WIDTH 32
#define TILE_HEIGHT 8
#define SUB_TILE_WIDTH 8
#define SUB_TILE_HEIGHT 4

#define BIN_TRIANGLE_CAPACITY_PER_TILE 100

#define OCCLUDER_MINIMUM_AREA 
#define OCCLUDEE_MAXIMUM_AREA

// Screen Space AABB Culling
//#define ENABLE_SCREEN_SAPCE_AABB_CULLING
