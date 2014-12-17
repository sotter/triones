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

MAKE     = make --no-print-directory 
AR       = ar
ARFLAGS  = -ruv
RANLIB	 = ranlib
CXX      = g++
CC       = gcc
CFLAGS   = -g -Wall -Wextra -Werror -Wno-deprecated -fPIC 
CXXFLAGS = ${CFLAGS} ${EXTRA_CFLAGS}
DLFLAGS  = -shared -Wl,-soname,${DLIB}

INC_DIR  = ${TRIONES_HOME}/include/triones/${MODULE}
LIB_DIR  = ${TRIONES_HOME}/lib
BIN_DIR  = ${TRIONES_HOME}/bin

INC_PATH = -I. ${EXTRA_INC_PATH}
LIB_PATH = -L. ${EXTRA_LIB_PATH}
LIBS     = ${EXTRA_LIBS}

OBJS     = $(addprefix ./var/, $(addsuffix .o, $(basename $(SRCS))))

all: ${BIN} ${SLIB} ${DLIB}

clean: 
	rm -rf ./var ${BIN} ${SLIB} ${DLIB}

install:
	@if [ ! -d "${INC_DIR}" ]; then mkdir -p ${INC_DIR}; fi
	@if [ ! -d "${LIB_DIR}" ]; then mkdir -p ${LIB_DIR}; fi
	@if [ ! -d "${BIN_DIR}" ]; then mkdir -p ${BIN_DIR}; fi
	@if [ "x${INCS}" != "x" ]; then cp -r ${INCS} ${INC_DIR}; fi
	@if [ "x${BIN}" != "x" ];  then cp ${BIN} ${BIN_DIR}; fi
	@if [ "x${SLIB}" != "x" ]; then cp ${SLIB} ${LIB_DIR}; fi
	@if [ "x${DLIB}" != "x" ]; then cp ${DLIB} ${LIB_DIR}; fi

./var/%.o: %.cpp
	mkdir -p ./var
	${CXX} -o $@ -c ${CXXFLAGS} ${INC_PATH} $< 

./var/%.o: %.c
	mkdir -p ./var
	${CC} -o $@ -c ${CXXFLAGS} ${INC_PATH} $< 

${BIN}: ${OBJS}
	${CXX} -o $@ ${CXXFLAGS} ${INC_PATH} ${LIB_PATH} ${LIBS} $^ 

${SLIB}: ${OBJS}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

${DLIB}: ${OBJS}
	${CXX} -o $@ ${DLFLAGS} ${CXXFLAGS} ${INC_PATH} ${LIB_PATH} ${LIBS} $^ 

