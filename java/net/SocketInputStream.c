/*
 * $Id$
 *
 * socketRead.c - Java native socketRead function.
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
 * Revision 1.2  1999/11/01 17:09:28  mpf
 * - Updated includes.
 *
 * Revision 1.1  1999/10/27 14:50:05  mpf
 * Inital import.
 *
 *
 */

#include <net6.h>
#include <socketRead.h>

/*
 * Class:     java_net_SocketInputStream
 * Method:    socketRead
 * Signature: ([BII)I
 */
JNIEXPORT jint JNICALL Java_java_net_SocketInputStream_socketRead
  (JNIEnv *env, jobject this, jbyteArray byteArray, jint offset, jint length)

{
	jclass thisClass;
	jobject implObj;
	jfieldID implID;
	jint sockfd;
	jbyte bytes[length];
	int n;

#ifdef DEBUG
	printf("NATIVE: SocketOutputStream.socketRead() entering\n");
#endif

	/* get this class */
	thisClass = (*env)->GetObjectClass(env, this);

	/* get the this.impl field */
	implID = (*env)->GetFieldID(env, thisClass, "impl", "Ljava/net/SocketImpl;");
	implObj = (*env)->GetObjectField(env, this, implID);

	sockfd = getSocketFileDescriptor(env, implObj);

	/* now read up to length bytes from the socket */
	if ((n = read(sockfd, bytes, length)) < 0) {
		throwException(env, EX_IO, strerror(errno));
#ifdef DEBUG
	printf("NATIVE: SocketOutputStream.socketRead() throwing\n");
#endif
		return n;
	}

	/* set the bytes in the jbyteArray we were passed and return 
	 * the number of bytes read
	 */
	(*env)->SetByteArrayRegion(env, byteArray, offset, n, bytes);
#ifdef DEBUG
	printf("NATIVE: SocketOutputStream.socketRead() returning\n");
#endif
	return n;
}
