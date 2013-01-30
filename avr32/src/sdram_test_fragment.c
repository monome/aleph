  
  ////////
  /// SDRAM test

  /*
  sdram_size = SDRAM_SIZE >> 2;
  print_dbg("\x0CSDRAM size: ");
  print_dbg_ulong(SDRAM_SIZE >> 20);
  print_dbg(" MB\r\n");

  // Initialize the external SDRAM chip.
  sdramc_init(FMCK_HZ);
  print_dbg("SDRAM initialized\r\n");

  // Determine the increment of SDRAM word address requiring an update of the
  // printed progression status.
  progress_inc = (sdram_size + 50) / 100;
  
  // Fill the SDRAM with the test pattern.
  for (i = 0, j = 0; i < sdram_size; i++)
  {
    if (i == j * progress_inc)
    {
       print_dbg("\rFilling SDRAM with test pattern: ");
      print_dbg_ulong(j++);
      print_dbg_char('%');
    }
    sdram[i] = i;
  }
   print_dbg("\rSDRAM filled with test pattern       \r\n");

  // Recover the test pattern from the SDRAM and verify it.
  for (i = 0, j = 0; i < sdram_size; i++)
  {
    if (i == j * progress_inc)
    {
      print_dbg("\rRecovering test pattern from SDRAM: ");
      print_dbg_ulong(j++);
      print_dbg_char('%');
    }
    tmp = sdram[i];
    if (tmp != i)
    {
      noErrors++;
    }
  }
  print_dbg("\rSDRAM tested: ");
  print_dbg_ulong(noErrors);
  print_dbg(" corrupted word(s)       \r\n");
*/
  ////////
