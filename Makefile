CC = gcc
AR = ar
CP = cp
ARMCC = arm-elf-gcc
ARMAS = arm-elf-as
#ARMAS = arm-linux-as
OBJCOPY = arm-elf-objcopy
BIN2C = bin2c
UNAME := $(shell uname -s)
ADDOBJ = ""
EXPLOITS = exploits/limera1n/limera1n.o exploits/steaks4uce/steaks4uce.o

ifeq ($(UNAME),Darwin)
	CFLAGS = -I./include -I./resources -I/usr/local/include -I/opt/local/include
	# MacPorts
	MACPORTS_LIB = /opt/local/lib
	ifneq ($(wildcard $(MACPORTS_LIB)),)
		OPT_LIB = $(MACPORTS_LIB)
		LUSB_LIB_NAME = usb-1.0
	else
		# Fink
		FINK_LIB = /sw/lib
		ifneq ($(wildcard $(FINK_LIB)),)
			OPT_LIB = $(FINK_LIB) 
			LUSB_LIB_NAME = usb
			ARMAS = arm-linux-as
			# FIXME: fix Fink 32bit build
			CFLAGS := $(CFLAGS) -m32
		else
			ERROR = Need MacPorts of Fink to link with libusb	
		endif
	endif

	LDFLAGS = -L/usr/lib -L$(OPT_LIB) -l$(LUSB_LIB_NAME) -lcurl -lz -framework CoreFoundation -framework IOKit
	LDFLAGS_STATIC = -L/usr/lib $(OPT_LIB)/lib$(LUSB_LIB_NAME).a -lcurl -lz -framework CoreFoundation -framework IOKit
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
ifdef ERROR
	$(error $(ERROR))
endif

	make -C tools
	make -C exploits
	$(CC) $(CFLAGS) -c libpois0n.c libirecovery.c libpartial.c common.c
	$(AR) rs libpois0n.a libpois0n.o libirecovery.o libpartial.o common.o $(EXPLOITS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o injectpois0n injectpois0n.c libpois0n.a $(ADDOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS_STATIC) -o tetheredboot tetheredboot.c libpois0n.a $(ADDOBJ)	
		
clean:
	make clean -C tools
	make clean -C exploits
	rm -rf *.o libpois0n.a injectpois0n tetheredboot
