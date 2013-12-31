#include <jni.h>
#include <stdlib.h>
#include "jutil.h"
#include "debug.h"

jfieldID GetClassField(JNIEnv *env, jobject obj, const char* method, 
	const char* signture)
{
	jclass clazz;

	LOGPOS();

	clazz = (jclass) (*env)->GetObjectClass(env, obj);

	return (jfieldID)(*env)->GetFieldID(env, clazz, method, signture);
}

void *GetClassInstance(JNIEnv *env, jobject obj)
{
	LOGPOS();

	int64_t i64 = 0;

	i64 = (*env)->GetLongField(env, obj, 
		GetClassField(env, obj, "instance", "J"));

	return (void*)(uintptr_t)(i64 & 0xFFFFFFFF);
}

void SetClassInstance(JNIEnv *env, jobject obj, void* instance)
{
	 LOGPOS();

	 int64_t i64 = (uintptr_t)instance;

	 (*env)->SetLongField(env, obj, 
	 	GetClassField(env, obj, "instance", "J"), i64);
}

void JByteArrayCopyToBS(JNIEnv *env, jbyteArray ba, bs_t *bs)
{
	LOGPOS();

	bs->len  = (*env)->GetArrayLength(env, ba);
	bs->data = (byte_t*)malloc(bs->len * sizeof(byte_t));
	(*env)->GetByteArrayRegion(env, ba, 0, bs->len, bs->data);
}

void JByteArraySetToBS(JNIEnv *env, jbyteArray ba, bs_t *bs)
{
	byte_t   *data;
	uint32_t len;

	LOGPOS();

	data = (byte_t*)(*env)->GetByteArrayElements(env, ba, 0);
	len = (*env)->GetArrayLength(env, ba);

	bs_ref(bs, data, len);
}

void ReleaseJBASetToBS(JNIEnv *env, jbyteArray ba, bs_t *bs)
{
	LOGPOS();
	
	(*env)->ReleaseByteArrayElements(env, ba, bs->data, JNI_ABORT);
	bs->data = NULL;
	bs->len = 0;
}

void BSSetToJByteArray(JNIEnv *env, bs_t* bs, jbyteArray *ba)
{
	LOGPOS();

	(*env)->SetByteArrayRegion(env, *ba, 0, bs->len, bs->data);
}

void RegisterNativeMethods(JNIEnv *env, const char *clazz, 
	const JNINativeMethod* methods, uint32_t num)
{
	errno_t err;

	LOGPOS();

	ASSERT((*env)->FindClass(env, clazz) != NULL);

	err = (*env)->RegisterNatives(env, 
		(*env)->FindClass(env, clazz),
		methods, 
		num);

	if (err != JNI_OK) {
		LOGW("Error Register Methods for %s", clazz);
	}
}