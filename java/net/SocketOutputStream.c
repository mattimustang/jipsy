/*
 * $Id$
 *
 * SocketOutputStream.c - Jave native socketWrite function.
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
 * Revision 1.4  2000/08/28 06:06:41  mpf
 * - Added init() native method stubs.
 *
 * Revision 1.3  1999/11/07 03:29:33  mpf
 * - Updated after rename of file(s).
 *
 * Revision 1.2  1999/11/01 17:09:50  mpf
 * - Updated includes.
 *
 * Revision 1.1  1999/10/27 14:50:05  mpf
 * Inital import.
 *
 *
 */

#include <net6.h>
#include <SocketOutputStream.h>

/*
 * Class:     java_net_SocketOutputStream
 * Method:    socketWrite
 * Signature: (I[BII)V
 */
JNIEXPORT void JNICALL Java_java_net_SocketOutputStream_socketWrite
  (JNIEnv *env, jobject this, jbyteArray byteArray, jint offset, jint length)

{
	jclass thisClass;
	jobject implObj;
	jfieldID implID;
	jint sockfd;
	jbyte bytes[length];

#ifdef DEBUG
	printf("NATIVE: SocketOutputStream.socketWrite() entering\n");
#endif
	thisClass = (*env)->GetObjectClass(env, this);

	implID = (*env)->GetFieldID(env, thisClass, "impl", "Ljava/net/SocketImpl;");
	implObj = (*env)->GetObjectField(env, this, implID);

	sockfd = getSocketFileDescriptor(env, implObj);

	(*env)->GetByteArrayRegion(env, byteArray, offset, length, bytes);

	if (write(sockfd, bytes, length) != length)
		throwException(env, EX_IO, strerror(errno));
	return;
}
JNIEXPORT void JNICALL Java_java_net_SocketOutputStream_init
  (JNIEnv *env, jobject this)
{
	return;
}
