# Example Shogi program
#
#   make                 build against the library in this source tree
#                        (../lib is built first when needed)
#   make INSTALLED=y     build against the installed library
#                        (headers in $(INCDIR), libshogi in the linker path)
#   make DEBUG=y         no optimization, for debugging
#   make MCHCK=y         with the address sanitizer
#
# BMI2 and TLS follow the settings in ../Makefile so that the program and
# the library are compiled the same way.

EXEC      = shogiprogram

INSTALLED = n
INCDIR    = /usr/local/include/shogi
LIBDIR    = ../lib
LIBNAME   = libshogi.so.0

DEBUG     = n
MCHCK     = n
BMI2      = $(strip $(shell grep ^BMI2 ../Makefile | cut -d= -f 2))
TLS       = $(strip $(shell grep ^TLS  ../Makefile | cut -d= -f 2))

CC        = g++

CFLAGS   += -std=c++14 -Wall -Wextra -Wformat=2 -Wcast-qual -Wwrite-strings \
            -Wpointer-arith -mtune=native -mpopcnt -pthread

ifeq ($(BMI2),y)
CFLAGS   += -DUSE_NATIVE_PEXTINSTRUCTION -mbmi -mbmi2
endif

ifeq ($(TLS),y)
CFLAGS   += -DUSE_THREADLOCALSTORAGE
endif

ifeq ($(DEBUG),y)
CFLAGS   += -O0 -g
else
CFLAGS   += -O3 -g
endif

ifeq ($(MCHCK),y)
CFLAGS   += -fsanitize=address
endif

ifeq ($(INSTALLED),y)
CFLAGS   += -I$(INCDIR)
LFLAGS    = -pthread -lrt -lshogi
LIBDEP    =
else
CFLAGS   += -I$(LIBDIR)/shogi -I$(LIBDIR)/foundation -I$(LIBDIR)/csa \
            -I$(LIBDIR)/utility
LFLAGS    = -pthread -lrt $(LIBDIR)/$(LIBNAME) \
            -Wl,-rpath,$(abspath $(LIBDIR))
LIBDEP    = $(LIBDIR)/$(LIBNAME)
endif

all: $(EXEC)

$(LIBDIR)/$(LIBNAME):
	$(MAKE) -C $(LIBDIR)

Main.o: Main.cpp $(LIBDEP)
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXEC): Main.o $(LIBDEP)
	$(CC) $(CFLAGS) -o $@ Main.o $(LFLAGS)

clean:
	rm -f *.o $(EXEC)

.PHONY: all clean
