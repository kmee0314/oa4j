/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class at_rocworks_oc4j_jni_DpVCGroup */

#ifndef _Included_at_rocworks_oc4j_jni_DpVCGroup
#define _Included_at_rocworks_oc4j_jni_DpVCGroup
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     at_rocworks_oc4j_jni_DpVCGroup
 * Method:    toDebug
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_at_rocworks_oc4j_jni_DpVCGroup_toDebug
  (JNIEnv *, jobject, jint);

/*
 * Class:     at_rocworks_oc4j_jni_DpVCGroup
 * Method:    getOriginTime
 * Signature: ()Lat/rocworks/oc4j/var/TimeVar;
 */
JNIEXPORT jobject JNICALL Java_at_rocworks_oc4j_jni_DpVCGroup_getOriginTime
  (JNIEnv *, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpVCGroup
 * Method:    setOriginTime
 * Signature: (Lat/rocworks/oc4j/var/TimeVar;)V
 */
JNIEXPORT void JNICALL Java_at_rocworks_oc4j_jni_DpVCGroup_setOriginTime
  (JNIEnv *, jobject, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpVCGroup
 * Method:    insertValueChange
 * Signature: (Lat/rocworks/oc4j/var/DpIdentifierVar;Lat/rocworks/oc4j/var/Variable;)Z
 */
JNIEXPORT jboolean JNICALL Java_at_rocworks_oc4j_jni_DpVCGroup_insertValueChange
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpVCGroup
 * Method:    malloc
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_at_rocworks_oc4j_jni_DpVCGroup_malloc
  (JNIEnv *, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpVCGroup
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_at_rocworks_oc4j_jni_DpVCGroup_free
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif