XXD = xxd
CROSS = arm-elf-
ARMCC = $(CROSS)gcc
ARMAS = $(CROSS)as
ARMOBJCOPY = $(CROSS)objcopy

PAYLOAD_TARGET = $(PAYLOAD_NAME).h
PAYLOAD_SOURCE = $(PAYLOAD_NAME).S

ARMCFLAGS = -Wa,-mthumb

$(PAYLOAD_TARGET): $(PAYLOAD_SOURCE)
#	$(ARMCC) -o $(PAYLOAD_NAME)_payload.o -c $(PAYLOAD_SOURCE) $(ARMCFLAGS) $(ARMLDFLAGS)
	$(ARMAS) -mthumb -o $(PAYLOAD_NAME)_payload.o $(PAYLOAD_SOURCE) 
	$(ARMOBJCOPY) -O binary $(PAYLOAD_NAME)_payload.o $(PAYLOAD_NAME).payload
	$(XXD) -i $(PAYLOAD_NAME).payload $(PAYLOAD_TARGET)

all: $(PAYLOAD_TARGET)

clean_all: clean

clean:
	rm -f $(PAYLOAD_NAME)_payload.o $(PAYLOAD_NAME).payload $(PAYLOAD_TARGET)
