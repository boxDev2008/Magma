#pragma once

#include "base.h"

#include <stdio.h>

typedef enum mg_log_level
{
	MG_LOG_TYPE_INFO,
	MG_LOG_TYPE_ERROR,
	MG_LOG_TYPE_WARNING
}
mg_log_level;

MG_API void mg_log(const mg_log_level level, const char *format, ...);

#if !defined(MG_NO_INFO_LOGGING)
#define MG_LOG(fmt, ...) mg_log(MG_LOG_TYPE_INFO, fmt, ##__VA_ARGS__)
#endif

#if !defined(MG_NO_ERROR_LOGGING)
#define MG_LOG_ERROR(fmt, ...) mg_log(MG_LOG_TYPE_ERROR, fmt, ##__VA_ARGS__)
#endif

#if !defined(MG_NO_WARNING_LOGGING)
#define MG_LOG_WARNING(fmt, ...) mg_log(MG_LOG_TYPE_WARNING, fmt, ##__VA_ARGS__)
#endif