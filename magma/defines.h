#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef int b32;
typedef char b8;

#define true 1
#define false 0

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define MG_PLATFORM_WINDOWS 1
#elif define(__linux__) || define(__gnu_linux__)
#define MG_PLATFORM_LINUX 1
#endif

#ifdef MG_EXPORT
#ifdef _MSC_VER
#define MG_API __declspec(dllexport)
#else
#define MG_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define MG_API __declspec(dllimport)
#else
#define MG_API
#endif
#endif