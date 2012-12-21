#
# Makefile for gdb examples 
#


# CC for regular build and for valgrind build
CC = gcc

#
# turn on all warnings and the debug flag
#
CFLAGS= -Wall -g

LFLAGS=
LIBS=
INCLUDES=

SRCS = badprog.c infiniteloop.c functions.c  loops.c

OBJS = $(SRCS:.c=.o)

ALL: badprog infiniteloop functions loops segfaulter attach_example

badprog:  badprog.o
	$(CC) $(CFLAGS) $(INCLUDES) badprog.o -o badprog $(LFLAGS) $(LIBS)

infiniteloop:  infiniteloop.o
	$(CC) $(CFLAGS) $(INCLUDES) infiniteloop.o -o infiniteloop $(LFLAGS) $(LIBS)

loops:  loops.o
	$(CC) $(CFLAGS) $(INCLUDES) loops.o -o loops $(LFLAGS) $(LIBS)

functions:  functions.o
	$(CC) $(CFLAGS) $(INCLUDES) functions.o -o functions $(LFLAGS) $(LIBS)

segfaulter:  segfaulter.o
	$(CC) $(CFLAGS) $(INCLUDES) segfaulter.o -o segfaulter $(LFLAGS) $(LIBS)

attach_example:  attach_example.o
	$(CC) $(CFLAGS) $(INCLUDES) attach_example.o -o attach_example $(LFLAGS) $(LIBS)

.C.o:
	$(CC) $(CLFAGS) $(INCLUDEES) -c $<

depend: $(SRCS)
	makedepend $(INCLUDES) $^

clean:
	 rm -f *.o *~ badprog infiniteloop functions segfaulter loops \
		 attach_example

# DO NOT DELETE THIS LINE -- make depend needs it
