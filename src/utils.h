#if !defined(UTILS_H)
#define UTILS_H

#include <stdint.h>

#define internal static
#define local_persist static
#define global static
#define local_const static const
#define global_const static const

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32_t bool32;

typedef float f32;
typedef double f64;

internal inline f32 RandomFloat(f32 Min, f32 Max)
{
    return(Min + ((f32)GetRandomValue(0, INT32_MAX) / (f32)INT32_MAX) * (Max - Min));
}

#endif
