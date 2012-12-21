# to build 
#  % make
#
# to clean up all stuff that can be re-built:
#  % make clean
#

CC = gcc
CFLAGS = -Wall -g 

LIBDIR = -L./
INCDIR = -I./


LIBS = $(LIBDIRS) -lpthread

TARGET = hello

.PHONY: clean

all: $(TARGET) 

	
$(TARGET).o: $(TARGET).c $(INCLUDES) 
	$(CC) $(INCDIRS) $(CFLAGS) -c $(TARGET).c 

$(TARGET): $(TARGET).o $(INCLUDES) 
	$(CC) $(INDIRS) $(CFLAGS) -o $(TARGET) $(TARGET).o $(LIBS) 

clean:
	$(RM) *.o core $(TARGET) 

