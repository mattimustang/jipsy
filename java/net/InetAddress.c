/*
 * $Id$
 *
 * InetAddress.c - An IPv6 capable version of the InetAddress class.
 *                 This is the native implementation.
 *
 * Copyright (C) 1999 Matthew Flanagan.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Change Log:
 *
 * $Log$
 * Revision 1.2  1999/10/20 07:39:12  mpf
 * - Added CVS Id and Log tags.
 * - Added LGPL license header.
 * - Moved some definitions to net6.h.
 *
 *
 */

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <jni.h>
#include "java_net_InetAddress.h"
#include "net6.h"


/*
 * Class:     java_net_InetAddress
 * Method:    pton
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_java_net_InetAddress_pton(JNIEnv *env, jclass this, jstring host)
{
	jbyte *bytes;
	jbyteArray byteArray;
	char *hostname;
	int len = 0;

	struct sockaddr_storage ss;

	
#ifdef DEBUG
	printf("NATIVE: InetAddress: pton() entering\n");
#endif
	/* convert jstring to char array  */
	hostname = (char *)(*env)->GetStringUTFChars(env, host, NULL);
	
	if (inet_aton(hostname, &((struct sockaddr_in *)&ss)->sin_addr)) {
		bytes = (jbyte *)&((struct sockaddr_in *)&ss)->sin_addr;
		len = 4;
	}

	/* if inet_aton failed len will still be 0. Try IPv6 */
	if (len == 0 && (inet_pton(AF_INET6, hostname, &((struct sockaddr_in6 *)&ss)->sin6_addr) > 0)) {
		bytes = (jbyte *)&((struct sockaddr_in6 *)&ss)->sin6_addr;
		len = 16;
	}
	
	/* wasn't a valid IPv4 or IPv6 address */
	if (len == 0)
		return NULL;

	byteArray = (*env)->NewByteArray(env, len);
	(*env)->SetByteArrayRegion(env, byteArray, 0, len, bytes);
	(*env)->ReleaseStringUTFChars(env, host, hostname);
#ifdef DEBUG
	printf("NATIVE: InetAddress: pton() leaving\n");
#endif
	return byteArray;
}


/*
 * Class:     java_net_InetAddress
 * Method:    getLoopbackHost
 * Signature: ()Ljava/net/InetAddress;
 */
