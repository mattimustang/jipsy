#
# $Id$
#
# This is a comment

#  Defines for LINUX/ppc
OS			= linux
JNI_INCLUDE	= linux
ARCH		= ppc

# some definitions for Linux/i386 Uncomment to compile for this platform
#JNI_INCLUDE	= genunix
#ARCH		= intel

#Uncomment the follwing lines to build on Solaris/sparc
#OS			= solaris
#JNI_INCLUDE	= solaris
#ARCH		= sparc

SRCDIR=.

JAVADIR		= /usr/java
JAVAC		= $(JAVADIR)/bin/javac
JAVA		= $(JAVADIR)/bin/java
JAVAH		= $(JAVADIR)/bin/javah
JAR			= $(JAVADIR)/bin/jar

CC			= gcc
INCLUDES	= -I$(SRCDIR)/include -I$(JAVADIR)/include \
				-I$(JAVADIR)/include/$(JNI_INCLUDE)
DEBUG		=
DEBUG		= -DDEBUG
CFLAGS		= -g -O2 -Wall -D_REENTRANT -D_GNU_SOURCE $(DEBUG) $(INCLUDES)
LDFLAGS		= -shared

DESTLIB		= lib/libnet6.so
DESTJAR		= lib/jipsy.jar

# The bootstrap classes
BOOTCLASSPATH = $(DESTJAR):$(JAVADIR)/jre/lib/rt.jar

# The java source code
JAVA_SRC		= java/net/InetAddress.java \
                  java/net/PlainDatagramSocketImpl.java \
                  java/net/PlainSocketImpl.java

# The java classes
JAVA_CLASSES= $(JAVA_SRC:%.java=%.class)

CSRC		= java/net/InetAddress.c java/net/PlainDatagramSocketImpl.c \
              java/net/PlainSocketImpl.c java/net/SocketInputStream.c \
              java/net/SocketOutputStream.c java/net/util.c

OBJS		= $(CSRC:%.c=%.o)

JNI_HEADERS	= include/InetAddress.h include/PlainDatagramSocketImpl.h \
              include/PlainSocketImpl.h include/SocketInputStream.h \
              include/SocketOutputStream.h

HEADERS		= $(JNI_HEADERS) include/net6.h

#
#
#

all: classes jar jni lib

%.class: %.java
	$(JAVAC) $<

$(OBJS): $(CSRC) $(HEADERS)

$(JAVA_CLASSES): $(JAVA_SRC)

$(DESTLIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $?

classes: $(JAVA_CLASSES)

lib: $(DESTLIB)

jni: $(JAVA_CLASSES)
	bin/makejniheaders

jar: $(JAVA_CLASSES)
	test -d lib || mkdir lib
	$(JAR) -0cvf lib/jipsy.jar java/net/*.class

test:
	$(JAVA) test.InetAddressTest

clean:
	rm -f $(JAVA_CLASSES) $(OBJS) $(DESTLIB) $(DESTJAR)
