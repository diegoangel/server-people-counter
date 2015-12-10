# The following definitions correspond to each of the three BlackfinAPI libraries,
# uncomment the set which correspond to the library you want to build against

# IMPORTANT: set "ulimit -c unlimited" on your system to allow the generation of core files, if you want to 
# take advantages of the debugging flags on the build instructions

# OS architecture
ARCH := $(shell getconf LONG_BIT)

ifeq ($(ARCH),64)
	# 64 bit build
	CC = g++
	CFLAGS = -Wall -std=gnu++11 -O2 -gdwarf-2 -g3 -I../BlackfinAPI/include/ -m64 -Iinclude/ #-I/home/movilgate/include/libmemcached/
	LFLAGS = -L../BlackfinAPI/lib -lBlackfinAPIx64 -m64 -lstdc++ -lpthread -lmemcached
else
	# 32 bit build
	CC=g++
	CFLAGS = -Wall -O2 -std=gnu++11 -gdwarf-2 -g3 -I../BlackfinAPI/include/ -m32 -Iinclude/
	LFLAGS = -L../BlackfinAPI/lib -lBlackfinAPIx86 -m32 -lstdc++ -lpthread -lmemcached
endif

SRCDIR = src
BUILDDIR = build
TARGET = bin

main: server

mkdir:	
	@echo -e '\n\e[33mPreparando la instalacion del server...\e[0m\n'
	@sleep 2
	@echo -e '\e[33mCreando directorios\e[0m\n'
	@mkdir -p ${BUILDDIR} ${TARGET}
BlackfinServer.o:
	@echo -e '\e[33mCompilando librerias necesarias\e[0m\n'
	@${CC} -c ${SRCDIR}/BlackfinServer.cpp -o ${BUILDDIR}/BlackfinServer.o ${CFLAGS}
MovilgateServerApp.o:
	@echo -e '\e[33mCompilando el ejecutable\e[0m\n'
	@${CC} -c ${SRCDIR}/MovilgateServerApp.cpp -o ${BUILDDIR}/MovilgateServerApp.o ${CFLAGS}

server: mkdir BlackfinServer.o MovilgateServerApp.o
	@${CC} ${BUILDDIR}/MovilgateServerApp.o ${BUILDDIR}/BlackfinServer.o -o ${TARGET}/server-servicio-conteo-personas ${LFLAGS}
	@echo -e '\e[32mEl programa compilo el ejecutable en bin/server-servicio-conteo-personas\e[0m\n'
	@chmod +x ${TARGET}/server-servicio-conteo-personas
	@echo -e '\e[32mSe dieron los permisos necesarios al programa\e[0m\n'
clean: 
	@rm -rf ${BUILDDIR}/* ${TARGET}/*
	@echo -e '\n\e[31mSe eliminaron los archivos de los directorios bin/ y build/ \e[0m\n'

.PHONY: clean
