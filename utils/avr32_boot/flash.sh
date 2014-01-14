make chiperase
make program
avr32program writefuses -f internal@80000000 gp=0xfff3ffff
avr32program secureflash
