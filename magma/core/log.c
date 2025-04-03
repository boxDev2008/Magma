#include "log.h"

#include <stdarg.h>
#include <time.h>

void mg_log(const mg_log_level level, const char *format, ...)
{
    static const char *levels[3] = {
        "\033[1;34mINFO\033[0m ",
        "\033[1;31mERROR\033[0m ",
        "\033[1;33mWARNING\033[0m "
    };

    char buffer[1024];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

#if !defined(MG_NO_TIME_LOGGING)
	char time_buffer[64];
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	strftime(time_buffer, sizeof(time_buffer), "[%Y-%m-%d %H:%M:%S] ", tm);
    fprintf(stdout, "%s%s%s\n", levels[level], time_buffer, buffer);
#else
	fprintf(stdout, "%s%s\n", levels[level], buffer);
#endif
}