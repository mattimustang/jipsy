/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class java_net_PlainDatagramSocketImpl */

#ifndef _Included_java_net_PlainDatagramSocketImpl
#define _Included_java_net_PlainDatagramSocketImpl
#ifdef __cplusplus
extern "C" {
#endif
#undef java_net_PlainDatagramSocketImpl_DEBUG
#define java_net_PlainDatagramSocketImpl_DEBUG 0L
/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    bind
 * Signature: (ILjava/net/InetAddress;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_bind
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    datagramSocketClose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_datagramSocketClose
  (JNIEnv *, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    datagramSocketCreate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_datagramSocketCreate
  (JNIEnv *, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    getOption
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_java_net_PlainDatagramSocketImpl_getOption
  (JNIEnv *, jobject, jint);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    getTTL
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_java_net_PlainDatagramSocketImpl_getTTL
  (JNIEnv *, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    getTimeToLive
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_java_net_PlainDatagramSocketImpl_getTimeToLive
  (JNIEnv *, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    join
 * Signature: (Ljava/net/InetAddress;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_join
  (JNIEnv *, jobject, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    leave
 * Signature: (Ljava/net/InetAddress;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_leave
  (JNIEnv *, jobject, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    peek
 * Signature: (Ljava/net/InetAddress;)I
 */
JNIEXPORT jint JNICALL Java_java_net_PlainDatagramSocketImpl_peek
  (JNIEnv *, jobject, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    receive
 * Signature: (Ljava/net/DatagramPacket;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_receive
  (JNIEnv *, jobject, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    send
 * Signature: (Ljava/net/DatagramPacket;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_send
  (JNIEnv *, jobject, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    setOption
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_setOption
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    setTTL
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_setTTL
  (JNIEnv *, jobject, jbyte);

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    setTimeToLive
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_setTimeToLive
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
