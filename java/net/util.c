/*
 * $Id$
 *
 * util.c - Utility functions used by native implementations.
 * Copyright (C) 1999 Matthew Flanagan. All Rights Reserved.
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
 * Revision 1.3  1999/11/03 22:30:15  mpf
 * - Updated getSocketFileDescriptor() to handle DatagramSocketImpl too.
 *
 * Revision 1.2  1999/11/01 17:21:34  mpf
 * - Updated includes.
 * - Added getSocketFamily() function.
 *
 * Revision 1.1  1999/10/27 14:40:36  mpf
 * - Initial Import.
 * - most of these functions have moved from PlainSocketImpl.c.
 *
 *
 */

#include <net6.h>

/**
 * get the socket file descriptor from a SocketImpl instance
 * @param env	a pointer to the JNi Environment
 * @param obj	a SocketImpl instance
 * @returns		the socket file descriptor or -1 if an error occurred
 */
int getSocketFileDescriptor(JNIEnv *env, jobject obj)
{
	jclass class, socketImplClass, datagramSocketImplClass, fdClass;
	jobject fdObj;
	jfieldID fdID, fdfdID;
	jint sockfd = -1;

	/* first check that were were passed the correct object class */
	socketImplClass = (*env)->FindClass(env, "java/net/SocketImpl");
	datagramSocketImplClass = (*env)->FindClass(env, "java/net/DatagramSocketImpl");
	if ((obj != NULL)
		&& ((*env)->IsInstanceOf(env, obj, socketImplClass) == JNI_TRUE || ((*env)->IsInstanceOf(env, obj, datagramSocketImplClass) == JNI_TRUE))) {

		class = (*env)->GetObjectClass(env, obj);

		fdID = (*env)->GetFieldID(env, class, "fd", "Ljava/io/FileDescriptor;");
		fdObj = (*env)->GetObjectField(env, obj, fdID);
		fdClass = (*env)->GetObjectClass(env, fdObj);

		fdfdID = (*env)->GetFieldID(env, fdClass, "fd", "I");
		sockfd = (*env)->GetIntField(env, fdObj, fdfdID);
	}

	return sockfd;
}

/**
 * Throw an exception
 * @param env	pointer to JNI Enviroment
 * @param type	the Exception type as defined in net6.h
 * @param msg	the reason for the exception
 */
void throwException(JNIEnv *env, int type, const char *msg)
{
	jclass exception;
#ifdef DEBUG
	printf("NATIVE: throwException(): entering\n");
#endif
	switch (type) {
		case EX_IO:
			exception = (*env)->FindClass(env, "java/io/IOException");
			break;
		case EX_BIND:
			exception = (*env)->FindClass(env, "java/net/BindException");
			break;
		case EX_CONNECT:
			exception = (*env)->FindClass(env, "java/net/ConnectException");
			break;
		case EX_SOCKET:
			exception = (*env)->FindClass(env, "java/net/ConnectException");
			break;
		default:
			exception = 0;
			break;
	}
	if (exception == 0)
		return;

	/* throw it */
	(*env)->ThrowNew(env, exception, msg);
#ifdef DEBUG
	printf("NATIVE: throwException(): returning\n");
#endif
	return;
}

/**
 * Get the port number out of the socket address structure
 * @param ss	a pointer to the socket address structure
 * @returns		the port number
 */
int getSockAddrPort(struct sockaddr_storage *ss)
{
#ifdef DEBUG
	printf("NATIVE: getSocketPort(): entering\n");
#endif
	switch (ss->__ss_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
#ifdef DEBUG
	printf("NATIVE: getSocketPort(): returning INET\n");
#endif
			return (sin->sin_port);
		}
		case  AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
#ifdef DEBUG
	printf("NATIVE: getSocketPort(): returning INET6\n");
#endif
			return (sin6->sin6_port);
		}
	}
#ifdef DEBUG
	printf("NATIVE: getSocketPort(): returning -1\n");
#endif
	return -1;
}

/* change a sockets family to that specifed in the af arg */
int setSocketFamily(int *sockfd, int af)
{
	struct sockaddr_storage ss;
	int len;
#ifdef DEBUG
	printf("NATIVE: setSocketFamily(): entering af = %d\n", af);
#endif

	if (getsockname(*sockfd, (struct sockaddr *)&ss, &len) == 0) {
#ifdef DEBUG
	printf("NATIVE: setSocketFamily(): sockfd family = %d\n", ss.__ss_family);
#endif
	}

	if (af == ss.__ss_family)
		return 0;

	if (setsockopt(*sockfd, IPPROTO_IPV6, IPV6_ADDRFORM, &af, sizeof(af)) < 0)
		return -1;
	if (getsockname(*sockfd, (struct sockaddr *)&ss, &len) == 0) {
#ifdef DEBUG
	printf("NATIVE: setSocketFamily(): sockfd new family = %d\n", ss.__ss_family);
#endif
	}
#ifdef DEBUG
	printf("NATIVE: setSocketFamily(): returning\n");
#endif
	return 0;
}

int getSocketFamily(int sockfd)
{
	struct sockaddr_storage ss;
	int len = sizeof(ss);

	if (getsockname(sockfd, (struct sockaddr *)&ss, &len) == 0) {
		return ss.__ss_family;
	}

	return -1;
} 
