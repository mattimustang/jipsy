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
 * Revision 1.11  2000/05/13 05:55:00  mpf
 * - Changed pton() native method to stringToAddress()
 *
 * Revision 1.10  1999/12/06 07:03:10  mpf
 * - Added #ifdef DEBUG around some code that was missing it.
 *
 * Revision 1.9  1999/11/30 14:41:01  mpf
 * - Fixed bug in getAllHostAddresses() where the count would include
 *   addresses other than AF_INET* like AF_UNIX.
 *
 * Revision 1.8  1999/11/09 12:05:02  mpf
 * - Modifed to use new macros defined in net6.h
 *
 * Revision 1.7  1999/11/01 16:55:04  mpf
 * - Modified #includes to reflect their new location.
 *
 * Revision 1.6  1999/10/27 14:28:49  mpf
 * - Cleaned up unused and unititialised variables.
 *
 * Revision 1.5  1999/10/26 18:28:23  mpf
 * - Removed some of the more verbose debugging messages.
 *
 * Revision 1.4  1999/10/26 17:16:57  mpf
 * - Fixed getLoopBackAddress().
 * - Added getAnyLocalAddress() native function implementation.
 * - General clean up of code.
 *
 * Revision 1.3  1999/10/20 22:55:53  mpf
 * - Cleaned up #includes and moved them all to net6.h.
 *
 * Revision 1.2  1999/10/20 07:39:12  mpf
 * - Added CVS Id and Log tags.
 * - Added LGPL license header.
 * - Moved some definitions to net6.h.
 *
 *
 */

#include <net6.h>
#include <InetAddress.h>


/*
 * Class:     java_net_InetAddress
 * Method:    stringToAddress
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_java_net_InetAddress_stringToAddress(JNIEnv *env, jclass this, jstring host)
{
	jbyte *bytes = NULL;
	jbyteArray byteArray;
	char *hostname;
	int len = 0;

	struct sockaddr_storage ss;

	
#ifdef DEBUG
	printf("NATIVE: InetAddress: stringToAddress() entering\n");
#endif
	/* convert jstring to char array  */
	hostname = (char *)(*env)->GetStringUTFChars(env, host, NULL);
	
	if (inet_aton(hostname, &((struct sockaddr_in *)&ss)->sin_addr)) {
		bytes = (jbyte *)&((struct sockaddr_in *)&ss)->sin_addr;
		len = IPV4_ADDRLEN;
	}

	/* if inet_aton failed len will still be 0. Try IPv6 */
	if (len == 0 && (inet_pton(AF_INET6, hostname, &((struct sockaddr_in6 *)&ss)->sin6_addr) > 0)) {
		bytes = (jbyte *)&((struct sockaddr_in6 *)&ss)->sin6_addr;
		len = IPV6_ADDRLEN;
	}
	
	/* wasn't a valid IPv4 or IPv6 address */
	if (len == 0)
		return NULL;

	byteArray = (*env)->NewByteArray(env, len);
	(*env)->SetByteArrayRegion(env, byteArray, 0, len, bytes);
	(*env)->ReleaseStringUTFChars(env, host, hostname);
#ifdef DEBUG
	printf("NATIVE: InetAddress: stringToAddress() leaving\n");
#endif
	return byteArray;
}

