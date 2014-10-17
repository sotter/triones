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

MODULE = comm

INCS = atomic.h comlog.h clog.h cond.h databuffer.h dataqueue.h netorder.h tqueue.h exception.h monitor.h mutex.h queuethread.h ref.h sem.h singleton.h thread.h util.h

SRCS = clog.cpp  monitor.cpp mutex.cpp queuethread.cpp ref.cpp sem.cpp thread.cpp util.cpp

SLIB = libcomm.a

EXTRA_LIB_PATH = 

include ${TRIONES_HOME}/Makefile.inc

