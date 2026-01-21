#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

typedef enum {
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR
} LOG_TYPE;

#define log_info(message, ...) log_category(LOG_INFO, message, ##__VA_ARGS__)
#define log_warn(message, ...) log_category(LOG_WARN, message, ##__VA_ARGS__)
#define log_err(message, ...) log_category(LOG_ERROR, message, ##__VA_ARGS__)

void log_category(LOG_TYPE type, const char* message, ...)
	__attribute__((format(printf, 2, 3)));

#endif
