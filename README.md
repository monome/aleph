# aleph
monome aleph firmware, built against common avr32 library


[ http://www.monome.org/docs/aleph ]

## quick start

is there an sdcard in it? there should be... then try:

- hold mode switch (square, upper right) while powering up to launch the bootloader [*]

- load an avr32 application from hex, in /hex folder on sdcard

- the unit should now reset and launch the chosen application. 

use usb cable and minicom/kermit to connect to aleph device port and get debug messages. 

currently using baudrate 115200 and a device spelled something like /dev/ttyACM0. [**] 

hints: 
```
sudo stty -F /dev/ttyACM0 115200
minicom -s
```
(serial port setup; then 'E' for bps; then 'a' until you see 11500; then 'f' OFF )


[*] for the "green sandwich" proto, mode switch is broken, bootloader toggle is SW4.

[**] for the "green sandwich" proto, the device is called /dev/ttyUSBx on linux, looks like FTDI,
and you may need to lower the baudrate.


## device description

### hardware:

aleph contains two processors:

1.	atmel AVR32 A0512.
	handles screen, encoders, panel switches, footswitches, USB, I2C, sdcard, and CV input.

2.	analog devices blackfin bf533.
	handles audio I/O, DSP processing, and CV output.

let's nickname these two processors the "controller" and the "dsp," respectively.

the controller is booted from internal flash memory.
it is connected to the dsp by an SPI bus (a fast, 3-wire serial protocol.)
to run a given audio program, the controller resets the dsp and sends a bootable binary image (.ldr) over the SPI bus.
with the dsp running, the same SPI bus carries parameter changes.

audio programs share a common format and protocol, much like an Audio Unit or VST plugin in software. 
each has a number of settable parameters, and metadata/descriptors for each.

the controller has a custom bootloader, which runs when a certain key combination is detected at powerup.
this bootloader is capable of flashing the internal memory with a new executable binary (.elf)


### software:

aleph will ship with a controller application called BEES. 
this is a relatively large and complex program designed to be as general-purpose as possible. 
it can arbitrarily and dynamically route and process control rate input, mangage multiple audio modules, 
store and recall configurations from sdcard.

but for some users and applications, it may make more sense to use or develop a single-purpose application 
(e.g. a loop pedal, monosynth, or some exotic experiment.) 
to encourage this kind of work, we will try to provide a relatively high-level API 
over the layers of software that communicate most directly with hardware on the avr32.
the app developer will presented with an array of handlers for handling UI or controller input, 
and an variety of functions for sending data to the dsp, to the screen, or to attached digital devices
(such as monomes, laptops, midi devices, and other alephs.)

the dsp side of the aleph presents another area of extendability. 
the different audio modules are similarly simple, 
building on both a hardware-specific firmware layer and a library of abstract fixed-point audio processors.


## code directory structure

### controller

`aleph/avr32_lib/src`
	contains default low-level routines specific to the Aleph hardware.
	application authors can use these sources, or replace them with their own as desired.

`aleph/avr32_lib/asf-x.x.x`
	a copy of the Atmel Software Framework.	
	this contains low-level drivers and utilies provided by Atmel, lightly modified by us.

`aleph/apps` 
	contains a number of project directories which define actual functionality for the controller.
	each directory should contain app-specific sources and Makefile.

`aleph/apps/bees`
	this is our main control routing/management application.

`aleph/common`
	routines that both the dsp and the controller need to agree on.
	this includes the SPI protocol and fixed-point math routines.

aleph/utils/avr32_boot  : 
	avr32 bootloader and hardware test program.

### DSP

`aleph/bfin_lib/src`	
	low-level sources for audio programs.
	these routines interact with the blackfin hardware directly, 
	and should be changed only with great care to avoid damaging the proces

`aleph/modules`
	contains a number of project directories for specific audio modules.
	
`aleph/dsp`
	common audio functions implemented in 32-bit fixed-point,
	(envelopes, filters, buffers, oscillators, etc);
	these should be freely added to as needed by new modules.	
	currently there is no rigid class structure to these things, but we could impose one.

`aleph/dsp/null`
	a clunky little portaudio-based audio wrapper, 
	suitable for preliminary development of new audio modules.



# development setup

clone git repository: 
`git clone https://github.com/tehn/aleph.git`



## AVR32

for mac:
[https://github.com/droparea/avr32-toolchain]

for linux:

get the toolchain and header files from atmel.com.

currently the page is here, (but it could move)
[http://www.atmel.com/tools/ATMELAVRTOOLCHAINFORLINUX.aspx]
you will need to sign up / login.

get the 32-bit linux toolchain. ** IMPORTANT: GET 32-BIT REGARDLESS OF YOUR HOST ARCHITECTURE. **  also get the atmel headers:
`avr32-gnu-toolchain-3.4.2.435-linux.any.x86.tar.gz`
`atmel-headers-6.1.3.1475.zip`

(these are the versions we use. feel free to try something newer, paying attention to potential conflicts with ASF and avr32_lib srouces.)

extracting the toolchain archive gives you something like 
`avr32-gnu-toolchain-linux_x86/`

move/rename this directory however you like and make sure the binaries are in your $PATH

extracting the headers gives you
atmel-headers-6.1.3.1475

move or copy the headers subdirectories avr/ and avr32/ to the avr32/include/ subdirectory of the toolchain location.
no, i have no idea why atmel chose to do it this way!

here is an example of the whole process:


```
cd ~/Downloads

tar -xzf avr32-gnu-toolchain-3.4.2.435-linux.any.x86.tar.gz
unzip atmel-headers-6.1.3.1475.zip

mv avr32-gnu-toolchain-linux_x86 ~/avr32-gnu-toolchain
PATH=$PATH:~/avr32-gnu-toolchain/bin

cd atmel-headers-6.1.3.1475
cp avr/ ~/avr32-gnu-toolchain/avr32/include -R
cp avr32/ ~/avr32-gnu-toolchain/avr32/include/ -R
```

you should now be able to run 'make' from aleph/apps/bees and produce aleph-bees.hex.

## BLACKFIN

general instructions are here:
[ http://blackfin.uclinux.org/doku.php?id=toolchain:installing ]

we are only building standalone binaries (no uclinux), so only the bfin-elf-gcc toolchain is needed. 

get the most recent stable release for your architecture (2012-RC2 as of this writing), unpack it and add the binaries to your path. 

example: 
visit: [ http://sourceforge.net/projects/adi-toolchain/files/2012R2/2012R2-RC2/i386/blackfin-toolchain-elf-gcc-4.3-2012R2-RC2.i386.tar.bz2/download ]

```
cd ~/Downloads
su
mv blackfin-toolchain-elf-gcc-4.3-2012R2-RC2.i386.tar.bz2 /
cd /
tar -xjvf blackfin-toolchain-elf-gcc-4.3-2012R2-RC2.i386.tar.bz2
export PATH=$PATH:/opt/uClinux/bfin-elf/bin
```

the toolchain will be extracted to ./opt/uClinux by default.
of course you can use a different location if you like.
you probably also want to add the path to your .bashrc or whatever.

you should now be able to run 'make' from aleph/modules/lines or any other module.
