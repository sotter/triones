AR       = ar
ARFLAGS  = -ruv
RANLIB   = ranlib

MAKEFLAGS += --no-print-directory

TRIONES_HOME ?= ${shell readlink -n -e .. }
PREFIX       ?= ${TRIONES_HOME}

MODS = comm net pack tinyxml utils

net: comm pack utils

pack: comm utils

.PHONY: all install clean ${MODS}

#ALL_OBJS  = ${MODS:%=%/var/*.o}
ALL_OBJS += ${MODS:%=%/*.o}

all: ${MODS}

install: ${MODS}
	${AR} ${ARFLAGS} ${PREFIX}/lib/libtriones.a ${ALL_OBJS}
	${RANLIB} ${PREFIX}/lib/libtriones.a

clean: ${MODS}

${MODS}:
	${MAKE} ${MAKECMDGOALS} -C $@

