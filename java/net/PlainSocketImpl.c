/*
 * $Id$
 *
 * PlainSocketImpl.c - The native functions for the  IPv6 capable version of
 *                     the PlainSocketImpl class.
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
 * Revision 1.1  1999/10/20 23:08:16  mpf
 * Initial Import.
 * - Can almost connect using IPv6 (connects but socket closes straight way).
 * - Still some problems with create, and connect, other functions nots tested yet.
 *
 *
 */

#include "net6.h"
#include "java_net_PlainSocketImpl.h"


/*
 * Class:     java_net_PlainSocketImpl
 * Method:    bind
 * Signature: (Ljava/net/InetAddress;I)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_bind
  (JNIEnv *env, jobject this, jobject host, jint port)
{
	struct sockaddr_storage 	ss;
	struct sockaddr_in			*sin;
	struct sockaddr_in6			*sin6;
	jbyte *addrBytes = NULL;
	jbyteArray addrByteArray;
	int addrlen = 0;
	int len = 0;
	int err;

	int fdnum = get_fdnum(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind(): entering\n");
#endif
	/* if host is not null then bind to the supplied address */
	if (host != NULL)  {
		jclass hostClass = (*env)->GetObjectClass(env, host);
		jfieldID addrID = (*env)->GetFieldID(env, hostClass, "address", "[B");
		addrByteArray = (jbyteArray)(*env)->GetObjectField(env, host, addrID);
		addrlen = (*env)->GetArrayLength(env, addrByteArray);
		addrBytes = (*env)->GetByteArrayElements(env, addrByteArray, NULL);
	}

	/* were given an IPv4 address */
	if (addrlen == 4) {
		sin = (struct sockaddr_in *)&ss;
		sin->sin_family = AF_INET;
		memcpy(&sin->sin_addr, addrBytes, addrlen);
		sin->sin_port = htons(port);
		len = sizeof(struct sockaddr_in);

	/* an IPv6 address */
	} else if (addrlen == 16) {
		sin6 = (struct sockaddr_in6 *)&ss;
		sin6->sin6_family = AF_INET6;
		memcpy(&sin6->sin6_addr, addrBytes, addrlen);
		sin6->sin6_port = htons(port);
		len = sizeof(struct sockaddr_in6);

	} else if (addrlen == 0 && host != NULL) {
		goto error;
	}
	if (bind(fdnum, (struct sockaddr *)&ss, len)) {
		jint lport;
		jclass thisClass = (*env)->GetObjectClass(env, this);
		/* get the this.address field */
		jfieldID addrID = (*env)->GetFieldID(env, thisClass, "address",
											"Ljava/net/InetAddress;");
		jobject addrObject = (*env)->GetObjectField(env, this, addrID);
		/* get the this.localport field */
		jfieldID localportID = (*env)->GetFieldID(env, thisClass, "localport", "I");

		/* set this.address to host */
		(*env)->SetObjectField(env, addrObject, addrID, host);

		if (port != 0) {
			/* set this.localport to port */
			lport = port;
		} else if ((getsockname(fdnum, (struct sockaddr *)&ss, &len)) == 0) {
			/* set this.localport to ss.sin_port */
			lport = ntohs(getSocketPort(&ss));
		} else {
			goto error;
		}
		(*env)->SetIntField(env, this, localportID, lport);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind(): returning\n");
#endif
		return;

	}
	error:
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind(): throwing\n");
#endif
		throwException(env, EX_BIND, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    listen
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_listen
  (JNIEnv *env, jobject this, jint backlog)
{
	int fdnum = get_fdnum(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.listen(): entering\n");
#endif
	if (listen(fdnum, backlog) != 0) {
		throwException(env, EX_IO, strerror(errno));
	}
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.listen(): returning\n");
#endif
	return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    accept
 * Signature: (Ljava/net/SocketImpl;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_accept
  (JNIEnv *env, jobject this, jobject impl)
{
	struct sockaddr_storage ss;
	socklen_t addrlen = sizeof(ss);
	int sock;
	jbyteArray remoteAddress;
	jbyte *remoteAddressBytes;

	jclass implClass, thisClass, inetClass, fdClass;
	jfieldID localportID, portID, addressID, fdID, fdnumID, fdfdID;
	jfieldID thisLocalportID;
	jint localport, remotePort, thisLocalport;
	jobject newInetAddress, newfd;
	jmethodID inetMethod, fdMethod;
	
	int fdnum = get_fdnum(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.accept(): entering\n");
#endif
	sock = accept(fdnum, (struct sockaddr *)&ss, &addrlen);

	if (sock < 0)
		goto error;

	switch (ss.__ss_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
			remoteAddress = (*env)->NewByteArray(env, 4);
			remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
			memcpy(remoteAddressBytes, &sin->sin_addr, 4);
			(*env)->SetByteArrayRegion(env, remoteAddress, 0, 4, remoteAddressBytes);
			remotePort = ntohs(sin->sin_port);
			break;
		}
		case AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
			remoteAddress = (*env)->NewByteArray(env, 16);
			remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
			memcpy(remoteAddressBytes, &(sin6->sin6_addr), 16);
			(*env)->SetByteArrayRegion(env, remoteAddress, 0, 16, remoteAddressBytes);
			remotePort = ntohs(sin6->sin6_port);
			break;
		}
		default:
			goto error;
	}

	implClass = (*env)->GetObjectClass(env, impl);
	fdnumID = (*env)->GetFieldID(env, implClass, "fdnum", "I");
	localportID = (*env)->GetFieldID(env, implClass, "localport", "I");
	portID = (*env)->GetFieldID(env, implClass, "port", "I");
	addressID = (*env)->GetFieldID(env, implClass, "address", "Ljava/net/InetAddress;");
	fdID = (*env)->GetFieldID(env, implClass, "fd", "Ljava/io/FileDescriptor;");

	/* impl.fdnum = sock; */
	(*env)->SetIntField(env, impl, fdnumID, sock);
	
	/* impl.localport = this.localport; */
	thisClass = (*env)->GetObjectClass(env, this);
	thisLocalportID = (*env)->GetFieldID(env, thisClass, "localport", "I");
	thisLocalport = (*env)->GetIntField(env, this, thisLocalportID);
	(*env)->SetIntField(env, impl, thisLocalportID, thisLocalport);

	/* impl.address = new InetAddress(null, remoteAddress); */
	inetClass = (*env)->FindClass(env, "java/net/InetAddress");
	inetMethod = (*env)->GetMethodID(env, inetClass, "<init>", "(Ljava/lang/String;[B)V");
	newInetAddress = (*env)->NewObject(env, inetClass, inetMethod, NULL, remoteAddress);
	(*env)->SetObjectField(env, impl, addressID, newInetAddress); 

	/* impl.port = remotePort; */
	(*env)->SetIntField(env, impl, portID, remotePort);

	/* impl.fd = new java.io.FileDescriptor(sock); */
	fdClass = (*env)->FindClass(env, "java/io/FileDescriptor");
	fdMethod = (*env)->GetMethodID(env, fdClass, "<init>", "()V");
	newfd = (*env)->NewObject(env, fdClass, fdMethod);

	/* fd.fd = sock */
	fdfdID = (*env)->GetFieldID(env, fdClass, "fd", "I");
	(*env)->SetIntField(env, newfd, fdfdID, sock);

	/* impl.fd = newfd */
	(*env)->SetObjectField(env, this, fdID, newfd); 

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.accept(): returning\n");
#endif
	return;
	error:
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.accept(): throwing\n");
#endif
		throwException(env, EX_IO, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    sockConnect
 * Signature: (Ljava/net/InetAddress;I)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_sockConnect
  (JNIEnv *env, jobject this, jobject host, jint port)
{
	struct sockaddr_storage 	ss;
	struct sockaddr_in			*sin;
	struct sockaddr_in6			*sin6;
	jbyte *addrBytes = NULL;
	jbyteArray addrByteArray;
	int addrlen = 0;
	int len = 0;
	int err;

	int fdnum = get_fdnum(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): entering\n");
#endif
	/* if host is not null then connect to the supplied address */
	if (host != NULL)  {
		jclass hostClass = (*env)->GetObjectClass(env, host);
		jfieldID addrID = (*env)->GetFieldID(env, hostClass, "address", "[B");
		addrByteArray = (jbyteArray)(*env)->GetObjectField(env, host, addrID);
		addrlen = (*env)->GetArrayLength(env, addrByteArray);
		addrBytes = (*env)->GetByteArrayElements(env, addrByteArray, NULL);
	}

	/* were given an IPv4 address */
	if (addrlen == 4) {
		sin = (struct sockaddr_in *)&ss;
		sin->sin_family = AF_INET;
		memcpy(&sin->sin_addr, addrBytes, addrlen);
		sin->sin_port = htons(port);
		len = sizeof(struct sockaddr_in);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): got IPv4 address\n");
#endif
	/* an IPv6 address */
	} else if (addrlen == 16) {
		sin6 = (struct sockaddr_in6 *)&ss;
		sin6->sin6_family = AF_INET6;
		memcpy(&sin6->sin6_addr, addrBytes, addrlen);
		sin6->sin6_port = htons(port);
		len = sizeof(struct sockaddr_in6);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): got IPv6 address\n");
#endif
	} else if (addrlen == 0 && host != NULL) {
		goto error;
	}
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): connecting %d\n", ss.__ss_family);
#endif
	if ((connect(fdnum, (struct sockaddr *)&ss, len)) == 0) {
		jint remoteport;
		jclass thisClass;
		jfieldID addrID, portID;

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): getting this.address and this.port\n");
#endif
		thisClass = (*env)->GetObjectClass(env, this);
		/* get the this.address field */
		addrID = (*env)->GetFieldID(env, thisClass, "address",
											"Ljava/net/InetAddress;");
		/* get the this.localport field */
		portID = (*env)->GetFieldID(env, thisClass, "port", "I");

		/* set this.address to host */
		(*env)->SetObjectField(env, this, addrID, host);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): setting port\n");
#endif
		if (port != 0) {
			/* set this.localport to port */
			remoteport = port;
		} else if ((getsockname(fdnum, (struct sockaddr *)&ss, &len)) == 0) {
			/* set this.localport to ss.sin_port */
			remoteport = ntohs(getSocketPort(&ss));
		} else {
			goto error;
		}
		(*env)->SetIntField(env, this, portID, remoteport);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): returning\n");
#endif
		return;

	}
	error:
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockConnect(): throwing\n");
#endif
		throwException(env, EX_CONNECT, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    sockCreate
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_create
  (JNIEnv *env, jobject this, jboolean stream)
{
	int sock;
	struct addrinfo hints;
	struct addrinfo *res, *ressave;
	int err;
	jfieldID fdID, thisFDID, fdnumID;
	jclass  fdClass, thisClass;
	jobject	fdObject, thisFDObject, gfdObject;
	jmethodID fdMethod;

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): entering\n");
#endif
	/* initialise the hints */
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = stream ? SOCK_STREAM : SOCK_DGRAM;

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): calling getaddrinfo()\n");
#endif
	err = getaddrinfo(NULL, AI_SERV, &hints, &res);
	if (err) {
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): throwing gai_strerror()\n");
#endif
		throwException(env, EX_IO, gai_strerror(err));
		return;
	}

	ressave = res;
	do {
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sock >= 0)
			break; /* use this socket */

	} while ((res = res->ai_next) != NULL);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): throwing sock = %d\n", sock);
