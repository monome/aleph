CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega328p
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref	
OBJ2HEX=avr-objcopy 

###### i2c params
ADDR := 80

####### Files:

SOURCES	= i2c-repeater.c twi.c
OBJECTS	= i2c-repeater.o twi.o
TARGET	= i2c-repeater

####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

%.elf:	$(OBJECTS)


####### Build rules

$(TARGET):	$(OBJECTS)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
		$(CC) $(ALL_CFLAGS) $(OBJECTS) --output $(TARGET).elf $(LDFLAGS)	
		$(OBJ2HEX) -R .eeprom -O ihex $(TARGET) $(TARGET).hex

erase:
	avrdude -p m328p -b 115200 -P usb -c avrispmkII -e 

fuses: 
	avrdude -p m168 -b 115200 -P usb -c avrispmkII -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m

flash: $(TARGET).hex
	avrdude -p m168 -b 115200 -P usb -c avrispmkII -e -V -D -U flash:w:$(TARGET).hex:i

p: $(TARGET).hex
	avrdude -p m328p -b 115200 -P usb -c avrispmkII -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m -U flash:w:$(TARGET).hex:i

boot: $(TARGET).hex
	avrdude -p m168 -b 115200 -P $(SERIAL) -c arduino -e -D -U flash:w:$(TARGET).hex:i


####### Compile

i2c-repeater.o:		i2c-repeater.c
twi.o:				twi.c