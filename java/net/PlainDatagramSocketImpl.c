/*
 * $Id$
 *
 * PlainDatagramSocketImpl.c - An IPv6 capable DatagramSocketImpl.
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
 * Revision 1.2  1999/11/08 13:36:45  mpf
 * - Added join(), leave(), setTTL(), getTTL(), setOption() and getOption().
 * - General code clean up.
 *
 *
 */

#include <net6.h>
#include <PlainDatagramSocketImpl.h>

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    bind
 * Signature: (ILjava/net/InetAddress;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_bind
  (JNIEnv *env, jobject this, jint port, jobject addr)
{
	struct sockaddr_storage 	ss;
	struct sockaddr_in			*sin;
	struct sockaddr_in6			*sin6;
	jbyte *addrBytes = NULL;
	jbyteArray addrByteArray;
	socklen_t addrlen = 0;

	int sockfd = getSocketFileDescriptor(env, this);
	int sockfamily = getSocketFamily(sockfd);

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.bind(): entering sockfd = %d sockfamily = %d port = %d\n", sockfd, sockfamily, port);
#endif
	/* if host is not null then bind to the supplied address */
	if (addr != NULL)  {
		jclass addrClass = (*env)->GetObjectClass(env, addr);
		jfieldID addrID = (*env)->GetFieldID(env, addrClass, "address", "[B");
		addrByteArray = (jbyteArray)(*env)->GetObjectField(env, addr, addrID);
		addrlen = (*env)->GetArrayLength(env, addrByteArray);
		addrBytes = (*env)->GetByteArrayElements(env, addrByteArray, NULL);
	}

	memset(&ss, 0, sizeof(ss));
	/* were given an IPv4 address */
	if (addrlen == IPV4_ADDRLEN) {
		if (sockfamily == AF_INET) {
			sin = (struct sockaddr_in *)&ss;
			sin->sin_family = AF_INET;
			memcpy(&sin->sin_addr, addrBytes, addrlen);
			sin->sin_port = htons(port);
		} else if (sockfamily == AF_INET6) {
			sin6 = (struct sockaddr_in6 *)&ss;
			sin6->sin6_family = AF_INET6;
			/* convert IPv4 address to IPv4-mapped IPv6 address */
			CREATE_IPV6_MAPPED(sin6->sin6_addr, addrBytes);
			sin6->sin6_port = htons(port);
		}

	/* an IPv6 address */
	} else if (addrlen == IPV6_ADDRLEN) {
		sin6 = (struct sockaddr_in6 *)&ss;
		sin6->sin6_family = AF_INET6;
		memcpy(&sin6->sin6_addr, addrBytes, addrlen);
		sin6->sin6_port = htons(port);

	} else if (addrlen == 0 && addr != NULL) {
		errno = EINVAL;
		goto error;
	}

	addrlen = SA_LEN((struct sockaddr *)&ss);
	if (bind(sockfd, (struct sockaddr *)&ss, addrlen) == 0) {
		jint lport;
		jclass thisClass = (*env)->GetObjectClass(env, this);

		/* get the this.localport field */
		jfieldID localportID = (*env)->GetFieldID(env, thisClass, "localPort", "I");


		if (port != 0) {
			/* set this.localport to port */
			lport = port;
		} else if ((getsockname(sockfd, (struct sockaddr *)&ss, &addrlen)) == 0) {
			/* set this.localport to ss.sin_port */
			lport = ntohs(getSockAddrPort(&ss));
		} else {
			goto error;
		}
		(*env)->SetIntField(env, this, localportID, lport);
		return;

	}
	error:
		throwException(env, EX_BIND, strerror(errno));
		return;

}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    send
 * Signature: (Ljava/net/DatagramPacket;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_send
  (JNIEnv *env, jobject this, jobject packet)
{
	struct sockaddr_storage 	ss;
	struct sockaddr_in			*sin;
	struct sockaddr_in6			*sin6;
	socklen_t addrlen = 0;
	jint dataLength, port;
	jobject addr;
	jbyte *addrBytes, *dataBytes;
	jbyteArray addrByteArray, dataArray;
	jfieldID addrID;
	jmethodID addrMethod, portMethod, dataMethod;
	jclass packetClass, addrClass;

	int sockfd = getSocketFileDescriptor(env, this);
	int sockfamily = getSocketFamily(sockfd);

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.send(): entering sockfd = %d sockfamily = %d\n", sockfd, sockfamily);
#endif
	packetClass = (*env)->GetObjectClass(env, packet);
	
	/* get the getAddress(), getPort(), and getData() methods and call them */
	addrMethod = (*env)->GetMethodID(env, packetClass, "getAddress", "()Ljava/net/InetAddress;");
	portMethod = (*env)->GetMethodID(env, packetClass, "getPort", "()I");
	dataMethod = (*env)->GetMethodID(env, packetClass, "getData", "()[B");
	
	/* get the DatagramPacket InetAddress */
	addr = (*env)->CallObjectMethod(env, packet, addrMethod);

	/* get the DatagramPacket port */
	port = (*env)->CallIntMethod(env, packet, portMethod);

	/* get the DatagramPacket data to send and data length */
	dataArray = (jbyteArray)(*env)->CallObjectMethod(env, packet, dataMethod);
	dataLength = (*env)->GetArrayLength(env, dataArray);
	dataBytes = (*env)->GetByteArrayElements(env, dataArray, NULL);


	/* now get the InetAddress.address field */
	addrClass = (*env)->GetObjectClass(env, addr);
	addrID = (*env)->GetFieldID(env, addrClass, "address", "[B");
	addrByteArray = (jbyteArray)(*env)->GetObjectField(env, addr, addrID);
	addrlen = (*env)->GetArrayLength(env, addrByteArray);
	addrBytes = (*env)->GetByteArrayElements(env, addrByteArray, NULL);

	memset(&ss, 0, sizeof(ss));
	/* were given an IPv4 address */
	if (addrlen == IPV4_ADDRLEN) {
		if (sockfamily == AF_INET) {
			sin = (struct sockaddr_in *)&ss;
			sin->sin_family = AF_INET;
			memcpy(&sin->sin_addr, addrBytes, addrlen);
			sin->sin_port = htons(port);
		} else if (sockfamily == AF_INET6) {
			sin6 = (struct sockaddr_in6 *)&ss;
			sin6->sin6_family = AF_INET6;
			/* convert IPv4 address to IPv4-mapped IPv6 address */
			CREATE_IPV6_MAPPED(sin6->sin6_addr, addrBytes);
			sin6->sin6_port = htons(port);
		}

	/* an IPv6 address */
	} else if (addrlen == IPV6_ADDRLEN) {
		sin6 = (struct sockaddr_in6 *)&ss;
		sin6->sin6_family = AF_INET6;
		memcpy(&sin6->sin6_addr, addrBytes, addrlen);
		sin6->sin6_port = htons(port);
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.send(): port = %d \n", htons(port));
#endif

	} else if (addrlen == 0 && addr != NULL) {
		errno = EINVAL;
		goto error;
	}

	addrlen = SA_LEN((struct sockaddr *)&ss);

	/* send the data if an error occurs then it falls through to the
     * throw below
	 */
	if (sendto(sockfd, (char *)dataBytes, dataLength, 0, (struct sockaddr *)&ss, addrlen) >= 0)
		return;
	error:
		throwException(env, EX_IO, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    peek
 * Signature: (Ljava/net/InetAddress;)I
 */
JNIEXPORT jint JNICALL Java_java_net_PlainDatagramSocketImpl_peek
  (JNIEnv *env, jobject this, jobject addr)
{
	struct sockaddr_storage ss;
	socklen_t addrlen = sizeof(ss);
	int sockfd;
	ssize_t recvlen;
	jclass inetClass;
	jmethodID inetMethod;
	jbyteArray remoteAddress;
	jbyte *remoteAddressBytes;
	jint remotePort;

	sockfd = getSocketFileDescriptor(env, this);

	recvlen = recvfrom(sockfd, (char *)NULL, 0, MSG_PEEK, (struct sockaddr *)&ss,&addrlen);
	if (recvlen < 0)
		goto error;

	switch (SS_FAMILY(&ss)) {
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
			remotePort = ntohs(sin6->sin6_port);
			remoteAddress = (*env)->NewByteArray(env, IPV6_ADDRLEN);
			remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
			memcpy(remoteAddressBytes, &(sin6->sin6_addr), IPV6_ADDRLEN);
			(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV6_ADDRLEN, remoteAddressBytes);
			/*
			 * if the DatagramPacket was created with an IPv4 address then
			 * we have to map it back to one when we return it.
			 */
			if (IN6_IS_ADDR_V4MAPPED(sin6->sin6_addr.s6_addr32)) {
				remoteAddress = (*env)->NewByteArray(env, IPV4_ADDRLEN);
				remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
				memcpy(remoteAddressBytes, &sin6->sin6_addr.s6_addr32[3], IPV4_ADDRLEN);
				(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV4_ADDRLEN, remoteAddressBytes);
			} else {
				remoteAddress = (*env)->NewByteArray(env, IPV6_ADDRLEN);
				remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
				memcpy(remoteAddressBytes, &sin6->sin6_addr, IPV6_ADDRLEN);
				(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV6_ADDRLEN, remoteAddressBytes);
			}

			break;
		}
		default:
			errno = EAFNOSUPPORT;
			goto error;
	}

	/* pass the address back through the reference var */
	inetClass = (*env)->GetObjectClass(env, addr);
	inetMethod = (*env)->GetMethodID(env, inetClass, "<init>", "(Ljava/lang/String;[B)V");
	addr = (*env)->NewObject(env, inetClass, inetMethod, NULL, remoteAddress);

	/* return the remote port */
	return remotePort;

	error:
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.accept(): throwing\n");
#endif
		throwException(env, EX_IO, strerror(errno));
		return -1;

}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    receive
 * Signature: (Ljava/net/DatagramPacket;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_receive
  (JNIEnv *env, jobject this, jobject packet)
{
	struct sockaddr_storage 	ss;
	ssize_t recvlen;
	socklen_t addrlen = sizeof(ss);
	jint dataLength, remotePort;
	jbyte *dataBytes, *remoteAddressBytes;
	jbyteArray dataArray, remoteAddress;
	jmethodID portMethod, dataMethod, lengthMethod, addrMethod, inetMethod;
	jobject inetObj;
	jclass packetClass, inetClass;

	int sockfd = getSocketFileDescriptor(env, this);

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.receive(): entering\n" );
#endif
	packetClass = (*env)->GetObjectClass(env, packet);
	inetClass = (*env)->FindClass(env, "java/net/InetAddress");
	
	/* get the getAddress(), getPort(), and getData() methods and call them */
	portMethod = (*env)->GetMethodID(env, packetClass, "setPort", "(I)V");
	dataMethod = (*env)->GetMethodID(env, packetClass, "getData", "()[B");
	lengthMethod = (*env)->GetMethodID(env, packetClass, "setLength", "(I)V");
	addrMethod = (*env)->GetMethodID(env, packetClass, "setAddress", "(Ljava/net/InetAddress;)V");
	inetMethod = (*env)->GetMethodID(env, inetClass, "<init>", "(Ljava/lang/String;[B)V");
	
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.receive(): getting dataArray\n" );
#endif
	/* get the DatagramPacket data to send and data length */
	dataArray = (jbyteArray)(*env)->CallObjectMethod(env, packet, dataMethod);
	dataLength = (*env)->GetArrayLength(env, dataArray);
	dataBytes = (*env)->GetByteArrayElements(env, dataArray, NULL);


#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.receive(): calling recvfrom sockfd = %d datalength = %d\n", sockfd, dataLength );
	if (dataBytes == NULL)
		printf("NATIVE: PlainDatagramSocketImpl.receive(): dataBytes == NULL\n");
#endif
	recvlen = recvfrom(sockfd, (char *)dataBytes, dataLength, 0, (struct sockaddr *)&ss, &addrlen);
	if (recvlen < 0)
		goto error;

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.receive(): calling recvfrom\n" );
#endif

	switch (SS_FAMILY(&ss)) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
			remotePort = ntohs(sin->sin_port);
			remoteAddress = (*env)->NewByteArray(env, IPV4_ADDRLEN);
			remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
			memcpy(remoteAddressBytes, &sin->sin_addr, IPV4_ADDRLEN);
			(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV4_ADDRLEN, remoteAddressBytes);
			break;
		}
		case AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
			remotePort = ntohs(sin6->sin6_port);
			/*
			 * if the DatagramPaccket was created with an IPv4 address then
			 * we have to map it back to one when we return it.
			 */
			if (IN6_IS_ADDR_V4MAPPED(sin6->sin6_addr.s6_addr32)) {
				remoteAddress = (*env)->NewByteArray(env, IPV4_ADDRLEN);
				remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
				memcpy(remoteAddressBytes, &sin6->sin6_addr.s6_addr32[3], IPV4_ADDRLEN);
				(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV4_ADDRLEN, remoteAddressBytes);
			} else {
				remoteAddress = (*env)->NewByteArray(env, IPV6_ADDRLEN);
				remoteAddressBytes = (*env)->GetByteArrayElements(env, remoteAddress, NULL);
				memcpy(remoteAddressBytes, &sin6->sin6_addr, IPV6_ADDRLEN);
				(*env)->SetByteArrayRegion(env, remoteAddress, 0, IPV6_ADDRLEN, remoteAddressBytes);
			}
			break;
		}
		default:
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.receive(): family = %d\n", SS_FAMILY(&ss));
#endif
			errno = EAFNOSUPPORT;
			goto error;
	}

	inetObj = (*env)->NewObject(env, inetClass, inetMethod, NULL, remoteAddress);
	(*env)->CallVoidMethod(env, packet, addrMethod, inetObj);
	(*env)->CallVoidMethod(env, packet, portMethod, remotePort);
	(*env)->CallVoidMethod(env, packet, lengthMethod, recvlen);
	return;
	error:
		throwException(env, EX_IO, strerror(errno));
		return;

}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    setTTL
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_setTTL
  (JNIEnv *env, jobject this, jbyte ttl)
{
	int sockfd, sockfamily;

	sockfd = getSocketFileDescriptor(env, this);

	sockfamily = getSocketFamily(sockfd);

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.setTTL() entering ttl = %d\n", ttl);
#endif
	switch (sockfamily)
	{
		case AF_INET:
			if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) < 0)
				goto error;
			return;

		case AF_INET6:
			if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char *)&ttl, sizeof(ttl)) < 0)
				goto error;
			return;

		default:
			errno = EAFNOSUPPORT;
			goto error;
	}

	error:
		throwException(env, EX_IO, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    getTTL
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_java_net_PlainDatagramSocketImpl_getTTL
  (JNIEnv *env, jobject this)
{
	int sockfd, sockfamily;
	jbyte ttl;
	socklen_t ttlLength;

	sockfd = getSocketFileDescriptor(env, this);
	sockfamily = getSocketFamily(sockfd);

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.getTTL() entering sockfd = %d sockfamily = %d\n", sockfd, sockfamily);
#endif
	return 1;
	switch (sockfamily)
	{
		case AF_INET:
			if (getsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, &ttlLength) < 0)
				goto error;
			break;

		case AF_INET6:
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.getTTL() getsockopt()\n");
#endif
			if (getsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char *)&ttl, &ttlLength) < 0)
				goto error;
			break;

		default:
			errno = EAFNOSUPPORT;
			goto error;
	}
			
	return ttl;

	error:
		throwException(env, EX_IO, strerror(errno));
		return (jbyte)NULL;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    join
 * Signature: (Ljava/net/InetAddress;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_join
  (JNIEnv *env, jobject this, jobject addr)
{
	int sockfd, sockfamily;
	jbyteArray addrArray;
	jbyte *addrBytes = NULL;
	jint addrlen = 0;

	sockfd = getSocketFileDescriptor(env, this);
	sockfamily = getSocketFamily(sockfd);

#ifdef DEBUG
	printf("NATIVE: PlainDatagramsocketImpl.join() entering sockfd = %d sockfamily = %d\n", sockfd, sockfamily);
#endif
	if (addr != NULL) {
		jclass addrClass = (*env)->GetObjectClass(env, addr);
		jfieldID addrID = (*env)->GetFieldID(env, addrClass, "address", "[B");
		addrArray = (jbyteArray)(*env)->GetObjectField(env, addr, addrID);
		addrlen = (*env)->GetArrayLength(env, addrArray);
		addrBytes = (*env)->GetByteArrayElements(env, addrArray, NULL);
	}

	switch (addrlen) {
		case IPV4_ADDRLEN: 
			switch (sockfamily) {
				case AF_INET: {
					struct ip_mreq mreq;
					memcpy(&mreq.imr_multiaddr, addrBytes, addrlen);
					mreq.imr_interface.s_addr = htonl(INADDR_ANY);
					if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
									(char *)&mreq, sizeof(mreq)) < 0)
						goto error;
					return;
				}
/* this code has been removed for the moment because multicast IPv4 mapped
 * IPv6 addresses are not supported.
 *
				case AF_INET6: {
					struct ipv6_mreq mreq6;
					struct in6_addr addr6;
#ifdef DEBUG
	printf("NATIVE: PlainDatagramsocketImpl.join() Mapping IPV4 -> IPv6\n");
#endif
					addr6.s6_addr32[0] = htonl(0xff0e0000);
					addr6.s6_addr32[1] = 0;
					addr6.s6_addr32[2] = htonl(0xffff);
					memcpy(&addr6.s6_addr32[3], addrBytes, addrlen);

					memcpy(&mreq6.ipv6mr_multiaddr, &addr6, IPV6_ADDRLEN);
#ifdef DEBUG
{
	char buf[INET6_ADDRSTRLEN];

	printf("NATIVE: PlainDatagramsocketImpl.join() mre6.ipv6_multiaddr = %s\n", inet_ntop(AF_INET6, &mreq6.ipv6mr_multiaddr, buf, sizeof(buf)));
}
#endif
					mreq6.ipv6mr_interface = 0;
					if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
									(char *)&mreq6, sizeof(mreq6)) < 0)
						goto error;
					return;
				}
*/
			}
		case IPV6_ADDRLEN: {
			struct ipv6_mreq mreq6;
#ifdef DEBUG
	printf("NATIVE: PlainDatagramsocketImpl.join() got IPV6 Multicast Addr\n");
#endif
			memcpy(&mreq6.ipv6mr_multiaddr, addrBytes, addrlen);
			mreq6.ipv6mr_interface = 0;
			if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
								(char *)&mreq6, sizeof(mreq6)) < 0)
				goto error;
			return;
		}
	}

	error:
		throwException(env, EX_IO, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    leave
 * Signature: (Ljava/net/InetAddress;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_leave
  (JNIEnv *env, jobject this, jobject addr)
{
	int sockfd, sockfamily;
	jbyteArray addrArray;
	jbyte *addrBytes;
	jint addrlen = 0;

	sockfd = getSocketFileDescriptor(env, this);
	sockfamily = getSocketFamily(sockfd);

	if (addr != NULL) {
		jclass addrClass = (*env)->GetObjectClass(env, addr);
		jfieldID addrID = (*env)->GetFieldID(env, addrClass, "address", "[B");
		addrArray = (jbyteArray)(*env)->GetObjectField(env, addr, addrID);
		addrlen = (*env)->GetArrayLength(env, addrArray);
		addrBytes = (*env)->GetByteArrayElements(env, addrArray, NULL);
	}

	switch (addrlen) {
		case IPV4_ADDRLEN: 
			switch (sockfamily) {
				case AF_INET: {
					struct ip_mreq mreq;
					memcpy(&mreq.imr_multiaddr, addrBytes, addrlen);
					mreq.imr_interface.s_addr = htonl(INADDR_ANY);
					if (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
									&mreq, sizeof(mreq)) < 0)
						goto error;
					return;
				}
				/* removed because IPv6 does not support mapping multicast
				 * IPv4 addresses.
				case AF_INET6: {
					struct ipv6_mreq mreq6;
					struct in6_addr addr6;
					addr6.s6_addr32[0] = 0;
					addr6.s6_addr32[1] = 0;
					addr6.s6_addr32[2] = htonl(0xffff);
					memcpy(&addr6.s6_addr32[3], addrBytes, addrlen);

					memcpy(&mreq6.ipv6mr_multiaddr, &addr6, IPV6_ADDRLEN);
					mreq6.ipv6mr_interface = 0;
					if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP,
									&mreq6, sizeof(mreq6)) < 0)
						goto error;
					return;
				}
				*/
			}
		case IPV6_ADDRLEN: {
			struct ipv6_mreq mreq6;
			memcpy(&mreq6.ipv6mr_multiaddr, &addrBytes, addrlen);
			mreq6.ipv6mr_interface = 0;
			if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP,
								&mreq6, sizeof(mreq6)) < 0)
				goto error;
			return;
		}
	}

	error:
		throwException(env, EX_IO, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    setOption
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_setOption
  (JNIEnv *env, jobject this, jint opt, jobject val)
{
	int sockfd, sockfamily;
	int value = 0;
	jclass inetClass, intClass;

	sockfd = getSocketFileDescriptor(env, this);
	sockfamily = getSocketFamily(sockfd);

	/* check to class of val before we proceed */
	inetClass = (*env)->FindClass(env, "java/net/InetAddress");
	intClass = (*env)->FindClass(env, "java/lang/Integer");

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.setOption() entering\n");
#endif

	if ((*env)->IsInstanceOf(env, val, inetClass)) {
	} else if ((*env)->IsInstanceOf(env, val, intClass)) {
		jmethodID intMethod = (*env)->GetMethodID(env, intClass, "intValue", "()I");
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.setOption() val is Integer\n");
#endif
		value = (*env)->CallIntMethod(env, val, intMethod);
	} else {
		throwException(env, EX_ILLEGAL_ARG, "Invalid Argument");
		return;
	}

	switch (opt)
	{
		case J_SO_TIMEOUT: {
			jclass thisClass = (*env)->GetObjectClass(env, this);
			jfieldID timeoutID = (*env)->GetFieldID(env, thisClass, "timeout", "I");
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.setOption() opt = SO_TIMEOUT\n");
#endif
			if (value < 0) {
				throwException(env, EX_SOCKET, "SO_TIMEOUT value less than zero");
				return;
			}

			(*env)->SetIntField(env, this, timeoutID, value);
			return;

		}
		case J_IP_MULTICAST_IF:
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.setOption() opt = IP_MULTICAST_IF\n");
#endif
			switch (sockfamily) {
				case AF_INET:
					break;
				case AF_INET6:
					break;
				default:
					errno = EAFNOSUPPORT;
					goto error;
			}
		case J_SO_REUSEADDR: {
			int on = 1;

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.setOption() opt = SO_REUSEADDR\n");
#endif
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
					goto error;
			return;
		}
		default:
			errno = ENOPROTOOPT;	
	}
	error:
		throwException(env, EX_IO, strerror(errno));
		return;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    getOption
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_java_net_PlainDatagramSocketImpl_getOption
  (JNIEnv *env, jobject this, jint opt)
{
	struct sockaddr_storage ss;
	int sockfd;
	int ssLength = sizeof(ss);
	jclass intClass;
	jmethodID intMethod;
	

	sockfd = getSocketFileDescriptor(env, this);

	intClass = (*env)->FindClass(env, "java/lang/Integer");
	intMethod = (*env)->GetMethodID(env, intClass, "<init>", "(I)V");

	switch (opt) {
		case J_SO_BINDADDR: {
			jbyteArray addrArray;
			jbyte *addrBytes;
			jclass addrClass;
			jmethodID addrMethod;

			/* get the socket's address structure */
			if (getsockname(sockfd, (struct sockaddr *)&ss, &ssLength) != 0)
				goto error;

			/* find out the address family and copy it's address */
			switch (SS_FAMILY(&ss)) {
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
			return  (*env)->NewObject(env, addrClass, NULL, addrArray);
		}
		case J_IP_MULTICAST_IF:
			/* FIXME: need to implement this */
			return NULL;
		case J_SO_TIMEOUT: {
			jclass thisClass = (*env)->GetObjectClass(env, this);
			jfieldID timeoutID = (*env)->GetFieldID(env, thisClass, "timeout", "I");
			jint timeout = (*env)->GetIntField(env, this, timeoutID);

			return (*env)->NewObject(env, intClass, intMethod, timeout);
		}

	}

	error:
		throwException(env, EX_SOCKET, strerror(errno));
		return NULL;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    datagramSocketCreate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_datagramSocketCreate
  (JNIEnv *env, jobject this)
{
	int sockfd;
	struct addrinfo hints;
	struct addrinfo *res, *reshead;
	int err;
	jclass thisClass, fdClass;
	jobject fdObj;
	jfieldID fdID, fdfdID;

	/* initialise the hints */
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_DGRAM;

#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.socketCreate(): calling getaddrinfo()\n");
#endif
	err = getaddrinfo(HOST_NULL, SERV_ZERO, &hints, &res);
	if (err) {
		throwException(env, EX_IO, gai_strerror(err));
		return;
	}

	reshead = res;
	do {
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.socketCreate(): ai_family = %d, ai_socktype = %d, ai_protocol = %d\n",res->ai_family, res->ai_socktype, res->ai_protocol );
#endif

		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd >= 0) {
#ifdef DEBUG
	printf("NATIVE: PlainDatagramSocketImpl.socketCreate(): sockfd = %d\n", sockfd);
#endif
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
	printf("NATIVE: PlainDatagramSocketImpl.socketCreate(): returning\n");
#endif
	return;
}

/*
 * Class:     java_net_PlainDatagramSocketImpl
 * Method:    datagramSocketClose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_java_net_PlainDatagramSocketImpl_datagramSocketClose
  (JNIEnv *env, jobject this)
{
	int sockfd = getSocketFileDescriptor(env, this);

	if (close(sockfd) != 0) {
		throwException(env, EX_IO, strerror(errno));
	}
	return;
}
