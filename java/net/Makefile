
# This is a comment
#OS=solaris
#OS=linux

JAVAC		= javac
JAVA		= java
JAVAH		= javah

CC			= gcc
INCLUDES	= -I/usr/java/include -I/usr/java/include/linux
CFLAGS		= -g -O2 -Wall $(INCLUDES)
LDFLAGS		= -shared

DESTLIB		= libnet6.so

JAVASRC		= $(wildcard *.java)
CSRC		= $(wildcard *.c)
OBJS		= $(patsubst %.c,%.o,$(wildcard $(CSRC)))
HEADERS		= $(patsubst %.java,%.h,$(wildcard $(JAVASRC)))
CLASSES		= $(patsubst %.java,%.class,$(wildcard $(JAVASRC)))
CLASS		:= $(patsubst %.class,% ,$(wildcard $(CLASSES)))

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
jni: $(CLASSES)
	$(JAVAH) -jni $(patsubst %.class,java.net.%,$<)



test:
	$(JAVA) test.InetAddressTest

clean:
	rm -f $(CLASSES) $(OBJS) $(HEADERS) $(DESTLIB)
