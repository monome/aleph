/*
 * The default linker script for standalone executables running on
 * ADSP-BF533 processor.
 *
 * Copyright (C) 2008 Analog Devices, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

MEMORY
{
  MEM_L1_CODE : ORIGIN = 0xFFA00000, LENGTH = 0x10000
  MEM_L1_CODE_CACHE : ORIGIN = 0xFFA10000, LENGTH = 0x4000
  MEM_L1_SCRATCH : ORIGIN = 0xFFB00000, LENGTH = 0x1000
  MEM_L1_DATA_B : ORIGIN = 0xFF900000, LENGTH = 0x8000
  MEM_L1_DATA_A : ORIGIN = 0xFF800000, LENGTH = 0x8000
  MEM_SDRAM : ORIGIN = 0x00000000, LENGTH = 64 * 1024 * 1024
  MEM_L2 : ORIGIN = 0xFEB00000, LENGTH = 0x0
}


/* The default linker script, for single core blackfin standalone executables */
OUTPUT_FORMAT("elf32-bfin", "elf32-bfin", "elf32-bfin") 
OUTPUT_ARCH(bfin) 
ENTRY(__start)
 
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  PROVIDE (__executable_start = 0x0); . = 0x0;
 
  .sdram.data :
  {
  	. = 0x10000;
	*(.sdram.data)
  } >MEM_SDRAM =0
 
  .l2             :
  {
    *(.l2 .l2.*)
  } >MEM_L2 =0
 
  .text           :
  {
    *(.text .text.* .l1.text .l1.text.*)
    KEEP (*(.text.*personality*))
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
  } >MEM_L1_CODE =0
 
  .init           :
  {
    KEEP (*(.init))
  } >MEM_L1_CODE =0
 
  .fini           :
  {
    KEEP (*(.fini))
  } >MEM_L1_CODE =0
 
  .rodata         : 
  { 
    *(.rodata .rodata.*) 
  } >MEM_L1_DATA_A

  .data           :
  {
    *(.data .data.* .l1.data .l1.data.*)
    KEEP (*(.*personality*))
  } >MEM_L1_DATA_A
 
  .bss            :
  {
   __bss_start = .;
    *(.bss .bss.*)
    *(COMMON)
    __bss_end = .;
  } >MEM_L1_DATA_A
 
  . = ALIGN(32 / 8);
  . = ALIGN(32 / 8);
 
  __end = .; PROVIDE (_end = .);
  __stack_start = ORIGIN(MEM_L1_SCRATCH);
  __stack_end   = ORIGIN(MEM_L1_SCRATCH) + 0x200;
 
  /DISCARD/ : { *(.note.GNU-stack) }
}
