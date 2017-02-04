
INCDIR  = /usr/local/include/shogi

EXEC    = shogiprogram

DEBUG   = n
MCHCK   = n
BMI2    = n
TLS     = n

CC      = g++

ifeq ($(BMI2),y)
CFLAGS += -DUSE_NATIVE_PEXTINSTRUCTION
endif

ifeq ($(TLS),y)
CFLAGS += -DUSE_THREADLOCALSTORAGE
endif

ifeq ($(DEBUG),y)
ifeq ($(MCHCK),y)
CFLAGS += -std=c++14 -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 \
          -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion \
          -Wfloat-equal -Wpointer-arith -Wswitch-enum \
          -O0 -g -mtune=native -ftrapv -fthreadsafe-statics \
          -fsanitize=address \
		  -I$(INCDIR)
LFLAGS  =  -pthread -lrt -lshogi
else
CFLAGS += -std=c++14 -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 \
          -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion \
          -Wfloat-equal -Wpointer-arith -Wswitch-enum \
          -O0 -g -mtune=native -fthreadsafe-statics \
		  -I$(INCDIR)
LFLAGS  = -pthread -lrt -lshogi
endif
else
ifeq ($(MCHCK),y)
CFLAGS += -std=c++14 -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 \
          -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion \
          -Wfloat-equal -Wpointer-arith -Wswitch-enum \
		  -O3 -g -mtune=native -funroll-loops \
          -fthreadsafe-statics -ftrapv -fsanitize=address \
		  -I$(INCDIR)
LFLAGS  = -pthread -lrt -lshogi
else
CFLAGS += -std=c++14 -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 \
          -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion \
          -Wfloat-equal -Wpointer-arith -Wswitch-enum \
		  -O3 -g -mtune=native -funroll-loops \
          -fthreadsafe-statics \
		  -I$(INCDIR)
LFLAGS  = -lpthread -lrt -lshogi
endif
endif

all: $(EXEC)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXEC): Main.o $(HEADERS) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

clean:
	rm -f *.o $(EXEC)
