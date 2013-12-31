#ifndef CPT_LOG_H
#define CPT_LOG_H

#include <android/log.h>

// Set to 1 to enable debug log traces.
#define DEBUG 1

// Set to 1 to optimize memory stores when generating plasma.
#define OPTIMIZE_WRITES  1

// Our library name.
#define LOG_TAG  "libcryptopg"

#if DEBUG
	#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__) // DEBUG
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__) // INFO
	#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__) // WARN
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__) // ERROR
	#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__) // FATAL
	#define ASSERT(exp) do { if (!(exp)) { __android_log_assert("assert", LOG_TAG, #exp); } } while(0)
	#define LOGPOS() do { LOGI("%s(%d){%s}", __FILE__, __LINE__, __func__); }  while(0)
#else
	#define NOP do {} while(0)
	#define LOGD(...) NOP
	#define LOGI(...) NOP
	#define LOGW(...) NOP
	#define LOGE(...) NOP
	#define LOGF(...) NOP
	#define ASSERT(exp) NOP
	#define LOGPOS() NOP
#endif

#endif // CPT_LOG_H