/* this function isn't working and needs lots of work...will come back to it if i have time */
JNIEXPORT jbyteArray JNICALL Java_java_net_InetAddress_getLoopbackAddress
  (JNIEnv *env, jclass thisClass)
{
	
	struct sockaddr_storage *ss;
	struct addrinfo hints;
	struct addrinfo *ai;
	jbyte *addr;
	jbyteArray byteArray;
	int error;
	int len = 0;

#ifdef DEBUG
	printf("NATIVE: getLoopbackAddress() enter\n");
#endif

	/* set up hints structure */
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	/*hints.ai_flags = AI_PASSIVE;*/

#ifdef DEBUG
	printf("NATIVE: getLoopbackAddress(): getaddrinfo()\n");
#endif
	error = getaddrinfo(NULL, AI_SERV, &hints, &ai);
#ifdef DEBUG
	printf("NATIVE: getLoopbackAddress(): getaddrinfo() done\n");
#endif

	if (error != EAI_NONAME) {
#ifdef DEBUG
		perror(gai_strerror(error));
		printf("NATIVE: getLoopbackAddress(): getaddrinfo() error\n");
#endif
		return NULL;
	} 

	/* just return the first one we get */
#ifdef DEBUG
	printf("NATIVE: getLoopbackAddress(): switch()\n");
#endif
	switch (ai->ai_family) {
		case AF_INET:
			addr = (jbyte *)&((struct sockaddr_in *)ai->ai_addr)->sin_addr;
			len = 4;
			break;
		case AF_INET6:
			addr = (jbyte *)&((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr;
			len = 16;
			break;
		default:
#ifdef DEBUG
			printf("NATIVE: getLoopbackAddress(): ai->ai_family = %d\n", ai->ai_family);
#endif
			break;
	}
	if (len == 0) {
#ifdef DEBUG
		printf("NATIVE: getLoopbackAddress(): len == 0\n");
#endif
		return NULL;
	}

#ifdef DEBUG
	printf("NATIVE: getLoopbackAddress(): NewByteArray()\n");
#endif
	byteArray = (*env)->NewByteArray(env, len);
	(*env)->SetByteArrayRegion(env, byteArray, 0, len, addr);
	return byteArray;
}

/*
 * Class:     java_net_InetAddress
 * Method:    getLocalHostName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_java_net_InetAddress_getLocalHostName
  (JNIEnv *env, jclass this)
{
	char *hostname;
	char buf[NI_MAXHOST];

#ifdef DEBUG
	printf("NATIVE: getLocalHostName(): enter\n");
#endif
	if (gethostname(buf, sizeof(buf)))
		return NULL;
	hostname = buf;
#ifdef DEBUG
	printf("NATIVE: getLocalHostName(): leave %s\n", hostname);
#endif
	return (*env)->NewStringUTF(env, hostname);
}

/*
 * Class:     java_net_InetAddress
 * Method:    getAllHostAddresses
 * Signature: (Ljava/lang/String;)[[B
 */
JNIEXPORT jobjectArray JNICALL Java_java_net_InetAddress_getAllHostAddresses
  (JNIEnv *env, jclass this, jstring host)
{
	char *hostname;
	
	struct addrinfo			hints;
	struct addrinfo			*res;
	struct addrinfo			*ressave;
	int err;
	int count = 0;

	jobjectArray 			addressArray;
	jclass 					byteArrayClass;

#ifdef DEBUG
	printf("NATIVE: getAllHostAddresses(): enter\n");
#endif
	hostname = (char *)(*env)->GetStringUTFChars(env, host, NULL);

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	/* we don't want to get duplicate addresses for TCP/UDP 
	 * so we set the hints.ai_socktype to SOCK_STREAM 
	 */
	hints.ai_socktype = SOCK_STREAM;

	err = getaddrinfo(hostname, AI_SERV, &hints, &res);

	if (err) {
		/* get a reference to the UnknownHostException */
		jclass uhe = (*env)->FindClass(env, "java/net/UnknownHostException");
	
		if (uhe == 0)
			return NULL;

		/* throw it */
		(*env)->ThrowNew(env, uhe, "Host Unknown");
		return;
	}

	/* save the result pointer and count the number of addresses returned */
	ressave = res;
	while (res != NULL) {
		count++;
		res = res->ai_next;
	}
#ifdef DEBUG
	printf("NATIVE: getAllHostAddresses(): gai returned %d addresses\n", count);
#endif

	/* allocate storage for addressArray */
	byteArrayClass = (*env)->FindClass(env, "[B");
	addressArray = (*env)->NewObjectArray(env, count, byteArrayClass, NULL);

	/* restore the result pointer and get the addresses out */
	res = ressave;
	count = 0;
	while (res != NULL) {
		jbyte *addressBytes;
		jbyteArray addressBytesArray;
		int addrlen;
		switch (res->ai_family) {
			case AF_INET:
#ifdef DEBUG
	printf("NATIVE: getAllHostAddresses(): res->ai_family == AF_INET\n", count);
#endif
				addressBytes = (jbyte *)&((struct sockaddr_in *)res->ai_addr)->sin_addr;
				addrlen = 4;
#ifdef DEBUG
	printf("NATIVE: getAllHostAddresses(): res->ai_ai_addr = %s\n",
			inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr));
#endif
				break;
			case AF_INET6:
#ifdef DEBUG
	printf("NATIVE: getAllHostAddresses(): res->ai_family == AF_INET6\n", count);
#endif
				addressBytes = (jbyte *)&((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
				addrlen = 16;
				break;

			default:
				break;
		}
		/* now put it in the array */
		addressBytesArray = (*env)->NewByteArray(env,addrlen);
		(*env)->SetByteArrayRegion(env, addressBytesArray, 0,
									addrlen, addressBytes);
		(*env)->SetObjectArrayElement(env, addressArray, count,
										addressBytesArray);

		count++;
		res = res->ai_next;
	} 

	/* free the the result pointer */
	freeaddrinfo(ressave);
#ifdef DEBUG
	printf("NATIVE: getAllHostAddresses(): returning\n");
#endif
	return addressArray;
}

/*
 * Class:     java_net_InetAddress
 * Method:    getHostByAddress
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_java_net_InetAddress_getHostByAddress
  (JNIEnv *env, jclass this, jbyteArray address)
{
	char *hostname;
	char buf[NI_MAXHOST];
	jsize len;
	struct sockaddr_storage ss;
	struct sockaddr_in *sin;
	struct sockaddr_in6 *sin6;
	int err;
	jbyte *addressBytes;

#ifdef DEBUG
	printf("NATIVE: getHostByAddress(): enter\n");
#endif
	len = (*env)->GetArrayLength(env, address);
	addressBytes = (*env)->GetByteArrayElements(env, address, NULL);

	/* got an IPv4 address */
	if (len == 4) {
		ss.__ss_family = AF_INET;
#ifndef SA_LEN
		ss.__ss_len = len;
#endif
		sin = (struct sockaddr_in *)&ss;
		memcpy(&sin->sin_addr, addressBytes, len);
	}

	/* got an IPv6 address */
	if (len == 16) {
		ss.__ss_family = AF_INET6;
#ifndef SA_LEN
		ss.__ss_len = len;
#endif
		
		sin6 = (struct sockaddr_in6 *)&ss;
		memcpy(&sin6->sin6_addr, addressBytes, len);
	}

	err = getnameinfo((struct sockaddr *)&ss, len,
						buf, sizeof(buf), NULL, 0, NI_NAMEREQD);
	/* we got an so throw an exception */
	if (err) {

		/* get a reference to the UnknownHostException */
		jclass uhe = (*env)->FindClass(env, "java/net/UnknownHostException");
	
		if (uhe == 0)
			return NULL;

		/* throw it */
		(*env)->ThrowNew(env, uhe, "Host Unknown");
		return;
	}
	hostname = buf;
#ifdef DEBUG
	printf("NATIVE: getHostByAddress(): returning %s\n", hostname);
#endif
	return (*env)->NewStringUTF(env, hostname);
}
