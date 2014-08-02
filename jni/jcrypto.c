#include <jni.h>
#include <stdio.h>

#include "debug.h"
#include "jutil.h"
#include "aes_t.h"
#include "bs_t.h"
#include "rc4_t.h"
#include "ffmt_t.h"

void RegisterRawRC4 (JNIEnv *env);
void RegisterRawAES (JNIEnv *env);
void RegisterCipherFormat (JNIEnv *env);

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNIEnv *env;

	LOGPOS();

	if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("Get Env Failed");
		return JNI_ERR;
	}

	// RegisterRawAES(env);
	RegisterRawRC4(env);
	RegisterCipherFormat(env);

	return JNI_VERSION_1_4;
}

/* =============================== RawAES ================================== */

/*
 * Signature: ()Z
 */
static jboolean 
RawAESNew(JNIEnv *env, jobject thiz)
{
	LOGPOS();

	if (GetClassInstance(env, thiz) != 0) {
		LOGW("The instance is not NULL, do we RECREATE it?");
	}

	SetClassInstance(env, thiz, aes_new());
}

/*
 * Signature: ([B)Z
 */
static jboolean 
RawAESSetupEncKey(JNIEnv *env, jobject thiz, jbyteArray ba)
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
static jbyteArray 
RawAESEncBuf(JNIEnv *env, jobject thiz, jbyteArray ba)
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
	BSSetToJByteArray(env, c, ret);

	ReleaseJBASetToBS(env, ba, p);
	bs_destroy(p);
	bs_destroy(c);

	return ret;
}

/*
 * Signature: ([B)Z
 */
static jboolean 
RawAESSetupDecKey(JNIEnv *env, jobject thiz, jbyteArray ba)
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
static jbyteArray 
RawAESDecBuf(JNIEnv *env, jobject thiz, jbyteArray ba)
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
	BSSetToJByteArray(env, p, ret);

	ReleaseJBASetToBS(env, ba, c);
	bs_destroy(p);
	bs_destroy(c);

	return ret;
}

/*
 * Signature: ()V
 */
static void 
RawAESDestroy(JNIEnv *env, jobject thiz)
{
	aes_t *aes;

	LOGPOS();

	aes = (aes_t*)GetClassInstance(env, thiz);
	if (aes != NULL) {
		aes_destory(aes);
		SetClassInstance(env, thiz, 0);
	}
}

void 
RegisterRawAES (JNIEnv *env) {
	LOGPOS();

	static JNINativeMethod methods[]  = {
		{ "RawNew",         "()Z",    (void*)RawAESNew },
		{ "RawSetupEncKey", "([B)Z",  (void*)RawAESSetupEncKey },
		{ "RawEncBuf",      "([B)[B", (void*)RawAESEncBuf },
		{ "RawSetupDecKey", "([B)Z",  (void*)RawAESSetupDecKey },
		{ "RawDecBuf",      "([B)[B", (void*)RawAESDecBuf },
		{ "RawDestroy",     "()V",    (void*)RawAESDestroy },
	};
	RegisterNativeMethods(env, "com/mcs/crypto/RawAES", methods, 
		sizeof(methods)/sizeof(methods[0]));
}

/* =============================== RawRC4 ================================== */

static jint 
RawRC4New(JNIEnv *env, jobject thiz, jbyteArray ba) 
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

	return 0;
}

static jint
RawRC4XORStream(JNIEnv *env, jobject thiz, jbyteArray ba)
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

	return 0;
}

static void 
RawRC4Reset(JNIEnv *env, jobject thiz)
{
	rc4_t *r;

	LOGPOS();

	r = GetClassInstance(env, thiz);
	ASSERT(r != NULL);

	rc4_reset(r);
}

static void 
RawRC4Destroy(JNIEnv *env, jobject thiz)
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

