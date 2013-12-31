#ifndef CPT_JUTIL_H
#define CPT_JUTIL_H

#include "bs_t.h"


jfieldID
GetClassField(JNIEnv *env, jobject obj, const char* method, const char* signture);

void * 
GetClassInstance(JNIEnv *env, jobject obj);

void
SetClassInstance(JNIEnv *env, jobject obj, void* instance);

void
JByteArrayCopyToBS(JNIEnv *env, jbyteArray ba, bs_t *bs);

void 
JByteArraySetToBS(JNIEnv *env, jbyteArray ba, bs_t *bs);

void
BSSetToJByteArray(JNIEnv *env, bs_t* bs, jbyteArray *ba);

void RegisterNativeMethods(JNIEnv *env, const char *clazz, 
	const JNINativeMethod* methods, uint32_t num);

#endif // CPT_JUTIL_H