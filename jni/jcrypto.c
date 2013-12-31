#include <jni.h>

#include "debug.h"
#include "jutil.h"
#include "aes_t.h"
#include "bs_t.h"
#include "rc4_t.h"

void RegisterRawRC4 (JNIEnv *env);
void RegisterRawAES (JNIEnv *env);
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNIEnv *env;

	LOGPOS();

	if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("Get Env Failed");
		return JNI_ERR;
	}

	RegisterRawAES(env);
	RegisterRawRC4(env);

	return JNI_VERSION_1_4;
}

/* =============================== RawAES ================================== */

/*
 * Signature: ()Z
 */
static jboolean RawAESNew(JNIEnv *env, jobject thiz)
{
	LOGPOS();

	if (GetClassInstance(env, thiz) != 0) {
		LOGW("The instance is not NULL, do we recreat it?");
	}

	SetClassInstance(env, thiz, aes_new());
}

/*
 * Signature: ([B)Z
 */
static jboolean RawAESSetupEncKey(JNIEnv *env, jobject thiz, jbyteArray ba)
{
	aes_t *aes;
	bs_t  *key;

	LOGPOS();

	aes = (aes_t*)GetClassInstance(env, thiz);

	key = bs_new();
	JByteArraySetToBS(env, ba, key);

	aes_setup_enckey(aes, key);

	ReleaseJBASetToBS(env, ba, key); // Instead of bs_destroy(bs).
}

/*
 * Signature: ([B)[B
 */
static jbyteArray RawAESEncBuf(JNIEnv *env, jobject thiz, jbyteArray ba)
{
	aes_t *aes;
	bs_t  *p, *c;
	jbyteArray ret;

	LOGPOS();

	aes = (aes_t*)GetClassInstance(env, thiz);

	p = bs_new();
	JByteArraySetToBS(env, ba, p);

	c = bs_new();
	aes_enc_buf(aes, p, c);

	ret = (*env)->NewByteArray(env, c->len);
	BSSetToJByteArray(env, c, &ret);

	ReleaseJBASetToBS(env, ba, p);
	bs_destroy(p);
	bs_destroy(c);

	return ret;
}

/*
 * Signature: ([B)Z
 */
static jboolean RawAESSetupDecKey(JNIEnv *env, jobject thiz, jbyteArray ba)
{
	aes_t *aes;
	bs_t  *key;

	LOGPOS();

	aes = (aes_t*)GetClassInstance(env, thiz);

	key = bs_new();
	JByteArraySetToBS(env, ba, key);

	aes_setup_deckey(aes, key);

	ReleaseJBASetToBS(env, ba, key); // Instead of bs_destroy(bs).
}

/*
 * Signature: ([B)[B
 */
static jbyteArray RawAESDecBuf(JNIEnv *env, jobject thiz, jbyteArray ba)
{
	aes_t *aes;
	bs_t  *p, *c;
	jbyteArray ret;

	LOGPOS();

	aes = (aes_t*)GetClassInstance(env, thiz);

	c = bs_new();
	JByteArraySetToBS(env, ba, c);

	p = bs_new();
	aes_dec_buf(aes, c, p);

	ret = (*env)->NewByteArray(env, p->len);
	BSSetToJByteArray(env, p, &ret);

	ReleaseJBASetToBS(env, ba, c);
	bs_destroy(p);
	bs_destroy(c);

	return ret;
}

/*
 * Signature: ()V
 */
static void RawAESDestroy(JNIEnv *env, jobject thiz)
{
	aes_t *aes;

	LOGPOS();

	aes = (aes_t*)GetClassInstance(env, thiz);
	if (aes != NULL) {
		aes_destory(aes);
		SetClassInstance(env, thiz, 0);
	}
}

void RegisterRawAES (JNIEnv *env) {
	static JNINativeMethod methods[]  = {
		{ "RawNew",         "()Z",    (void*)RawAESNew },
		{ "RawSetupEncKey", "([B)Z",  (void*)RawAESSetupEncKey },
		{ "RawEncBuf",      "([B)[B", (void*)RawAESEncBuf },
		{ "RawSetupDecKey", "([B)Z",  (void*)RawAESSetupDecKey },
		{ "RawDecBuf",      "([B)[B", (void*)RawAESDecBuf },
		{ "RawDestroy",     "()V",    (void*)RawAESDestroy },
	};
	RegisterNativeMethods(env, "com/mobilepg/crypto/RawAES", methods, 
		sizeof(methods)/sizeof(methods[0]));
}

/* =============================== RawRC4 ================================== */

static void RawRC4New(JNIEnv *env, jobject thiz, jbyteArray ba) 
{
	bs_t  *k;
	rc4_t *r;

	LOGPOS();

	k = bs_new();
	JByteArrayCopyToBS(env, ba, k);
	
	r = rc4_new(k);
	if (r == NULL) {
		LOGE("Error Create RC4 Instance!");
		return ;
	}

	SetClassInstance(env, thiz, r);

	bs_destroy(k);

	return ;
}

static void RawRC4XORStream(JNIEnv *env, jobject thiz, jbyteArray ba)
{
	rc4_t *r;
	bs_t  *s;
	int32_t i;

	LOGPOS();

	r = GetClassInstance(env, thiz);
	ASSERT(r != NULL);

	s = bs_new();
	s->data = (byte_t*)(*env)->GetByteArrayElements(env, ba, 0);
	s->len =  (*env)->GetArrayLength(env, ba);

	rc4_xstream(r, s, s);

	(*env)->ReleaseByteArrayElements(env, ba, s->data, JNI_COMMIT);
	s->data = NULL;
	s->len = 0;

	bs_destroy(s);

	return ;
}

static void RawRC4Reset(JNIEnv *env, jobject thiz)
{
	rc4_t *r;

	LOGPOS();

	r = GetClassInstance(env, thiz);
	ASSERT(r != NULL);

	rc4_reset(r);
}

static void RawRC4Destroy(JNIEnv *env, jobject thiz)
{
	rc4_t *r;

	LOGPOS();

	r = GetClassInstance(env, thiz);

	if (r != NULL) {
		rc4_destroy(r);
		SetClassInstance(env, thiz, 0);
	}

	return ;
}

void RegisterRawRC4 (JNIEnv *env) {
	static JNINativeMethod methods[]  = {
		{ "RawNew",       "([B)V", (void*)RawRC4New},
		{ "RawXORStream", "([B)V", (void*)RawRC4XORStream},
		{ "RawRC4Reset",  "()V",   (void*)RawRC4Reset },
		{ "RawDestroy",   "()V",   (void*)RawRC4Destroy}
	};
	RegisterNativeMethods(env, "com/mobilepg/crypto/RawRC4", methods, 
		sizeof(methods)/sizeof(methods[0]));
}