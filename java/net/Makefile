#
# $Id$
#
# This is a comment
#OS=solaris
OS=linux

SRCDIR=/home/mpf/project

JAVAC		= javac
JAVA		= java
JAVAH		= javah
JAVADIR		= /usr/java


CC			= gcc
INCLUDES	= -I$(SRCDIR)/include -I$(JAVADIR)/include \
				-I$(JAVADIR)/include/$(OS)
CFLAGS		= -g -O2 -Wall $(INCLUDES)
LDFLAGS		= -shared

DESTLIB		= libnet6.so

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

all: classes jni lib

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
