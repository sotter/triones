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

MODULE = pack

INCS = commproto.h gbhandler.h gbheader.h msg_808.h msgpack.h pack.h packfactory.h tprotocol.h

SRCS = gbhandler.cpp pack.cpp tprotocol.cpp

SLIB = libtrionespack.a

EXTRA_INC_PATH = 

EXTRA_LIB_PATH = 

EXTRA_CFLAGS = 

include ${TRIONES_HOME}/Makefile.inc

