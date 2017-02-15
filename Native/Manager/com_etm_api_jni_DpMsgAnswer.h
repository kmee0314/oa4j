/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_etm_api_jni_DpMsgAnswer */

#ifndef _Included_com_etm_api_jni_DpMsgAnswer
#define _Included_com_etm_api_jni_DpMsgAnswer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_etm_api_jni_DpMsgAnswer
 * Method:    insertGroup
 * Signature: (Lcom/etm/api/jni/AnswerGroup;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_etm_api_jni_DpMsgAnswer_insertGroup
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_etm_api_jni_DpMsgAnswer
 * Method:    setOutstandingProgress
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_etm_api_jni_DpMsgAnswer_setOutstandingProgress
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_etm_api_jni_DpMsgAnswer
 * Method:    newFromMsg
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_etm_api_jni_DpMsgAnswer_newFromMsg
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_etm_api_jni_DpMsgAnswer
 * Method:    newFromMsgAnswer
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_etm_api_jni_DpMsgAnswer_newFromMsgAnswer
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