void 
RegisterRawRC4 (JNIEnv *env) 
{
	LOGPOS();

	static JNINativeMethod methods[]  = {
		{ "RawNew",       "([B)I", (void*)RawRC4New},
		{ "RawXORStream", "([B)I", (void*)RawRC4XORStream},
		{ "RawReset",     "()V",   (void*)RawRC4Reset },
		{ "RawDestroy",   "()V",   (void*)RawRC4Destroy}
	};
	RegisterNativeMethods(env, "com/mcs/crypto/RawRC4", methods, 
		sizeof(methods)/sizeof(methods[0]));
}

/* ============================= CipherFormat ================================ */

static void 
RawCipherFormatNew(JNIEnv *env, jobject thiz)
{
	ffmt_t *ffmt;

	LOGPOS();

	ffmt = ffmt_new();
	if (ffmt == NULL) {
		LOGW("Error alloc memory for ffmt!");
		return ;
	}

	SetClassInstance(env, thiz, ffmt);
} 

static int32_t 
RawCipherFormatEncrypt(JNIEnv *env, jclass thiz, jbyteArray jfnplain, 
	jbyteArray jfncipher, jbyteArray jkey)
{
	ffmt_t *ffmt;
	bs_t *fncipher, *fnplain, *key;
	errno_t err = CPT_ERROR;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);

	fncipher = bs_new();
	if (fncipher == NULL) {
		LOGW("Error when alloc memory for fncipher!");
		goto cleanup;
	}
	JByteArrayCopyToBS(env, jfncipher, fncipher);

	fnplain = bs_new();
	if (fnplain == NULL) {
		LOGW("Error when alloc memory for fnplain!");
		goto cleanup;
	}
	JByteArrayCopyToBS(env, jfnplain, fnplain);

	key = bs_new();
	if (key == NULL) {
		LOGW("Error when alloc memory for key!");
		goto cleanup;
	}
	JByteArrayCopyToBS(env, jkey, key);

	err = ffmt_encrypt_file(ffmt, fnplain, fncipher, key);

cleanup:
	if (fncipher != NULL)
		bs_destroy(fncipher);

	if (fnplain != NULL)
		bs_destroy(fnplain);

	if (key != NULL)
		bs_destroy(key);

	return err;
}

static jbyteArray 
RawCipherFormatDecrypt(JNIEnv *env, jclass thiz, jbyteArray jfncipher)
{
	ffmt_t *ffmt;
	bs_t   *fncipher;
	bs_t   *fname;
	jbyteArray ret = NULL;
	errno_t err = CPT_ERROR;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);

	fncipher = bs_new();
	if (fncipher == NULL) {
		LOGW("Error when alloc memory for fncipher!");
		goto cleanup;
	}
	JByteArrayCopyToBS(env, jfncipher, fncipher);

	err = ffmt_decrypt_file(ffmt, fncipher);
	if (err != CPT_OK) {
		LOGW("Error  when decrypt file!");
		goto cleanup;
	}
	fname = bs_new();
	if (fname == NULL) {
		LOGW("Error when alloc memory for fname!");
		goto cleanup;
	}
	err = ffmt_get_filename(ffmt, fname);
	if (err != CPT_OK) {
		LOGW("Error when get filename!");
		goto cleanup;
	}
	LOGI("a");
	LOGI(fname->data);
	ret = (*env)->NewByteArray(env, fname->len);
	BSSetToJByteArray(env, fname, ret);
	fname->data = NULL;

cleanup:
	if (fncipher != NULL) {
		bs_destroy(fncipher);
	}

	if (fname != NULL) {
		bs_destroy(fname);
	}

	return ret;
}

static jint
RawCipherFormatSetHeader(JNIEnv *env, jobject thiz, jint type, jbyteArray jbytes)
{
	ffmt_t  *ffmt;
	bs_t    *bs;
	errno_t  err;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);

	if (ffmt == NULL) {
		LOGW("ffmt is NULL.");
		return CPT_ERROR;
	}

	bs = bs_new();
	if (bs == NULL) {
		return CPT_ERROR;
	}

	JByteArrayCopyToBS(env, jbytes, bs);

	err = ffmt_set_header(ffmt, type, bs);

	bs_destroy(bs);

	return err;
}

