README
deer/b533/core

this directory contains core routines for initializing the blackfin 533 processor to communicate with the AD1836 audio codec and the AVR32. 
custom audio processing modules should include this directory and define their own process_frame() function. 

NOTES

on the ADSP-BF533 EZ-KIT development board, pins 5 and 6 of SW9 must be turned ON to connect RSCLK0 -> TSCLK0 and RFS0 -> TFS0. 
thus, the blackfin will use the bit and frame clocks from the codec for both receive and transmit.
