avrdude -p atmega32u4 -P usb -c avrispmkII -U lfuse:w:0x6e:m -U hfuse:w:0xdf:m -U efuse:w:0xcf:m
