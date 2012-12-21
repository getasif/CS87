CC = nvcc
TARGET=simple
CFLAGS = -g -G
AR = /usr/bin/ar -rvs 



all: ${TARGET}

${TARGET}: ${TARGET}.cu libmyopengllib.a 
	${CC}  ${CFLAGS}  -o ${TARGET} ${TARGET}.cu -L. -lmyopengllib  -lglut -lGL -lGLU -lGLEW

myopengllib.o: myopengllib.cu 
	${CC} ${CFLAGS} -c myopengllib.cu -lglut -lGL -lGLU -lGLEW 


libmyopengllib.a: myopengllib.o
	${AR} libmyopengllib.a  myopengllib.o 

clean:
	${RM} ${TARGET} myopengllib.o libmyopengllib.a 
