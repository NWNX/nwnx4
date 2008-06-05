CXX=g++
cc=gcc
DEFINES=
INCLUDES=
DEBUG=-g
CXXFLAGS=${DEBUG} ${DEFINES} ${INCLUDES} -Wall -O3 -fno-omit-frame-pointer
CFLAGS=${DEBUG} ${DEFINES} ${INCLUDES} -Wall -O3 -fno-omit-frame-pointer
OBJS=nwn2srvquery.o
BIN=../../../bin/release/nwn2srvquery
LIBS=

all: ${OBJS}
	${CXX} ${CXXFLAGS} -o ${BIN} ${OBJS} ${LIBS}

clean:
	rm -rf ${OBJS} ${BIN}

distclean:
	$(MAKE) clean

*.cpp.${O}:
	${CXX} ${CXXFLAGS} $*.cpp

.c.${O}:
	${CC} ${CFLAGS} $*.c

