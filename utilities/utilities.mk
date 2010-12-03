INCLUDES = ../include
SOURCES = $(TARGET).c

LIBRARIES = syringe usb-1.0 curl z
LDFLAGS = -L../syringe -L/opt/local/lib

include ../common.mk