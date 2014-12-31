.DEFAULT_GOAL = auto
CPU_NUM   ?= ${shell getconf _NPROCESSORS_ONLN }
MAKEFLAGS += -j${CPU_NUM} --no-print-directory
PREFIX    ?= $(shell pwd)

MODS = src

.PHONY: auto tar all clean install tar examples clean-examples ${MODS}

auto:
	make clean
	make all
	make install

all: ${MODS}

clean: ${MODS}
	rm -rf lib/*
	rm -rf include/*

install: ${MODS}

tar:
	@if [ ! -d "${PREFIX}/triones" ]; then mkdir -p ${PREFIX}/triones; fi
	rsync -a --exclude=".*" ./bin ${PREFIX}/triones
	rsync -a --exclude=".*" ./lib ${PREFIX}/triones
	rsync -a --exclude=".*" ./include ${PREFIX}/triones
	tar -cvf ${PREFIX}/triones.tar.gz ${PREFIX}/triones

${MODS}:
	$(MAKE) -C $@ $(MAKECMDGOALS) 

examples: 
	${MAKE} -C examples all

clean-examples:
	${MAKE} -C examples clean