#endif
	/* throw IOException if none of the sockets were any good */
	if (res == NULL) {
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): throwing res == NULL\n");
#endif
		throwException(env, EX_IO, "Create Socket Error");
		return;
	}

	/* find the FileDescriptor Class */
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): finding FileDescriptor\n");
#endif
	fdClass = (*env)->FindClass(env, "java/io/FileDescriptor");
	
	/* get the FileDescriptor() constructor */
	fdMethod = (*env)->GetMethodID(env, fdClass, "<init>", "()V");

	/* create an new object */
	fdObject = (*env)->NewObject(env, fdClass, fdMethod);

	/* get the fd field from the class */
/*
	fdID = (*env)->GetFieldID(env, fdClass, "fd", "I");

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): this.fd.fd = sock\n");
#endif
	(*env)->SetIntField(env, fdObject, fdID, sock);
*/	
/*
	gfdObject = (*env)->NewGlobalRef(env, fdObject);
*/

	/* now assign the this.fd FileDescriptor object */
	thisClass = (*env)->GetObjectClass(env, this);

	/* get the fdnum field from the class */
	fdnumID= (*env)->GetFieldID(env, thisClass, "fdnum", "I");
	
	/* get the fd FileDescriptor field */
	thisFDID = (*env)->GetFieldID(env, thisClass, "fd",
									"Ljava/io/FileDescriptor;");

	(*env)->SetObjectField(env, this, thisFDID, fdObject);
		
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): this.fdnum = sock\n");
#endif
	(*env)->SetIntField(env, this, fdnumID, sock);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): freeaddrinfo()\n");
