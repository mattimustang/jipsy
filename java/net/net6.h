/*
 * $Id$
 *
 * net6.h - Common definitions for IPv6 java.net implementations.
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
 * Revision 1.3  1999/10/27 14:41:06  mpf
 * - Updated.
 *
 * Revision 1.2  1999/10/20 23:06:41  mpf
 * - Added defines for EX_* exception types.
 * - Cleaned up prototypes.
 *
 * Revision 1.1  1999/10/20 07:39:36  mpf
 * Initital import.
 *
 *
 */

#ifndef __java_net_net6_h__
#define __java_net_net6_h__

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <jni.h>

#define DEBUG		1
#undef DEBUG
#define SERV_ZERO		"0"		/* used as 'serv' argument to getaddrinfo() */
#define SERV_NULL		"*"
#define HOST_NULL		"*"


/* Exception types used by throwException() */
#define EX_IO			0
#define EX_BIND			1
#define EX_CONNECT		2
#define EX_SOCKET		3


/* function prototypes */

/* utility functions */
void throwException(JNIEnv *, int, const char *);
int getSocketFileDescriptor(JNIEnv *, jobject);
int getSockAddrPort(struct sockaddr_storage *);


#endif
