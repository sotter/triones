MAKE     = make
AR       = ar
ARFLAGS  = -ruv
RANLIB	 = ranlib

TRIONES_HOME ?= ${shell readlink -n -e .. }

SUB_DIRS =  comm net pack utils user tinyxml
OBJ_DIRS =  comm net pack utils user tinyxml

all: 
	@for dir in ${SUB_DIRS}; do \
		echo "cd $$dir; ${MAKE} all"; \
		(cd $$dir && ${MAKE} --no-print-directory all) || exit 1; \
	done

clean: 
	@for dir in ${SUB_DIRS}; do \
		echo "cd $$dir; ${MAKE} clean"; \
		(cd $$dir && ${MAKE} --no-print-directory clean) || exit 1; \
	done

ALL_OBJS=${OBJ_DIRS:%=%/var/*.o}

install: 
	@for dir in ${SUB_DIRS}; do \
		echo "cd $$dir; ${MAKE} install"; \
		(cd $$dir && ${MAKE} --no-print-directory install) || exit 1; \
	done
	
	${AR} ${ARFLAGS} ${TRIONES_HOME}/lib/libtriones.a ${ALL_OBJS}
	${RANLIB} ${TRIONES_HOME}/lib/libtriones.a
