#pragma once

#define MAX_ENTITY_BLOCK_COUNT 1000

#define MAX_CAMERA_COUNT 8

#define BOUNDING_SPHRE_RADIUS_MARGIN 0.2f

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

#define DISABLE_SCREEN_SAPCE_AABB_CULLING