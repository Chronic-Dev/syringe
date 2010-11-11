CC = gcc
AR = ar
CP = cp
ARMCC = arm-elf-gcc
ARMAS = arm-elf-as
OBJCOPY = arm-elf-objcopy
BIN2C = bin2c
ADDOBJ = ""

EXPLOITS_TARGET = exploits.a
EXPLOITS_DEPENDENCIES = 

POIS0N_TARGET = libpois0n.a
POIS0N_DEPENDENCIES = $(EXPLOITS_TARGET)

SYRINGE_TARGET = syringe
SYRINGE_DEPENDENCIES = $(POIS0N_TARGET)

TARGETS = $(EXPLOITS_TARGET) $(POIS0N_TARGET) $(SYRINGE_TARGET)

UNAME := $(shell uname -s)
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

$(EXPLOITS_TARGET):
	$(MAKE) -C exploits

$(POIS0N_TARGET): $(POIS0N_OBJECTS)

$(SYRINGE_TARGET): $(SYRINGE_OBJECTS)

all: $(TARGETS)
	make -C tools
	make -C exploits
	$(CC) $(CFLAGS) -c libpois0n.c libirecovery.c libpartial.c common.c
	$(AR) rs libirecovery.a libirecovery.o libpartial.o common.o
	$(AR) rs libsyringe.a libpois0n.o libirecovery.a $(EXPLOITS_TARGET)
	$(CC) $(CFLAGS) $(LDFLAGS) -o injectpois0n injectpois0n.c libsyringe.a $(ADDOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o tetheredboot tetheredboot.c libsyringe.a $(ADDOBJ)	
	$(CC) $(CFLAGS) $(LDFLAGS) -o loadibec loadibec.c libirecovery.a $(ADDOBJ)	
		
clean:
	make clean -C tools
	make clean -C exploits
	rm -rf *.o libpois0n.a injectpois0n tetheredboot
