// case CMD_LS:
        // Check if params are correct or mount needed.
        if (nav_drive_get() >= nav_drive_nb() || first_ls)
        {
          first_ls = false;
          // Reset navigators .
          nav_reset();
          // Use the last drive available as default.
          nav_drive_set(nav_drive_nb() - 1);
          // Mount it.
          nav_partition_mount();
        }
        // Get the volume name
        nav_dir_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH);
        // Display general informations (drive letter and current path)
        print(SHL_USART, "\r\nVolume is ");
        print_char(SHL_USART, 'A' + nav_drive_get());
        print(SHL_USART, ":\r\nDir name is ");
        print(SHL_USART, str_buff);
        print(SHL_USART, CRLF);
        // Try to sort items by folders
        if (!nav_filelist_first(FS_DIR))
        {
          // Sort items by files
          nav_filelist_first(FS_FILE);
        }
        // Display items informations
        print(SHL_USART, "\tSize (Bytes)\tName\r\n");
        // reset filelist before to start the listing
        nav_filelist_reset();
        // While an item can be found
        while (nav_filelist_set(0, FS_FIND_NEXT))
        {
          // Get and display current item informations
          print(SHL_USART, (nav_file_isdir()) ? "Dir\t" : "   \t");
          print_ulong(SHL_USART, nav_file_lgt());
          print(SHL_USART, "\t\t");
          nav_file_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
          print(SHL_USART, str_buff);
          print(SHL_USART, CRLF);
        }
        // Display the files number
        print_ulong(SHL_USART, nav_filelist_nb(FS_FILE));
        print(SHL_USART, "  Files\r\n");
        // Display the folders number
        print_ulong(SHL_USART, nav_filelist_nb(FS_DIR));
        print(SHL_USART, "  Dir\r\n");
//break;
