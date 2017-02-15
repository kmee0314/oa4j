/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_etm_api_jni_DpVCGroup */

#ifndef _Included_com_etm_api_jni_DpVCGroup
#define _Included_com_etm_api_jni_DpVCGroup
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_etm_api_jni_DpVCGroup
 * Method:    toDebug
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_etm_api_jni_DpVCGroup_toDebug
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_etm_api_jni_DpVCGroup
 * Method:    getOriginTime
 * Signature: ()Lcom/etm/api/var/TimeVar;
 */
JNIEXPORT jobject JNICALL Java_com_etm_api_jni_DpVCGroup_getOriginTime
  (JNIEnv *, jobject);

/*
 * Class:     com_etm_api_jni_DpVCGroup
 * Method:    setOriginTime
 * Signature: (Lcom/etm/api/var/TimeVar;)V
 */
JNIEXPORT void JNICALL Java_com_etm_api_jni_DpVCGroup_setOriginTime
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_etm_api_jni_DpVCGroup
 * Method:    insertValueChange
 * Signature: (Lcom/etm/api/var/DpIdentifierVar;Lcom/etm/api/var/Variable;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_etm_api_jni_DpVCGroup_insertValueChange
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     com_etm_api_jni_DpVCGroup
 * Method:    malloc
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_etm_api_jni_DpVCGroup_malloc
  (JNIEnv *, jobject);

/*
 * Class:     com_etm_api_jni_DpVCGroup
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_etm_api_jni_DpVCGroup_free
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
