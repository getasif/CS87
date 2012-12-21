#
# a generic makefile for a building a single executable 
# from one or more C source files
#
# it uses makedepend, so run 'make depend' once first (and
# again anytime you change #includes). it will spit out 
# some errors about not being able to find some standard .h
# files, which you can ignore. If it cannot find your .h files, 
# however, you need to fix the makefile variable definitions.
#
# 'make depend' auto generate .o dependencies to end of makefile
#
# 'make'  builds the myprog executable
# 'make clean' removes all files that can be rebuilt
#
####################################################################

# makefile variables, some or all may need to be changed 
# to build different executables

# define the compiler to use
CC = gcc

# define compiler flags
# -g    adds debugging information to the executable file
# -Wall turns on most, but not all, compiler warnings
CFLAGS = -g -Wall  

# define any libraries that need to be linked with executable
# -L specifies a library directory (/usr/lib is the default)
#    ex.  LFLAGS = -L/home/newhall/lib  
LFLAGS =
# -l specifies the name of a libray to link in
#    ex.  LIBS = -lm  -lreadline        
LIBS = 

# add to the include path all directories containing .h files 
# needed for build  (the default is in /usr/include) 
INCLUDEPATH = -I./ 

# list my program specific header files (not standard ones included)
INCLS = counter.h 

# define the .c source files
SRCS = counter.c main.c

# define the C object files automatically from $SRCS
# This uses Suffix Replacement within a macro:
#    $(name:string1=string2)
#          For each word in 'name' replace 'string1' with 'string2'
#  Below we are replacing the suffix .c of all words in the macro SRCS
#  with the .o suffix
OBJS = $(SRCS:.c=.o)

# define the name of executable file
TARGET = myprog


# the rest of this is generic build rules and does not need to be 
# changed for different projects 

.PHONY: depend clean

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(INCLUDES) -o $(TARGET)  $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o: 
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(INCLUDES) -c $<  -o $@

clean: 
	$(RM) *.o *~ $(TARGET)

depend: $(SRCS)
	        makedepend $(INCLUDEPATH) $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

