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
 * Revision 1.4  1999/11/08 13:27:37  mpf
 * - Removed unnecessary debugging code.
 * - Removed unused setSocketFamily() function.
 * - General clean up of code.
 *
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
 * @param env	a pointer to the JNI Environment
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
		&& (((*env)->IsInstanceOf(env, obj, socketImplClass) == JNI_TRUE)
			|| ((*env)->IsInstanceOf(env, obj, datagramSocketImplClass) == JNI_TRUE))) {

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
		case EX_INT_IO:
			exception = (*env)->FindClass(env, "java/io/InterruptedIOException");
			break;
		case EX_ILLEGAL_ARG:
			exception = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
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
 * @returns		the port number or -1 if an error occurred
 */
int getSockAddrPort(struct sockaddr_storage *ss)
{
	switch (SS_FAMILY(ss)) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
			return (sin->sin_port);
		}
		case  AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
			return (sin6->sin6_port);
		}
	}
	return -1;
}

/**
 * Get the address family of a socket file descriptor.
 * @param sockfd	a socket file descriptor
 * @returns			the address family of the socket or -1 if an error occurred
 */
int getSocketFamily(int sockfd)
{
	struct sockaddr_storage ss;
	int len = sizeof(ss);

	if (getsockname(sockfd, (struct sockaddr *)&ss, &len) == 0) {
		return SS_FAMILY(&ss);
	}

	return -1;
} 
