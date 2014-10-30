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

TRIONES_HOME ?= ${shell readlink -n -e ../.. }

MODULE = utils

INCS = base64.h config.h debug.h murhash.h runinfo.h str.h tools.h tqueue.h

SRCS = base64.cpp config.cpp runinfo.cpp str.cpp tools.cpp

SLIB = libtrionesutils.a

EXTRA_INC_PATH = 

EXTRA_LIB_PATH = 

EXTRA_CFLAGS = 

include ${TRIONES_HOME}/Makefile.inc

