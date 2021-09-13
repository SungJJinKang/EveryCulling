#pragma once
#include <assert.h>

#if defined(_MSC_VER)
#  define COMPILER_MSVC
#elif defined(__GNUC__)
#  define COMPILER_GCC
#endif

#ifndef FORCE_INLINE

#if defined(COMPILER_GCC)
#  define FORCE_INLINE inline __attribute__ ((always_inline))
#elif defined(COMPILER_MSVC)
#  define FORCE_INLINE __forceinline
#endif

#endif

///////////////////////////////////////////////////////////////////////////////////////
//Math
#ifndef MAX
#define MAX(A, B) A > B ? A : B
#endif

#ifndef MAX3
#define MAX3(A, B, C) MAX(A, MAX(B, C))
#endif

#ifndef MIN
#define MIN(A, B) A < B ? A : B
#endif

#ifndef MIN3
#define MIN3(A, B) MIN(A, MIN(B, C))
#endif

#ifndef ABS
#define	ABS(x)			( ((x)<0)?-(x):(x) )
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
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
#define INITIAL_ENTITY_BLOCK_RESERVED_SIZE 100
#endif

#ifndef MAX_CAMERA_COUNT
#define MAX_CAMERA_COUNT 3
#endif

///////////////////////////////////////////////////////////////////////////////////////
//ViewFrustum Culling
#ifndef BOUNDING_SPHRE_RADIUS_MARGIN
#define BOUNDING_SPHRE_RADIUS_MARGIN 0.001f
#endif

///////////////////////////////////////////////////////////////////////////////////////
//Masked SW Occlusion Culling

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

#ifndef BIN_TRIANGLE_CAPACITY_PER_TILE
#define BIN_TRIANGLE_CAPACITY_PER_TILE 100
#endif

#ifndef OCCLUDER_MINIMUM_AREA
#define OCCLUDER_MINIMUM_AREA 
#endif

#ifndef OCCLUDEE_MAXIMUM_AREA
#define OCCLUDEE_MAXIMUM_AREA
#endif

// Screen Space Bounding Sphere Culling
#ifndef ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
//#define ENABLE_SCREEN_SAPCE_BOUDING_SPHERE_CULLING
#endif


// Distance Culling
#ifndef ENABLE_DISTANCE_CULLING
#define ENABLE_DISTANCE_CULLING
#endif

// Query Occlusion
#if !defined(ENABLE_QUERY_OCCLUSION) && defined(CULLING_OPENGL)
#define ENABLE_QUERY_OCCLUSION
#endif