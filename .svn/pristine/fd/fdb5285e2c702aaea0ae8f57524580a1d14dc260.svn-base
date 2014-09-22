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

MODULE = vos

INCS = vos_event.h vos_lock.h vos_sem.h vos_thread.h vos_timer.h vos_util.h vos.h

SRCS = vos_event.cpp vos_sem.cpp vos_thread.cpp vos_timer.cpp 

SLIB = libtrionesvos.a

EXTRA_LIB_PATH = 

EXTRA_CFLAGS = 

include ${TRIONES_HOME}/Makefile.inc