/*
 * Class:     java_net_InetAddress
 * Method:    getAnyLocalAddress
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_java_net_InetAddress_getAnyLocalAddress
  (JNIEnv *env, jclass thisClass)
{
	
	struct addrinfo hints;
	struct addrinfo *res, *reshead;
	jbyte *addr = NULL;
	jbyteArray byteArray;
	int error;
	int len = 0;

#ifdef DEBUG
	printf("NATIVE: getAnyLocalAddress() enter\n");
#endif

	/* set up hints structure */
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	error = getaddrinfo(HOST_NULL, SERV_ZERO, &hints, &res);
	reshead = res;

	if (error) {
		freeaddrinfo(reshead);
		return NULL;
	} 

	/* loop through results and find an address we can actually use */
	do {
		int fd = -1;

		switch (res->ai_family)
		{
			case AF_INET:
			case AF_INET6:
				fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
				break;
			default:
				break;
		}
		if (fd >= 0) {
			close(fd);
			break;
		}
	} while ((res = res->ai_next) != NULL);

	/* just return the first valid address we get */
	switch (res->ai_family) {
		case AF_INET:
			addr = (jbyte *)&((struct sockaddr_in *)res->ai_addr)->sin_addr;
			len = IPV4_ADDRLEN;
			break;
		case AF_INET6:
			addr = (jbyte *)&((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
			len = IPV6_ADDRLEN;
			break;
		default:
			break;
	}
	if (len == 0) {
		freeaddrinfo(reshead);
		return NULL;
	}

	byteArray = (*env)->NewByteArray(env, len);
	(*env)->SetByteArrayRegion(env, byteArray, 0, len, addr);

	freeaddrinfo(reshead);
#ifdef DEBUG
	printf("NATIVE: getAnyLocalAddress(): returning\n");
#endif
	return byteArray;
}

/*
 * Class:     java_net_InetAddress
 * Method:    getLoopbackHost
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_java_net_InetAddress_getLoopbackAddress
  (JNIEnv *env, jclass thisClass)
{
	
	struct addrinfo hints;
	struct addrinfo *res, *reshead;
	jbyte *addr = NULL;
	jbyteArray byteArray;
	int error;
	int len = 0;

#ifdef DEBUG
	printf("NATIVE: getLoopbackAddress() enter\n");
#endif

	/* set up hints structure */
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(HOST_NULL, SERV_ZERO, &hints, &res);
	reshead = res;

	if (error) {
		freeaddrinfo(reshead);
		return NULL;
	} 

	/* loop through results and find an address we can actually use */
	do {
		int fd = -1;

		switch (res->ai_family)
		{
			case AF_INET:
			case AF_INET6:
				fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
				break;
			default:
				break;
		}
		if (fd >= 0) {
			close(fd);
			break;
		}
	} while ((res = res->ai_next) != NULL);

	/* just return the first valid address we get */
	switch (res->ai_family) {
		case AF_INET:
			addr = (jbyte *)&((struct sockaddr_in *)res->ai_addr)->sin_addr;
			len = IPV4_ADDRLEN;
			break;
		case AF_INET6:
			addr = (jbyte *)&((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
			len = IPV6_ADDRLEN;
			break;
		default:
			break;
	}
	if (len == 0) {
		freeaddrinfo(reshead);
		return NULL;
	}

	byteArray = (*env)->NewByteArray(env, len);
	(*env)->SetByteArrayRegion(env, byteArray, 0, len, addr);

	freeaddrinfo(reshead);
#ifdef DEBUG
	printf("NATIVE: getLoopbackAddress(): returning\n");
#endif
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
	struct addrinfo			*res, *reshead;
	int err;
	int count = 0;

	jobjectArray 			addressArray;
	jclass 					byteArrayClass;

#ifdef DEBUG
	printf("NATIVE: InetAddress.getAllHostAddresses(): enter\n");
#endif
	hostname = (char *)(*env)->GetStringUTFChars(env, host, NULL);

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	/* we don't want to get duplicate addresses for TCP/UDP 
	 * so we set the hints.ai_socktype to SOCK_STREAM 
	 */
	/*hints.ai_socktype = SOCK_STREAM;*/

	err = getaddrinfo(hostname, NULL, &hints, &res);

	if (err) {
		/* get a reference to the UnknownHostException */
		jclass uhe = (*env)->FindClass(env, "java/net/UnknownHostException");
	
		if (uhe == 0)
			return NULL;

		/* throw it */
		(*env)->ThrowNew(env, uhe, gai_strerror(err));
		return NULL;
	}

	/* save the result pointer and count the number of addresses returned */
	reshead = res;
	while (res != NULL) {
		switch (res->ai_family) {
			case AF_INET:
			case AF_INET6:
				count++;
				break;
			default:
				break;
		}
		res = res->ai_next;
	}

#ifdef DEBUG
	printf("NATIVE: InetAddress.getAllHostAddresses(): %d address returned\n",count);
#endif
	/* allocate storage for addressArray */
	byteArrayClass = (*env)->FindClass(env, "[B");
	addressArray = (*env)->NewObjectArray(env, count, byteArrayClass, NULL);

	/* restore the result pointer and get the addresses out */
	res = reshead;
	count = 0;
	while (res != NULL) {
		jbyte *addressBytes = NULL;
		jbyteArray addressBytesArray;
		int addrlen = 0;
#ifdef DEBUG
		char buf[INET6_ADDRSTRLEN];
#endif
		switch (res->ai_family) {
			case AF_INET:
				addressBytes = (jbyte *)&((struct sockaddr_in *)res->ai_addr)->sin_addr;
				addrlen = IPV4_ADDRLEN;
#ifdef DEBUG
	printf("NATIVE: InetAddress.getAllHostAddresses: address %d is %s\n",count,
			inet_ntop(AF_INET, addressBytes, buf, sizeof(buf)));
#endif
				break;
			case AF_INET6:
				addressBytes = (jbyte *)&((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
#ifdef DEBUG
	printf("NATIVE: InetAddress.getAllHostAddresses: address %d is %s\n",count,
			inet_ntop(AF_INET6, addressBytes, buf, sizeof(buf)));
#endif
				addrlen = IPV6_ADDRLEN;
				break;
			default:
				break;
		}
		if (addressBytes != NULL) {
			/* now put it in the array */
			addressBytesArray = (*env)->NewByteArray(env,addrlen);
			(*env)->SetByteArrayRegion(env, addressBytesArray, 0,
										addrlen, addressBytes);
			(*env)->SetObjectArrayElement(env, addressArray, count,
											addressBytesArray);

			count++;
		}
		res = res->ai_next;
	} 

	/* free the the result pointer */
	freeaddrinfo(reshead);
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
	if (len == IPV4_ADDRLEN) {
		SS_FAMILY(&ss) = AF_INET;
		sin = (struct sockaddr_in *)&ss;
		memcpy(&sin->sin_addr, addressBytes, len);
	}

	/* got an IPv6 address */
	if (len == IPV6_ADDRLEN) {
		SS_FAMILY(&ss) = AF_INET6;
		
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
		return NULL;
	}
	hostname = buf;
#ifdef DEBUG
	printf("NATIVE: getHostByAddress(): returning %s\n", hostname);
#endif
	return (*env)->NewStringUTF(env, hostname);
}
