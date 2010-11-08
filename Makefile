CC = gcc
AR = ar
CP = cp
ARMCC = arm-elf-gcc
ARMAS = arm-elf-as
OBJCOPY = arm-elf-objcopy
BIN2C = bin2c
UNAME := $(shell uname -s)
ADDOBJ = ""
EXPLOITS = exploits/limera1n/limera1n.o exploits/steaks4uce/steaks4uce.o

ifeq ($(UNAME),Darwin)
	CFLAGS = -I./include -I./resources -I/usr/local/include -I/opt/local/include
	LDFLAGS = -L/usr/lib -L/opt/local/lib -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit
	ADDOBJ = 
else
	ifeq ($(UNAME),MINGW32_NT-5.1)
		CFLAGS = -O3 -I./resources -DCURL_STATICLIB
		LDFLAGS = -lcurl -lz
		ADDOBJ = /mingw/lib/libcurl.a /mingw/lib/libwsock32.a /mingw/lib/libwldap32.a /mingw/lib/libpenwin32.a /mingw/lib/libz.a /mingw/lib/libsetupapi.a
	else
		CFLAGS = -O3 -I./resources -I./include
		LDFLAGS = -lusb-1.0 -lcurl -lz
		ADDOBJ = 
	endif
endif

all:
	make -C tools
	make -C exploits
	$(CC) $(CFLAGS) -c libpois0n.c libirecovery.c libpartial.c common.c
	$(AR) rs libpois0n.a libpois0n.o libirecovery.o libpartial.o common.o $(EXPLOITS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o injectpois0n injectpois0n.c libpois0n.a $(ADDOBJ)
		
clean:
	make clean -C tools
	make clean -C exploits
	rm -rf *.o libpois0n.a injectpois0n
