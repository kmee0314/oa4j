/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_etm_api_jni_Driver */

#ifndef _Included_com_etm_api_jni_Driver
#define _Included_com_etm_api_jni_Driver
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_etm_api_jni_Driver
 * Method:    apiGetLogPath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_etm_api_jni_Driver_apiGetLogPath
  (JNIEnv *, jobject);

/*
 * Class:     com_etm_api_jni_Driver
 * Method:    apiGetDataPath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_etm_api_jni_Driver_apiGetDataPath
  (JNIEnv *, jobject);

/*
 * Class:     com_etm_api_jni_Driver
 * Method:    apiStartup
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_etm_api_jni_Driver_apiStartup
  (JNIEnv *, jobject, jobjectArray);

#ifdef __cplusplus
}
#endif
#endif
