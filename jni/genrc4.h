/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_mobilepg_crypto_RawRC4 */

#ifndef _Included_com_mobilepg_crypto_RawRC4
#define _Included_com_mobilepg_crypto_RawRC4
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_mobilepg_crypto_RawRC4
 * Method:    RawNew
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_mobilepg_crypto_RawRC4_RawNew
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     com_mobilepg_crypto_RawRC4
 * Method:    RawXORStream
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_mobilepg_crypto_RawRC4_RawXORStream
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     com_mobilepg_crypto_RawRC4
 * Method:    RasDestroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_mobilepg_crypto_RawRC4_RasDestroy
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif