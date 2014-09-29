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

MODULE = net

SRCS += iocomponent.cpp
SRCS += serversocket.cpp
SRCS += socket.cpp
SRCS += socketevent.cpp
SRCS += tcpacceptor.cpp
SRCS += tcpcomponent.cpp
SRCS += udpacceptor.cpp
SRCS += udpcomponent.cpp
SRCS += stats.cpp
SRCS += tbtimeutil.cpp
SRCS += transport.cpp
SRCS += baseservice.cpp


SLIB = libtrionesnet.a

EXTRA_INC_PATH = 

EXTRA_LIB_PATH = 

EXTRA_CFLAGS = 

include ${TRIONES_HOME}/Makefile.inc

