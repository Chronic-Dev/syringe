include Makefile.common

all:
ifdef ERROR
	$(error $(ERROR))
endif

	make -C tools
	make -C exploits
	$(CC) $(CFLAGS) -c libpois0n.c libirecovery.c libpartial.c common.c
	$(AR_RS) libpois0n.a libpois0n.o libirecovery.o libpartial.o common.o $(EXPLOITS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o injectpois0n injectpois0n.c libpois0n.a $(ADDOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS_STATIC) -o tetheredboot tetheredboot.c libpois0n.a $(ADDOBJ)	
		
clean:
	make clean -C tools
	make clean -C exploits
	rm -rf *.o libpois0n.a injectpois0n tetheredboot
