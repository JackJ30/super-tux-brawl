#include "logger.h"

#include <stdio.h>

#define COLOR_RESET  "\033[0m"
#define COLOR_INFO   "\033[37m"
#define COLOR_WARN   "\033[33m"
#define COLOR_ERROR  "\033[31m"

void log_category(LOG_TYPE type, const char* message, ...) {
	va_list args;
	const char* color;
	const char* prefix;
	va_start(args, message);

	switch (type) {
		case LOG_WARN:
			color = COLOR_WARN;
			prefix = "WARN    ";
			break;
		case LOG_ERROR:
			color = COLOR_ERROR;
			prefix = "ERROR   ";
			break;
		default:
			color = COLOR_INFO;
			prefix = "INFO    ";
			break;
	}

    fprintf(stderr, "%s%s%s", color, prefix, COLOR_RESET); // Print color and prefix
    vfprintf(stderr, message, args);
	fprintf(stderr, "\n");

	va_end(args);
}
