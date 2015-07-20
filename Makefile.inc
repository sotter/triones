##
# TRIONES_HOME
#
# MODULE
# EXTRA_CFLAGS
# EXTRA_INC_PATH
# EXTRA_LIB_PATH
# EXTRA_LIBS
# SRCS
# INCS
# BIN
# SLIB
# DLIB

AR       = ar
ARFLAGS  = -ruv
RANLIB   = ranlib
CXX      = g++
CC       = gcc

CPPFLAGS = -D_USE_MACRO -D_CSERVER_DEBUG_ -D_USE_SECOND -D_USE_SINGLE -D_GPS_STAT ${PRJ_VER_DEF}
CPPFLAGS+= ${EXTRA_CPPFLAGS}
CFLAGS   = -Wall -Wextra -Werror -fPIC 
CFLAGS  += ${EXTRA_CFLAGS}
CXXFLAGS = ${CFLAGS} 
DLFLAGS  = -shared -Wl,-soname,${DLIB}

PREFIX  ?= ${TRIONES_HOME}

INC_PATH = ${EXTRA_INC_PATH}
LIB_PATH = ${EXTRA_LIB_PATH}
LIBS     = ${EXTRA_LIBS}

INCS ?= $(wildcard *.h)
SRCS ?= $(wildcard *.cpp) $(wildcard *.c)

OBJS  = $(addsuffix .o, $(basename ${SRCS}))
OBJS += $(addprefix var/, $(addsuffix .o, $(basename $(notdir ${EXTRA_SRCS}))))

all: ${BIN} ${SLIB} ${DLIB}

clean: 
	rm -rf ./var ${OBJS} ${BIN} ${SLIB} ${DLIB}

install:
	@if [ ! -d "${PREFIX}/include/triones/${MODULE}" ]; then mkdir -p ${PREFIX}/include/triones/${MODULE}; fi
	@if [ ! -d "${PREFIX}/lib" ];     then mkdir -p ${PREFIX}/lib; fi
	@if [ ! -d "${PREFIX}/bin" ];     then mkdir -p ${PREFIX}/bin; fi
	@if [ "x${INCS}" != "x" ]; then cp -r ${INCS} ${PREFIX}/include/triones/${MODULE}; fi
	@if [ "x${BIN}"  != "x" ]; then cp -f ${BIN}  ${PREFIX}/bin; fi
	@if [ "x${SLIB}" != "x" ]; then cp -f ${SLIB} ${PREFIX}/lib; fi
	@if [ "x${DLIB}" != "x" ]; then cp -f ${DLIB} ${PREFIX}/lib; fi

%.o: %.cpp
	${CXX} -o $@ $< -c ${CXXFLAGS} ${CPPFLAGS} ${INC_PATH}

%.o: %.c
	${CC} -o $@ $< -c ${CFLAGS} ${CPPFLAGS} ${INC_PATH}

var/%.o: ${EXTRA_SRCS}
	mkdir -p ./var
	$(eval SRC_$* := $(filter %$*.c %$*.cpp, $^))
	@if test "$(findstring .cpp,${SRC_$*})"; then \
		echo "${CXX} -o $@ ${SRC_$*} -c ${CXXFLAGS} ${CPPFLAGS} ${INC_PATH}" ; \
		${CXX} -o $@ ${SRC_$*} -c ${CXXFLAGS} ${CPPFLAGS} ${INC_PATH} ; \
	else \
		echo "${CC}  -o $@ ${SRC_$*} -c ${CFLAGS} ${CPPFLAGS} ${INC_PATH}" ; \
		${CC}  -o $@ ${SRC_$*} -c ${CFLAGS} ${CPPFLAGS} ${INC_PATH} ; \
	fi;

${BIN}: ${OBJS}
	${CXX} -o $@ $^ ${LDFLAGS} ${LDLIBS}

${SLIB}: ${OBJS}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

${DLIB}: ${OBJS}
	${CXX} -o $@ $^ ${DLFLAGS} ${LDFLAGS} ${LDLIBS}

