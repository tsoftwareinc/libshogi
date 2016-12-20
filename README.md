#libshogi

## REQUIREMENTS

   - x64
   - POSIX system that supports Berkeley sockets interface
   - gcc 5.1 or higher that supports c++14

   We tested on Fedora25, Ubuntu 16.04 and Linux Mint 18. For users
   newly installing OS for libshogi, Linux Mint 18 is recommended.
   For an optimal environment, choose the minimal option on the
   installation of Fedora 25 Server and yum install 'gcc-c++', 
   'make' and 'ctags' afterwards. It takes only 20 min approximately
   to get ready to use the software. libshogi should work on other
   POSIX systems such as xBSDs although we have not tested yet.


## INSTALL

   Entering

```
   $ make
```

   will compile the source codes. Then, install the library to your
   host.

```
   $ sudo make install
```


## RECEIPE

   libshogi/Makefile has several options. 

```Makefile
   TARGET  = /usr/local
   HEADDIR = $(TARGET)/include/shogi/
   LIBRDIR = $(TARGET)/lib64
   BMI2    = n
   TLS     = n
```

   TARGET is a top directory the library is installed to. HEADDIR
   is a subsidiary directory the header files are placed. LIBDIR
   is also a tributary for the library file.

   If your processor supports BMI2 (`cat /proc/cpuinfo | grep bmi2`
   says something - on linux) you should use native PEXT instruction
   for the better performance. To do this, you may specify this
   option as:

```
   BMI2    = y
```

   We do not suppose libshogi to be used with frequent copying
   operation about the position. If the cost of copying is still
   an issue in your program, you can make the context cache^1 TLS.
   Using TLS, there is some performance penalty because you have to
   check the address each time you access it. To do this, you may
   specify this option as:

```
   TLS     = y
```

   1:context cache
   doesn't hold information of positions. It contains the checking
   pieces, number of checks, pinned pieces and etc. This varies
   depending on contexts and require some amount of memory
   (approx. 500 bytes per instance).