static jbyteArray 
RawCipherFormatGetHeader(JNIEnv *env, jobject thiz, jint type)
{
	ffmt_t  *ffmt;
	bs_t    *bs;
	errno_t  err;
	jbyteArray ret = NULL;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);

	if (ffmt == NULL) {
		LOGW("ffmt is NULL.");
		return NULL;
	}

	bs = bs_new();
	if (bs == NULL) {
		return NULL;
	}

	err = ffmt_get_header(ffmt, type, bs);

	if (err == CPT_OK) {
		ret = (*env)->NewByteArray(env, bs->len);
		BSSetToJByteArray(env, bs, ret);
		bs->data = NULL;
	}

	bs_destroy(bs);

	return ret;
}

static jint 
RawCipherFormatSetFileKey(JNIEnv *env, jobject thiz, jbyteArray jkey)
{
	bs_t   *key;
	ffmt_t *ffmt;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);

	key = bs_new();
	if (key == NULL) {
		LOGW("Error alloc memory for key!");
		return CPT_ERROR;
	}
	JByteArrayCopyToBS(env, jkey, key);

	ffmt_set_filekey(ffmt, key);

	bs_destroy(key);

	return CPT_OK;
}

static jbyteArray 
RawCipherFormatGetFileKey(JNIEnv *env, jobject thiz) 
{
	ffmt_t  *ffmt;
	bs_t    *key;
	errno_t  err;
	jbyteArray ret = NULL;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);

	key = bs_new();
	if (key == NULL) {
		LOGW("Error alloc memory for key!");
		return NULL;
	}
	err = ffmt_get_filekey(ffmt, key);

	if (err == CPT_OK) {
		ret = (*env)->NewByteArray(env, key->len);
		BSSetToJByteArray(env, key, ret);
		key->data = NULL;
	}

	bs_destroy(key);

	return ret;
}

jint
RawCipherFormatLoadHeaders(JNIEnv *env, jobject thiz, jbyteArray jfname)
{
	ffmt_t *ffmt;
	bs_t   *fname;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);

	fname = bs_new();
	if (fname == NULL) {
		return CPT_ERROR;
	}
	JByteArrayCopyToBS(env, jfname, fname);

	return ffmt_load_headers(ffmt, fname);
}

static void 
RawCipherFormatDestroy(JNIEnv *env, jobject thiz) 
{
	ffmt_t *ffmt;

	LOGPOS();

	ffmt = GetClassInstance(env, thiz);
	if (ffmt != NULL) {
		ffmt_destroy(ffmt);
	}
}

void 
RegisterCipherFormat(JNIEnv *env)
{
	LOGPOS();

	static JNINativeMethod methods[] = {
		{"RawNew",        "()V",       (void*)RawCipherFormatNew },
		{"RawEncrypt",    "([B[B[B)I", (void*)RawCipherFormatEncrypt },
		{"RawDecrypt",    "([B)[B",     (void*)RawCipherFormatDecrypt },
		{"RawSetHeader",  "(I[B)I",    (void*)RawCipherFormatSetHeader },
		{"RawGetHeader",  "(I)[B",     (void*)RawCipherFormatGetHeader },
		{"RawSetFileKey", "([B)I",     (void*)RawCipherFormatSetFileKey },
		{"RawGetFileKey", "()[B",      (void*)RawCipherFormatGetFileKey },
		{"RawLoadHeaders", "([B)I",       (void*)RawCipherFormatLoadHeaders },
		{"RawDestroy",    "()V",       (void*)RawCipherFormatDestroy }
	};
	RegisterNativeMethods(env, "com/mcs/crypto/CipherFormat", methods,
		sizeof(methods)/sizeof(methods[0]));
}