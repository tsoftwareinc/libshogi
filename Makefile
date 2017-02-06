
TARGET  = /usr/local
HEADDIR = $(TARGET)/include/shogi/
LIBRDIR = $(TARGET)/lib64
LIBNAME = libshogi.so.0
BMI2    = n
TLS     = n

HEADERS = lib/shogi/Bitboard.h lib/shogi/Color.h lib/shogi/Convert.h \
          lib/shogi/Direction.h lib/shogi/Effect.h lib/shogi/Evaluation.h \
          lib/shogi/Move.h lib/shogi/Piece.h lib/shogi/Position.h \
          lib/shogi/Region.h lib/shogi/Shogi.h lib/shogi/Square.h \
          lib/shogi/Zobrist.h \
          lib/foundation/Common.h \
          lib/foundation/Array.h lib/foundation/Atomic.h \
          lib/foundation/BTree.h lib/foundation/HashTree.h \
          lib/foundation/List.h lib/foundation/Semaphore.h \
	      lib/foundation/Mutex.h lib/foundation/SpinLock.h \
          lib/foundation/Thread.h lib/foundation/Vector.h \
		  lib/foundation/BitOperations.h \
          lib/utility/Utility.h lib/csa/CSASummary.h \
          lib/csa/CSAConnection.h lib/csa/CSAFile.h

PRGRMS  = lib/shogi/Direction.cpp lib/shogi/Region.cpp lib/shogi/Square.cpp \
          lib/shogi/Zobrist.cpp lib/shogi/Effect.cpp \
          lib/shogi/Shogi.cpp lib/shogi/Position.cpp \
          lib/shogi/Bitboard.cpp lib/utility/Utility.cpp \
          lib/csa/CSAConnection.cpp lib/csa/CSAFile.cpp lib/csa/CSASummary.cpp

all: lib/$(LIBNAME) tags 

lib/$(LIBNAME): $(HEADERS) $(PRGRMS)
	CFLAGS=$(CFLAGS) make -C lib

tags: $(HEADERS) $(PRGRMS)
	ctags -R .

install: lib/$(LIBNAME)
	mkdir -p $(HEADDIR)
	mkdir -p $(LIBRDIR)
	install -m 644 $(HEADERS) $(HEADDIR)
	install -m 755 lib/$(LIBNAME) $(LIBRDIR)
	ln -fs $(LIBNAME) $(LIBRDIR)/libshogi.so

uninstall:
	rm -rf $(HEADDIR)
	rm -f  $(LIBRDIR)/$(LIBNAME)
	rm -f  $(LIBRDIR)/libshogi.so

clean:
	make -C lib clean
	make -C example clean
	make -C regression clean
	rm -f tags
