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
 * Revision 1.4  1999/11/03 22:27:56  mpf
 * - Added getOption and setOption native functions.
 *
 * Revision 1.3  1999/11/01 16:58:43  mpf
 * - Squashed many bugs in bind() and accept(). Everything working
 * - Fixed problem with socketConnect() so we can now connect to an IPv4
 *   address from an IPv6 socket.
 * - Updated includes.
 *
 * Revision 1.2  1999/10/27 14:34:52  mpf
 * - Changed socketCreate() now creates the socket and puts it in this.fd.fd.
 * - socketConnect() is now working for IPv6 end points. I need to warp the socket
 *   to an IPv4 socket OR map the IPv4 address to an IPv4 mapped IPv6 address if
 *   the socket is an IPv6 socket.
 * - Moved all of the helper functions to util.c
 * - Changed available(), now using select().
 *
 * Revision 1.1  1999/10/20 23:08:16  mpf
 * Initial Import.
 * - Can almost connect using IPv6 (connects but socket closes straight way).
 * - Still some problems with create, and connect, other functions nots tested yet.
 *
 *
 */

#include <net6.h>
#include <PlainSocketImpl.h>


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
	int len = sizeof(ss);

	int sockfd = getSocketFileDescriptor(env, this);
	int sockfamily = getSocketFamily(sockfd);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind(): entering sockfamily = %d\n", sockfamily);
