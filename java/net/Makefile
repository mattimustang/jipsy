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

SRCDIR=../..

JAVAC		= javac
JAVA		= java
JAVAH		= javah
JAVADIR		= /usr/java


CC			= gcc
INCLUDES	= -I$(SRCDIR)/include -I$(JAVADIR)/include \
				-I$(JAVADIR)/include/$(JNI_INCLUDE)
DEBUG		=
#DEBUG		=
#DEBUG		= 
CFLAGS		= -g -O2 -Wall $(DEBUG) $(INCLUDES)
LDFLAGS		= -shared

DESTLIB		= ../../lib/$(OS)/$(ARCH)/libnet6.so

JAVASRC		= $(wildcard *.java)
CSRC		= $(wildcard *.c)
OBJS		= $(patsubst %.c,%.o,$(wildcard $(CSRC)))
HEADERS		= $(patsubst %.c,%.h,$(wildcard $(CSRC))) net6.h
CLASSES		= $(patsubst %.java,%.class,$(wildcard $(JAVASRC)))
CLASS		= $(patsubst %.java,% ,$(wildcard $(JAVASRC)))

%.class: %.java
	$(JAVAC) $<

#%.o: %.c
#	$(CC) $(CFLAGS) $<

all: classes lib

lib: $(OBJS)
	$(CC) $(LDFLAGS) -o $(DESTLIB) $(OBJS)

classes: $(CLASSES)

$(CLASS): $(JAVASRC)

.PHONY: $(CLASS)
jni: $(CLASS)
	$(JAVAH) -jni -o $(SRCDIR)/include/$<.h java.net.$<



test:
	$(JAVA) test.InetAddressTest

clean:
	rm -f $(CLASSES) $(OBJS) $(DESTLIB)
