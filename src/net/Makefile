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

INCS = cnet.h atomic.h iserveradapter.h socket.h serversocket.h socketevent.h iocomponent.h
INCS += tcpacceptor.h tcpcomponent.h udpacceptor.h udpcomponent.h transport.h baseservice.h
INCS += sockutil.h tbtimeutil.h

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
SRCS += sockutil.cpp


SLIB = libtrionesnet.a

EXTRA_INC_PATH = 

EXTRA_LIB_PATH = 

EXTRA_CFLAGS = -Wno-deprecated 

include ${TRIONES_HOME}/Makefile.inc

