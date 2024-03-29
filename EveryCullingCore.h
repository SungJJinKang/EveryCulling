#pragma once
#include <cassert>
#include <cstdint>

#if defined(__GNUC__)  || defined( __clang__)
#  define EVERYCULLING_FORCE_INLINE inline __attribute__ ((always_inline))
#elif defined(_MSC_VER)
#  define EVERYCULLING_FORCE_INLINE __forceinline
#endif

#define EVERYCULLING_CACHE_LINE_SIZE 64

///////////////////////////////////////////////////////////////////////////////////////
//Math
#ifndef EVERYCULLING_MAX
#define EVERYCULLING_MAX(A, B) ((A > B) ? A : B)
#endif

#ifndef EVERYCULLING_MAX3
#define EVERYCULLING_MAX3(A, B, C) EVERYCULLING_MAX(A, EVERYCULLING_MAX(B, C))
#endif

#ifndef EVERYCULLING_MIN
#define EVERYCULLING_MIN(A, B) ((A < B) ? A : B)
#endif

#ifndef EVERYCULLING_MIN3
#define EVERYCULLING_MIN3(A, B) EVERYCULLING_MIN(A, EVERYCULLING_MIN(B, C))
#endif

#ifndef EVERYCULLING_ABS
#define	EVERYCULLING_ABS(x)			( ((x)<0)?-(x):(x) )
#endif


///////////////////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
#define EVERYCULLING_DEBUG_CULLING
#endif

#if defined(_DEBUG) || true
#define EVERYCULLING_PROFILING_CULLING
#endif


///////////////////////////////////////////////////////////////////////////////////////
//Graphics API

//Use Your 
#ifndef EVERYCULLING_OPENGL
#define EVERYCULLING_OPENGL
//#include "" Put Your Opengl Header!!
#endif

#ifndef EVERYCULLING_DIRECTX
//#define EVERYCULLING_DIRECTX
//#include "" Put Your DIRECTX Header!!
#endif

#ifdef EVERYCULLING_OPENGL
#define EVERYCULLING_NDC_RANGE EVERYCULLING_MINUS_ONE_TO_POSITIVE_ONE
#elif DIRECTX
#define EVERYCULLING_NDC_RANGE EVERYCULLING_ZERO_TO_POSITIVE_ONE
#endif



///////////////////////////////////////////////////////////////////////////////////////
//EntityBlock
#ifndef EVERYCULLING_INITIAL_ENTITY_BLOCK_COUNT
#define EVERYCULLING_INITIAL_ENTITY_BLOCK_COUNT 10
#endif

#ifndef EVERYCULLING_INITIAL_ENTITY_BLOCK_RESERVED_SIZE
#define EVERYCULLING_INITIAL_ENTITY_BLOCK_RESERVED_SIZE 512
#endif

#ifndef EVERYCULLING_MAX_CAMERA_COUNT
#define EVERYCULLING_MAX_CAMERA_COUNT 5
#endif

#ifndef EVERYCULLING_MAX_THREAD_COUNT
#define EVERYCULLING_MAX_THREAD_COUNT 10
#endif

///////////////////////////////////////////////////////////////////////////////////////
//ViewFrustum Culling
#ifndef EVERYCULLING_BOUNDING_SPHRE_RADIUS_MARGIN
#define EVERYCULLING_BOUNDING_SPHRE_RADIUS_MARGIN 0.1f
#endif

///////////////////////////////////////////////////////////////////////////////////////
//Masked SW Occlusion Culling

//HAAM16
#ifndef EVERYCULLING_QUICK_MASK
#define EVERYCULLING_QUICK_MASK 1
#endif

#ifndef EVERYCULLING_FETCH_OBJECT_SORT_FROM_DOOMS_ENGINE_IN_BIN_TRIANGLE_STAGE
#define EVERYCULLING_FETCH_OBJECT_SORT_FROM_DOOMS_ENGINE_IN_BIN_TRIANGLE_STAGE 1
#endif