#endif
	freeaddrinfo(ressave);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.create(): returning\n");
#endif
	return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    sockClose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_sockClose
  (JNIEnv *env, jobject this)
{
	int fdnum = get_fdnum(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockClose(): entering\n");
#endif
	/* now close the file descriptor */
	if((close(fdnum)) != 0) {
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockClose(): throwing\n");
#endif
		throwException(env, EX_IO, strerror(errno));
	}
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockClose(): returning\n");
#endif
	return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    sockAvailable
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_java_net_PlainSocketImpl_sockAvailable
  (JNIEnv *env, jobject this)
{
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.sockClose(): entering/returning\n");
#endif
	return 1;
}


void throwException(JNIEnv *env, int type, const char *msg)
{
	jclass exception;
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.throwException(): entering\n");
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
	printf("NATIVE: PlainSocketImpl.throwException(): returning\n");
#endif
	return;
}

int get_fdnum(JNIEnv *env, jobject this)
{
	jfieldID fdnumID;
	jclass	thisClass;
	int fdnum;

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.get_fdnum(): entering\n");
#endif
	/* now assign the this.fd FileDescriptor object */
	thisClass = (*env)->GetObjectClass(env, this);

	/* get the fdnum field from the class */
	fdnumID = (*env)->GetFieldID(env, thisClass, "fdnum", "I");
	
	fdnum = (*env)->GetIntField(env, this, fdnumID);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.get_fdnum(): returning fdnum = %d\n", fdnum);
#endif

	return fdnum;

}

/* get the port number out of the socket address structure */
int getSocketPort(struct sockaddr_storage *ss)
{
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.getSocketPort(): entering\n");
#endif
	switch (ss->__ss_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.getSocketPort(): returning INET\n");
#endif
			return (sin->sin_port);
		}
		case  AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.getSocketPort(): returning INET6\n");
#endif
			return (sin6->sin6_port);
		}
	}
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.getSocketPort(): returning -1\n");
#endif
	return -1;
}