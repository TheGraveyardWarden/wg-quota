#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#define CHECK_PTR(ptr, ...) \
	if (ptr == NULL) {\
		LOG_ERR(__VA_ARGS__);\
		exit(1);\
	}
#else
#define LOG(...)
#define CHECK_PTR(ptr, ...)
#endif

#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#define WARNING(...) fprintf(stderr, __VA_ARGS__)

#ifdef BACKTRACE
#define LOG_BACKTRACE(...) LOG_ERR(__VA_ARGS__)
#else
#define LOG_BACKTRACE(...)
#endif

#endif

