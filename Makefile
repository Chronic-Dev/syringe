CC = gcc
AR = ar
CP = cp
ARMCC = arm-elf-gcc
ARMAS = arm-elf-as
OBJCOPY = arm-elf-objcopy
BIN2C = bin2c
ADDOBJ = 
LDFLAGS = -L.
GIT_COMMIT = $(shell git log | head -n1 | cut -b8-14)
CFLAGS = -DCOMMIT="$(GIT_COMMIT)"

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	CFLAGS += -I./include -I./resources -I/usr/local/include -I/opt/local/include
	LDFLAGS += -L/usr/lib -L/opt/local/lib -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit
else
	ifeq ($(UNAME),MINGW32_NT-5.1)
		CFLAGS += -O3 -I./resources -DCURL_STATICLIB
		LDFLAGS += -lcurl -lz
		ADDOBJ += /mingw/lib/libcurl.a /mingw/lib/libwsock32.a /mingw/lib/libwldap32.a /mingw/lib/libpenwin32.a /mingw/lib/libz.a /mingw/lib/libsetupapi.a
	else
		CFLAGS += -O3 -I./resources -I./include
		LDFLAGS += -lusb-1.0 -lcurl -lz
	endif
endif

all: loadibec tetheredboot injectpois0n

LIBIRECOVERY_LIBS = libirecovery.a
LIBSYRINGE_LIBS = libsyringe.a exploits/exploits.a $(LIBIRECOVERY_LIBS)

force:
	true

exploits/exploits.a: force
	cd exploits && $(MAKE)

tools: force
	$(MAKE) -C tools

libirecovery.a: libirecovery.c tools
	$(CC) $(CFLAGS) -DLIBIRECOVERY_EXPORTS -static -shared -c $<
	$(AR) rs $@ $(patsubst %.c, %.o, $<)

libsyringe.a: libpois0n.c libpartial.c common.c libirecovery.a
	$(CC) $(CFLAGS) -DLIBSYRINGE_EXPORTS -static -shared -c $^
	$(AR) rs $@ $(patsubst %.c, %.o, $(filter %.c, $^))

loadibec: loadibec.c $(LIBSYRINGE_LIBS) $(ADDOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

tetheredboot: tetheredboot.c $(LIBSYRINGE_LIBS) $(ADDOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

injectpois0n: injectpois0n.c $(LIBSYRINGE_LIBS) $(ADDOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
		
clean:
	make clean -C tools
	make clean -C exploits
	rm -rf *.so *.o *.a injectpois0n tetheredboot loadibec
