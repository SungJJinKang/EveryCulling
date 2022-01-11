#pragma once
#include <assert.h>

#include <stddef.h>

#include "DataType/Math/SIMD_Core.h"

#if defined(__GNUC__)  || defined( __clang__)
#  define FORCE_INLINE inline __attribute__ ((always_inline))
#  define NEVER_INLINE __attribute__ ((noinline))
#  define RESTRICT __restrict
#  define VLA_ARRAY_ON_STACK(type__, varname__, size__) type__ varname__[size__];
#elif defined(_MSC_VER)
#  define FORCE_INLINE __forceinline
#  define NEVER_INLINE __declspec(noinline)
#  define RESTRICT __restrict
#  define VLA_ARRAY_ON_STACK(type__, varname__, size__) type__ *varname__ = (type__*)_alloca(size__ * sizeof(type__))
#endif

#define CACHE_LINE_SIZE 64

///////////////////////////////////////////////////////////////////////////////////////
//Math
#ifndef MAX
#define MAX(A, B) ((A > B) ? A : B)
#endif

#ifndef MAX3
#define MAX3(A, B, C) MAX(A, MAX(B, C))
#endif

#ifndef MIN
#define MIN(A, B) ((A < B) ? A : B)
#endif

#ifndef MIN3
#define MIN3(A, B) MIN(A, MIN(B, C))
#endif

#ifndef ABS
#define	ABS(x)			( ((x)<0)?-(x):(x) )
#endif


///////////////////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
#define DEBUG_CULLING
#endif

#if defined(_DEBUG) || true
#define PROFILING_CULLING
#endif


///////////////////////////////////////////////////////////////////////////////////////
//Graphics API

//Use Your 
#ifndef CULLING_OPENGL
#define CULLING_OPENGL
//#include "" Put Your Opengl Header!!
#endif

#ifndef CULLING_DIRECTX
//#define CULLING_DIRECTX
//#include "" Put Your DIRECTX Header!!
#endif

#ifdef CULLING_OPENGL
#define NDC_RANGE MINUS_ONE_TO_POSITIVE_ONE
#elif DIRECTX
#define NDC_RANGE ZERO_TO_POSITIVE_ONE
#endif



///////////////////////////////////////////////////////////////////////////////////////
//EntityBlock
#ifndef INITIAL_ENTITY_BLOCK_COUNT
#define INITIAL_ENTITY_BLOCK_COUNT 10
#endif

#ifndef INITIAL_ENTITY_BLOCK_RESERVED_SIZE
#define INITIAL_ENTITY_BLOCK_RESERVED_SIZE 512
#endif

#ifndef MAX_CAMERA_COUNT
#define MAX_CAMERA_COUNT 3
#endif

///////////////////////////////////////////////////////////////////////////////////////
//ViewFrustum Culling
#ifndef BOUNDING_SPHRE_RADIUS_MARGIN
#define BOUNDING_SPHRE_RADIUS_MARGIN 0.1f
#endif

///////////////////////////////////////////////////////////////////////////////////////
//Masked SW Occlusion Culling

//HAAM16
#ifndef QUICK_MASK
#define QUICK_MASK 1
#endif

#ifndef FETCH_OBJECT_SORT_FROM_DOOMS_ENGINE_IN_BIN_TRIANGLE_STAGE
#define FETCH_OBJECT_SORT_FROM_DOOMS_ENGINE_IN_BIN_TRIANGLE_STAGE 1
#endif

#define BACK_FACE_WINDING BACK_FACE_CCW

#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE

#define MIN_DEPTH_VALUE -1.0f
#define MAX_DEPTH_VALUE 1.0f

#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE

#define MIN_DEPTH_VALUE 0.0f
#define MAX_DEPTH_VALUE 1.0f

#endif

#ifndef TILE_WIDTH
#define TILE_WIDTH 32
#endif

#ifndef TILE_HEIGHT
#define TILE_HEIGHT 8
#endif

#ifndef SUB_TILE_WIDTH
#define SUB_TILE_WIDTH 8
#endif

#ifndef SUB_TILE_HEIGHT
#define SUB_TILE_HEIGHT 4
#endif

#ifndef BIN_TRIANGLE_CAPACITY_PER_TILE_PER_OBJECT
#define BIN_TRIANGLE_CAPACITY_PER_TILE_PER_OBJECT 32
#endif

#ifndef MAX_OCCLUDER_COUNT
#define MAX_OCCLUDER_COUNT 10
#endif

// Screen Space Bounding Sphere Culling
#ifndef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
//#define ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
#endif


// Distance Culling
#ifndef DEFAULT_DESIRED_MAX_DRAW_DISTANCE
#define DEFAULT_DESIRED_MAX_DRAW_DISTANCE 6000.0f
#endif

#ifndef IS_ALIGNED_ASSERT
#define IS_ALIGNED_ASSERT(ADDRESS, ALIGNMENT) (assert(ADDRESS % ALIGNMENT == 0))
#endif