#endif
	/* if host is not null then bind to the supplied address */
	if (host != NULL)  {
		jclass hostClass = (*env)->GetObjectClass(env, host);
		jfieldID addrID = (*env)->GetFieldID(env, hostClass, "address", "[B");
		addrByteArray = (jbyteArray)(*env)->GetObjectField(env, host, addrID);
		addrlen = (*env)->GetArrayLength(env, addrByteArray);
		addrBytes = (*env)->GetByteArrayElements(env, addrByteArray, NULL);
	}

	memset(&ss, 0, sizeof(ss));
	/* were given an IPv4 address */
	if (addrlen == IPV4_ADDRLEN) {
		sin = (struct sockaddr_in *)&ss;
		sin->sin_family = AF_INET;
		memcpy(&sin->sin_addr, addrBytes, addrlen);
		sin->sin_port = htons(port);

	/* an IPv6 address */
	} else if (addrlen == IPV6_ADDRLEN) {
		sin6 = (struct sockaddr_in6 *)&ss;
		sin6->sin6_family = AF_INET6;
		memcpy(&sin6->sin6_addr, addrBytes, addrlen);
		sin6->sin6_port = htons(port);

	} else if (addrlen == 0 && host != NULL) {
		errno = EINVAL;
		goto error;
	}

	len = SA_LEN((struct sockaddr *)&ss);
	if (bind(sockfd, (struct sockaddr *)&ss, len) == 0) {
		jint lport;
		jclass thisClass = (*env)->GetObjectClass(env, this);

		/* get the this.address field */
		jfieldID addrID = (*env)->GetFieldID(env, thisClass, "address",
											"Ljava/net/InetAddress;");
		/* get the this.localport field */
		jfieldID localportID = (*env)->GetFieldID(env, thisClass, "localport", "I");

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind() succeeded\n");
#endif
		/* set this.address to host */
		(*env)->SetObjectField(env, this, addrID, host);

		if (port != 0) {
			/* set this.localport to port */
			lport = port;
		} else if ((getsockname(sockfd, (struct sockaddr *)&ss, &len)) == 0) {
			/* set this.localport to ss.sin_port */
			lport = ntohs(getSockAddrPort(&ss));
		} else {
			goto error;
		}
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind(): localport = %d\n", lport);
#endif
		(*env)->SetIntField(env, this, localportID, lport);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind(): returning\n");
#endif
		return;

	}
	error:
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.bind(): throwing errno = %d\n", errno);
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
	int sockfd = getSocketFileDescriptor(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.listen(): entering\n");
#endif
	if (listen(sockfd, backlog) != 0) {
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
	int newsockfd;
	jbyteArray remoteAddress;
	jbyte *remoteAddressBytes;

	jclass implClass, thisClass, inetClass, fdClass;
	jfieldID localportID, portID, addressID, fdID, fdfdID;
	jfieldID thisLocalportID;
	jint remotePort, thisLocalport;
	jobject newInetAddress, newfd;
	jmethodID inetMethod, fdMethod;
	
	int sockfd = getSocketFileDescriptor(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.accept(): entering\n");
#endif
	newsockfd = accept(sockfd, (struct sockaddr *)&ss, &addrlen);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.accept(): newsockfd = %d\n", newsockfd);
#endif

	if (newsockfd < 0)
		goto error;

	switch (ss.__ss_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
			remoteAddress = (*env)->NewByteArray(env, IPV4_ADDRLEN);
			remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
			memcpy(remoteAddressBytes, &sin->sin_addr, IPV4_ADDRLEN);
			(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV4_ADDRLEN, remoteAddressBytes);
			remotePort = ntohs(sin->sin_port);
			break;
		}
		case AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
			remoteAddress = (*env)->NewByteArray(env, IPV6_ADDRLEN);
			remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
			memcpy(remoteAddressBytes, &(sin6->sin6_addr), IPV6_ADDRLEN);
			(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV6_ADDRLEN, remoteAddressBytes);
			remotePort = ntohs(sin6->sin6_port);
			break;
		}
		default:
			errno = EAFNOSUPPORT;
			goto error;
	}

	/* get all the field IDs for the impl object */
	implClass = (*env)->GetObjectClass(env, impl);
	localportID = (*env)->GetFieldID(env, implClass, "localport", "I");
	portID = (*env)->GetFieldID(env, implClass, "port", "I");
	addressID = (*env)->GetFieldID(env, implClass, "address", "Ljava/net/InetAddress;");
	fdID = (*env)->GetFieldID(env, implClass, "fd", "Ljava/io/FileDescriptor;");

	/* impl.sockfd = sock; */
	/*(*env)->SetIntField(env, impl, sockfdID, sock);*/
	
	/* impl.localport = this.localport; */
	thisClass = (*env)->GetObjectClass(env, this);
	thisLocalportID = (*env)->GetFieldID(env, thisClass, "localport", "I");
	thisLocalport = (*env)->GetIntField(env, this, thisLocalportID);
	(*env)->SetIntField(env, impl, localportID, thisLocalport);

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
	(*env)->SetIntField(env, newfd, fdfdID, newsockfd);

	/* impl.fd = newfd */
	(*env)->SetObjectField(env, impl, fdID, newfd); 
#ifdef DEBUG
{
	int fd = getSocketFileDescriptor(env, impl);
	printf("NATIVE: PlainSocketImpl.accept(): impl.fd.fd == %d\n",fd);
}
#endif

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
 * Method:    socketConnect
 * Signature: (Ljava/net/InetAddress;I)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_socketConnect
  (JNIEnv *env, jobject this, jobject host, jint port)
{
	struct sockaddr_storage 	ss;
	struct sockaddr_in			*sin;
	struct sockaddr_in6			*sin6;
	jbyte *addrBytes = NULL;
	jbyteArray addrByteArray;
	int addrlen = 0;
	socklen_t len = 0;
	int sockfamily;

	int sockfd = getSocketFileDescriptor(env, this);
	sockfamily = getSocketFamily(sockfd);

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketConnect(): entering\n");
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
	if (addrlen == IPV4_ADDRLEN) {
		if (sockfamily == AF_INET) {
			sin = (struct sockaddr_in *)&ss;
			sin->sin_family = AF_INET;
			memcpy(&sin->sin_addr, addrBytes, addrlen);
			sin->sin_port = htons(port);
			len = sizeof(struct sockaddr_in);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketConnect(): got IPv4 && AF_INET\n");
#endif
		} else if (sockfamily == AF_INET6) {
			
			struct in6_addr *in6;
			sin6 = (struct sockaddr_in6 *)&ss;
			in6 = &sin6->sin6_addr;
			sin6->sin6_family = AF_INET6;
			/* convert IPv4 address to IPv4-mapped IPv6 address */
			in6->s6_addr32[0] = 0;
			in6->s6_addr32[1] = 0;
			in6->s6_addr32[2] = htonl(0xffff);
			memcpy(&in6->s6_addr32[3], addrBytes, addrlen);
			sin6->sin6_port = htons(port);
			len = sizeof(struct sockaddr_in6);
		}

	/* an IPv6 address */
	} else if (addrlen == IPV6_ADDRLEN) {
		if (sockfamily == AF_INET6) {
			sin6 = (struct sockaddr_in6 *)&ss;
			sin6->sin6_family = AF_INET6;
			memcpy(&sin6->sin6_addr, addrBytes, addrlen);
			sin6->sin6_port = htons(port);
			len = sizeof(struct sockaddr_in6);
		}

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketConnect(): got IPv6 address\n");
#endif
	} else if (addrlen == 0 && host != NULL) {
		errno = EINVAL;
		goto error;
	}

	if ((connect(sockfd, (struct sockaddr *)&ss, len)) == 0) {
		jint remoteport;
		jclass thisClass;
		jfieldID addrID, portID;

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketConnect(): getting this.address and this.port\n");
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
	printf("NATIVE: PlainSocketImpl.socketConnect(): setting port\n");
#endif
		if (port != 0) {
			/* set this.localport to port */
			remoteport = port;
		} else if ((getsockname(sockfd, (struct sockaddr *)&ss, &len)) == 0) {
			/* set this.localport to ss.sin_port */
			remoteport = ntohs(getSockAddrPort(&ss));
		} else {
			goto error;
		}
		(*env)->SetIntField(env, this, portID, remoteport);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketConnect(): returning\n");
#endif
		return;

	}
	error:
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketConnect(): throwing\n");
#endif
		throwException(env, EX_CONNECT, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    socketCreate
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_socketCreate
  (JNIEnv *env, jobject this, jboolean stream)
{
	int sockfd;
	struct addrinfo hints;
	struct addrinfo *res, *reshead;
	int err;
	jclass  thisClass, fdClass;
	jobject	fdObj;
	jfieldID fdID, fdfdID;

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketCreate(): entering\n");
#endif
	/* initialise the hints */
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = stream ? SOCK_STREAM : SOCK_DGRAM;

#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketCreate(): calling getaddrinfo()\n");
#endif
	err = getaddrinfo(HOST_NULL, SERV_ZERO, &hints, &res);
	if (err) {
		throwException(env, EX_IO, gai_strerror(err));
		return;
	}

	reshead = res;
	do {
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketCreate(): ai_family = %d, ai_socktype = %d, ai_protocol = %d\n",res->ai_family, res->ai_socktype, res->ai_protocol );
#endif

		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd >= 0) {
			break; /* use this socket */
		}

	} while ((res = res->ai_next) != NULL);

	/* throw IOException if none of the sockets were any good */
	if (res == NULL) {
		throwException(env, EX_IO, "Cannot create a valid socket");
		return;
	}

	thisClass = (*env)->GetObjectClass(env, this);


	/* get the fd FileDescriptor field */
	fdID = (*env)->GetFieldID(env, thisClass, "fd", "Ljava/io/FileDescriptor;");
	fdObj = (*env)->GetObjectField(env, this, fdID);
	fdClass = (*env)->GetObjectClass(env, fdObj);

	fdfdID = (*env)->GetFieldID(env, fdClass, "fd", "I");

	/* now set it to the newly created socket file descriptor */
	(*env)->SetIntField(env, fdObj, fdfdID, sockfd);
	

	freeaddrinfo(reshead);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.socketCreate(): returning\n");
#endif
	return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    socketClose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_socketClose
  (JNIEnv *env, jobject this)
{
	int sockfd = getSocketFileDescriptor(env, this);

	/* now close the file descriptor */
	if((close(sockfd)) != 0) {
		throwException(env, EX_IO, strerror(errno));
	}
	return;
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    available
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_java_net_PlainSocketImpl_available
  (JNIEnv *env, jobject this)
{
	int nbits;
	int sockfd;
	struct timeval tv;
	fd_set rset;
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.available(): entering\n");
#endif
	sockfd = getSocketFileDescriptor(env, this);

	if (sockfd < 0) {
		errno = EBADF;
		nbits = 0;
	} else {
		FD_ZERO(&rset);
		FD_SET(sockfd, &rset);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		nbits = select (sockfd + 1, &rset, NULL, NULL, &tv);
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.available(): returning %d\n", nbits);
#endif
	}
	return (nbits == 0 ? 0 : 1);
}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    getOption
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_java_net_PlainSocketImpl_getOption
  (JNIEnv *env, jobject this, jint opt)
{
	int sockfd;
	struct sockaddr_storage ss;
	int ssLength = sizeof(ss);
	int val;
	int valLength = sizeof(val);
	struct linger lingerVal;
	int lingerLength;

	jclass boolClass, intClass;
	jmethodID boolMethod, intMethod;

	sockfd = getSocketFileDescriptor(env, this);

	/*
	 * get the Boolean and Integer classes and their constructors for 
	 * use later
	 */
	boolClass = (*env)->FindClass(env, "java/lang/Boolean");
	boolMethod = (*env)->GetMethodID(env, boolClass, "<init>", "(Z)V");
	intClass = (*env)->FindClass(env, "java/lang/Integer");
	intMethod = (*env)->GetMethodID(env, boolClass, "<init>", "(I)V");

	switch (opt) {
		case J_TCP_NODELAY:

			if (getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&val, &valLength) != 0)
				goto error;

			/* create the Boolean instance to return */
			return (*env)->NewObject(env, boolClass, boolMethod, (val != 0));
		case J_SO_LINGER:
			if (getsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char *)&lingerVal,
							&lingerLength) != 0)
				goto error;
			if (lingerVal.l_onoff) {
				/* create the new Integer instance to return */
				return (*env)->NewObject(env, intClass, intMethod, lingerVal.l_linger);
			} else {
				/* create the Boolean instance to return */
				return (*env)->NewObject(env, boolClass, boolMethod, JNI_FALSE);
			}

		case J_SO_BINDADDR: {
			jbyteArray addrArray;
			jbyte *addrBytes;
			jobject addrObj;
			jclass addrClass;
			jmethodID addrMethod;

			/* get the socket's address structure */
			if (getsockname(sockfd, (struct sockaddr *)&ss, &ssLength) != 0)
				goto error;

			/* find out the address family and copy it's address */
			switch (ss.__ss_family) {
				case AF_INET: {
					struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
					addrArray = (*env)->NewByteArray(env, IPV4_ADDRLEN);
					addrBytes = (*env)->GetByteArrayElements(env, addrArray, NULL);
					memcpy(addrBytes, &sin->sin_addr, IPV4_ADDRLEN);
					(*env)->SetByteArrayRegion(env, addrArray, 0, IPV4_ADDRLEN, addrBytes);
					break;
				}
				case AF_INET6: {
					struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
					addrArray = (*env)->NewByteArray(env, IPV6_ADDRLEN);
					addrBytes = (*env)->GetByteArrayElements(env, addrArray, NULL);
					memcpy(addrBytes, &sin6->sin6_addr, IPV6_ADDRLEN);
					(*env)->SetByteArrayRegion(env, addrArray, 0, IPV6_ADDRLEN, addrBytes);
					break;
				}
				default:
					errno = EAFNOSUPPORT;
					goto error;
			}
			/* now create a new InetAddress and return it */
			addrClass = (*env)->FindClass(env, "java/net/InetAddress");
			addrMethod = (*env)->GetMethodID(env, addrClass, "<init>", "(Ljava/lang/String;[B)V");
			addrObj = (*env)->NewObject(env, addrClass, NULL, addrArray);
			return addrObj;	
		}
		case J_SO_TIMEOUT: {
			jclass thisClass = (*env)->GetObjectClass(env, this);
			jfieldID timeoutID = (*env)->GetFieldID(env, thisClass, "timeout", "I");
			jint timeout = (*env)->GetIntField(env, this, timeoutID);

			return (*env)->NewObject(env, intClass, intMethod, timeout);
		}
		default:
			errno = ENOPROTOOPT;
	}
	error:
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.getOption(): throwing\n");
#endif
	throwException(env, EX_SOCKET, strerror(errno));
	return NULL;

}

/*
 * Class:     java_net_PlainSocketImpl
 * Method:    setOption
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainSocketImpl_setOption
  (JNIEnv *env, jobject this, jint opt, jobject val)
{
	int value, sockfd;
	int valueLength = sizeof(value);
	jclass boolClass;


	sockfd = getSocketFileDescriptor(env, this);

	boolClass = (*env)->FindClass(env, "java/lang/Boolean");

	if ((*env)->IsInstanceOf(env, val, boolClass)) {
		jmethodID boolMethod = (*env)->GetMethodID(env, boolClass, "booleanValue", "()Z");
		jboolean enable = (*env)->CallBooleanMethod(env, val, boolMethod);
		if (enable)
			value = 1;
		else
			value = 0;
	} else {
		/* FIXME: do stricter type checking */
		/* assume that val is an Integer */
		jclass intClass = (*env)->GetObjectClass(env, val);
		jmethodID intMethod = (*env)->GetMethodID(env, intClass, "intValue", "()I");
		value = (*env)->CallIntMethod(env, val, intMethod);
	}

	
	switch (opt) {
		case J_TCP_NODELAY:
			if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &value, valueLength) != 0)
			goto error;
			return;
		case J_SO_LINGER: {
			struct linger l;
			l.l_onoff = value;
			l.l_linger = (value) ? -1 : value;
			if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char *)&l, sizeof(l)) != 0)
				goto error;
			return;
		}
		case J_SO_BINDADDR:
			throwException(env, EX_SOCKET, "SO_BINDADDR is read-only option");
			return;
		case J_SO_TIMEOUT: {
			jclass thisClass = (*env)->GetObjectClass(env, this);
			jfieldID timeoutID = (*env)->GetFieldID(env, thisClass, "timeout", "I");
			if (value < 0) {
				throwException(env, EX_SOCKET, "SO_TIMEOUT value less than zero");
				return;
			}

			(*env)->SetIntField(env, this, timeoutID, value);
			return;
		}
		default:
			errno = ENOPROTOOPT;
	}
	error:
#ifdef DEBUG
	printf("NATIVE: PlainSocketImpl.setOption(): throwing\n");
#endif
	throwException(env, EX_SOCKET, strerror(errno));
	return;


}
