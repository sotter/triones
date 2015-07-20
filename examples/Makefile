MAKE     = make

SUB_DIRS = baseclient baseserver chatxml chatserver chatclient 

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
	rm -rf lib/*
	rm -rf include/*
 
install: 
	@for dir in ${SUB_DIRS}; do \
		echo "cd $$dir; ${MAKE} install"; \
		(cd $$dir && ${MAKE} --no-print-directory install) || exit 1; \
	done