#define EVERYCULLING_BACK_FACE_WINDING EVERYCULLING_BACK_FACE_CCW

#if EVERYCULLING_NDC_RANGE == EVERYCULLING_MINUS_ONE_TO_POSITIVE_ONE

#define EVERYCULLING_MIN_DEPTH_VALUE -1.0f
#define EVERYCULLING_MAX_DEPTH_VALUE 1.0f

#elif EVERYCULLING_NDC_RANGE == EVERYCULLING_ZERO_TO_POSITIVE_ONE

#define EVERYCULLING_MIN_DEPTH_VALUE 0.0f
#define EVERYCULLING_MAX_DEPTH_VALUE 1.0f

#endif

#ifndef EVERYCULLING_TILE_WIDTH
#define EVERYCULLING_TILE_WIDTH 32
#endif

#ifndef EVERYCULLING_TILE_HEIGHT
#define EVERYCULLING_TILE_HEIGHT 8
#endif

#ifndef EVERYCULLING_SUB_TILE_WIDTH
#define EVERYCULLING_SUB_TILE_WIDTH 8
#endif

#ifndef EVERYCULLING_SUB_TILE_HEIGHT
#define EVERYCULLING_SUB_TILE_HEIGHT 4
#endif

#ifndef EVERYCULLING_BIN_TRIANGLE_CAPACITY_PER_TILE_PER_OBJECT
#define EVERYCULLING_BIN_TRIANGLE_CAPACITY_PER_TILE_PER_OBJECT 32
#endif

#ifndef EVERYCULLING_MAX_BINNED_INDICE_COUNT
#define EVERYCULLING_MAX_BINNED_INDICE_COUNT (std::uint64_t)50000
#endif

#ifndef EVERYCULLING_RASTERIZE_DEPTH_BUFFER_FOR_TWO_FRAMES
#define EVERYCULLING_RASTERIZE_DEPTH_BUFFER_FOR_TWO_FRAMES 1
#endif

#if EVERYCULLING_RASTERIZE_DEPTH_BUFFER_FOR_TWO_FRAMES == 1

#ifndef EVERYCULLING_WHEN_TO_BIN_TRIANGLE
#define EVERYCULLING_WHEN_TO_BIN_TRIANGLE(TICK_COUNT) (TICK_COUNT % 2 == 1)
#endif

#ifndef EVERYCULLING_WHEN_TO_RASTERIZE_DEPTHBUFFER
#define EVERYCULLING_WHEN_TO_RASTERIZE_DEPTHBUFFER(TICK_COUNT) (TICK_COUNT % 2 == 0)
#endif

#else

#ifndef EVERYCULLING_WHEN_TO_BIN_TRIANGLE
#define EVERYCULLING_WHEN_TO_BIN_TRIANGLE(TICK_COUNT) true
#endif

#ifndef EVERYCULLING_WHEN_TO_RASTERIZE_DEPTHBUFFER
#define EVERYCULLING_WHEN_TO_RASTERIZE_DEPTHBUFFER(TICK_COUNT) true
#endif

#endif

// Distance Culling
#ifndef EVERYCULLING_DEFAULT_DESIRED_MAX_DRAW_DISTANCE
#define EVERYCULLING_DEFAULT_DESIRED_MAX_DRAW_DISTANCE 10000.0f
#endif

#ifndef EVERYCULLING_ALIGNMENT_ASSERT
#define EVERYCULLING_ALIGNMENT_ASSERT(ADDRESS, ALIGNMENT) (assert(ADDRESS % ALIGNMENT == 0))
#endif

#ifndef EVERYCULLING_INVALID_ENTITY_UNIQUE_ID_MAGIC_NUMBER
#define EVERYCULLING_INVALID_ENTITY_UNIQUE_ID_MAGIC_NUMBER (std::uint64_t)0xfafafafafafafafa
#endif
