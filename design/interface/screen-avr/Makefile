CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega32u4
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref	
OBJ2HEX=avr-objcopy 

SERIAL = /dev/tty.usbmodemfd111

###### i2c params
ADDR := 100


####### Files:

SOURCES	= native.c
OBJECTS	= native.o
TARGET	= native

####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.c.o:
	$(CC) -c -DADDR="$(ADDR)" $(CFLAGS) $(INCPATH) -o $@ $<

%.elf:	$(OBJECTS)


####### Build rules

$(TARGET):	$(OBJECTS)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
		$(CC) $(ALL_CFLAGS) $(OBJECTS) --output $(TARGET).elf $(LDFLAGS)	
		$(OBJ2HEX) -R .eeprom -O ihex $(TARGET) $(TARGET).hex

erase:
	avrdude -p m32u4 -b 115200 -P usb -c avrispmkII -e 

fuses: 
	avrdude -p m32u4 -b 115200 -P usb -c avrispmkII -U lfuse:w:0xcf:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m

flash: $(TARGET).hex
	avrdude -p m32u4 -b 115200 -P usb -c avrispmkII -e -V -D -U flash:w:$(TARGET).hex:i

p: $(TARGET).hex
	avrdude -p m32u4 -b 115200 -P usb -c avrispmkII -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0x03:m -U flash:w:$(TARGET).hex:i

boot: $(TARGET).hex
	avrdude -p m32u4 -b 115200 -P $(SERIAL) -c avr109 -e -D -U flash:w:$(TARGET).hex:i


####### Compile

native.o:		native.c