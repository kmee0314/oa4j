/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class at_rocworks_oc4j_jni_DpIdValueList */

#ifndef _Included_at_rocworks_oc4j_jni_DpIdValueList
#define _Included_at_rocworks_oc4j_jni_DpIdValueList
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    toDebug
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_toDebug
  (JNIEnv *, jobject, jint);

/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    getFirstItem
 * Signature: ()Lat/rocworks/oc4j/jni/DpVCItem;
 */
JNIEXPORT jobject JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_getFirstItem
  (JNIEnv *, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    getNextItem
 * Signature: ()Lat/rocworks/oc4j/jni/DpVCItem;
 */
JNIEXPORT jobject JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_getNextItem
  (JNIEnv *, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    cutFirstItem
 * Signature: ()Lat/rocworks/oc4j/jni/DpVCItem;
 */
JNIEXPORT jobject JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_cutFirstItem
  (JNIEnv *, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    getNumberOfItems
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_getNumberOfItems
  (JNIEnv *, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    appendItem
 * Signature: (Lat/rocworks/oc4j/var/DpIdentifierVar;Lat/rocworks/oc4j/var/Variable;)Z
 */
JNIEXPORT jboolean JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_appendItem
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    malloc
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_malloc
  (JNIEnv *, jobject);

/*
 * Class:     at_rocworks_oc4j_jni_DpIdValueList
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_at_rocworks_oc4j_jni_DpIdValueList_free
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
