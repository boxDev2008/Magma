#pragma once

#include <stdbool.h>
#include <stdint.h>

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