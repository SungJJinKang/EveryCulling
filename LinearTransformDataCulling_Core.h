#pragma once

#define MAX_ENTITY_BLOCK_COUNT 200
#define ENTITY_COUNT_IN_ENTITY_BLOCK 40

#define MAX_CAMERA_COUNT 8

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
