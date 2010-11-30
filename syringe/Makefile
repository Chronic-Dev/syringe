# /syringe/Makefile

SUBDIRS = exploits

MKSTATICLIB = 1

INCLUDES = ../include ../include/resources

SOURCES = \
	common.c		\
	libirecovery.c	\
	libpartial.c	\
	libpois0n.c		\

PREMADE_OBJECTS = exploits/limera1n.o exploits/steaks4uce.o

LIBRARIES = usb-1.0 curl z
LDFLAGS = -L/opt/local/lib

TARGET = syringe

include ../common.mk
