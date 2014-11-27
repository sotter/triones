MAKEFLAGS += --no-print-directory

SUB_DIRS = src

.PHONY: all clean install examples clean-examples ${SUB_DIRS}


all: ${SUB_DIRS}

clean: ${SUB_DIRS}
	rm -rf lib/*
	rm -rf include/*

install: ${SUB_DIRS}

${SUB_DIRS}:
	$(MAKE) -C $@ $(MAKECMDGOALS) 

examples: 
	${MAKE} -C examples all

clean-examples:
	${MAKE} -C examples clean
