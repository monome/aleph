/*****************************************************************************
 *
 * \file
 *
 * \brief Navigation automatic Services.
 *
 * Automatic navigation has been developed for the player and viewer.
 * The plug-in provides a specific "file list".
 * The "file list" contains all files included in a playlist or it is built with the following user specifications :
 *   - extension filter
 *   - limitation scan (folder, folder and sub folder, one disk, all disks)
 * The plug-in can manage a random feature.
 * Note: The play list limitations is 65535 files.
 *
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/


//_____  I N C L U D E S ___________________________________________________
#include "conf_explorer.h"
#include "nav_automatic.h"
#include "file.h"
#include LIB_CTRLACCESS
#include LIB_MEM


//_____ D E F I N I T I O N S ______________________________________________

                  bool           g_navauto_b_playlist;                        //!< If true, then the file list contains files from play list file, else the file list contains files included in directories
_MEM_TYPE_SLOW_   Navauto_mode   g_navauto_exp_mode = NAVAUTO_MODE_DISKS;     //!< Define the directory limitation of the file list (ignored if g_navauto_b_playlist = true )
_MEM_TYPE_SLOW_   Navauto_rand   g_navauto_rand = NAVAUTO_RAND_OFF;           //!< Flag the random state
_MEM_TYPE_SLOW_   uint16_t       g_navauto_u16_nb = 0;                        //!< Number of file included in file list (correct only if define FS_NAV_AUTOMATIC_NBFILE enabled)
_MEM_TYPE_SLOW_   uint16_t       g_navauto_u16_pos;                           //!< Position of selected file in file list
_MEM_TYPE_SLOW_   uint16_t       g_navauto_u16_dir_level;                     //!< Directory level of selected file (if play list file used then always 0)
_MEM_TYPE_SLOW_   uint16_t       g_navauto_u16_dir_level_root;
_MEM_TYPE_SLOW_   FS_STRING      g_navauto_filter;                            //!< File extension filter used to create the file list (ignored if g_navauto_b_playlist = true )
_MEM_TYPE_SLOW_   uint16_t       g_navauto_u16_rand_pos;                      //!< Offset of bit table "g_navauto_rand_tab" in file list
_MEM_TYPE_SLOW_   uint8_t        g_navauto_rand_tab[NAVAUTO_MAX_RANGE_RAND];  //!< Bit table used to random the file list. This one flag each files already selected

//! The FS_NAV_AUTOMATIC_NBFILE define changes the rule to create a file list from directories
//! When this define is enabled, the files from a directory are included before files from sub directories.
#ifndef FS_NAV_AUTOMATIC_NBFILE
#  define FS_NAV_AUTOMATIC_NBFILE DISABLE
#endif

//_____ D E C L A R A T I O N S ____________________________________________

typedef enum
{
  NAVAUTO_MOV_EXPLORER_OK,
  NAVAUTO_MOV_EXPLORER_ERROR,
  NAVAUTO_MOV_EXPLORER_RECURSIVE
} navauto_mov_explorer_rec_t;

static uint8_t navauto_mov_explorer_rec(bool b_direction, navauto_mov_options_t options);
static navauto_mov_explorer_rec_t navauto_mov_explorer_select_limit_dir_cur_folder(bool b_direction);
static navauto_mov_explorer_rec_t navauto_mov_explorer_select_limit_file_cur_folder(bool b_direction);
static navauto_mov_explorer_rec_t navauto_mov_explorer_updir(bool b_direction);
static navauto_mov_explorer_rec_t navauto_mov_explorer_limit(bool b_direction);

// This type defines an action list
typedef navauto_mov_explorer_rec_t (*navauto_mov_explorer_process_fct_t) (bool);

//! This table describes what the action list to process one a new folder
//! is accessed.\n
//!
//! - is_dir_process_fct[0] = first action
//! - is_dir_process_fct[1] = second action
//! - ...
//!
static const navauto_mov_explorer_process_fct_t new_dir_process_fct[] = {
#ifdef NAV_AUTO_FILE_IN_FIRST
  navauto_mov_explorer_select_limit_file_cur_folder,
  navauto_mov_explorer_select_limit_dir_cur_folder
#else
  navauto_mov_explorer_select_limit_dir_cur_folder,
  navauto_mov_explorer_select_limit_file_cur_folder
#endif
};
//! This table describes what to do once the navigator reaches a limit while
//! browsing directories.\n
//!
//! - is_dir_process_fct[0] is the function called on a previous command.
//! - is_dir_process_fct[1] is the function called on a next command.
//!
static const navauto_mov_explorer_process_fct_t is_dir_process_fct[2] = {
#ifdef NAV_AUTO_FILE_IN_FIRST
  navauto_mov_explorer_select_limit_file_cur_folder,
  navauto_mov_explorer_updir
#else
  navauto_mov_explorer_updir,
  navauto_mov_explorer_select_limit_file_cur_folder
#endif
};
//! This table describes what to do once the navigator reaches a limit while
//! browsing files.\n
//!
//! - is_file_process_fct[0] is the function called on a previous command.
//! - is_file_process_fct[1] is the function called on a next command.
//!
static const navauto_mov_explorer_process_fct_t is_file_process_fct[2] = {
#ifdef NAV_AUTO_FILE_IN_FIRST
  navauto_mov_explorer_updir,
  navauto_mov_explorer_select_limit_dir_cur_folder
#else
  navauto_mov_explorer_select_limit_dir_cur_folder,
  navauto_mov_explorer_updir
#endif
};
//! This table describes what to do once the navigator reaches the limit of the "playlist".
//!
//! - is_file_process_fct[0] is the function called on a previous command.
//! - is_file_process_fct[1] is the function called on a next command.
//!
static const navauto_mov_explorer_process_fct_t limit_process_fct[2] = {
  navauto_mov_explorer_limit,
  navauto_mov_explorer_limit
};

static Navauto_mov_bitfield_status bitfield_status;

void              navauto_rand_init          ( void );
uint16_t          navauto_rand_choose        ( void );
void              navauto_mov_explorer_reset ( void );
Navauto_mov_state navauto_mov_explorer       ( bool b_direction, navauto_mov_options_t options );
Navauto_mov_state navauto_mov_playlist       ( bool b_direction );


//! This function returns the directory limitation of the file list
//!
//! @return Used navigation mode  (NAVAUTO_MODE_DISKS, NAVAUTO_MODE_DIRONLY, NAVAUTO_MODE_DIRSUB)
//!
//! @verbatim
//! This parameter is ignored then a play list file is selected
//! @endverbatim
//!
Navauto_mode navauto_getmode( void )
{
   return g_navauto_exp_mode;
}


//! This function sets the directory limitation of the file list
//!
//! @param exp_mode    mode to select (NAVAUTO_MODE_DISKS, NAVAUTO_MODE_DIRONLY, NAVAUTO_MODE_DIRSUB)
//!
//! @return true, mode changed
//! @return false, mode can't be changed (note: the selected file must be close)
//!
//! @verbatim
//! This parameter is ignored then a play list file is selected
//! When the mode change, the folder level is reseted at the current position.
//! @endverbatim
//!
bool  navauto_setmode( Navauto_mode exp_mode )
{
   if( g_navauto_exp_mode == exp_mode )
      return true;
   g_navauto_exp_mode = exp_mode;
   if( g_navauto_b_playlist )
      return true;
   return navauto_open( false , 0 );   // Reopen navigation to recompute the file list size
}


//! This function returns the random state used
//!
//! @return random used
//!
Navauto_rand navauto_getrand( void )
{
   return g_navauto_rand;
}


//! This function changes the random state
//!
//! @param rand   random mode to use
//!
void  navauto_setrand( Navauto_rand rand )
{
   g_navauto_rand = rand;
   navauto_rand_init();
   // If current file in rand table
   if( g_navauto_u16_pos <= (NAVAUTO_MAX_RANGE_RAND*8) )
   {
      // Set current position in rand table
      uint8_t u8_pos_rand = g_navauto_u16_pos-1;
      g_navauto_rand_tab[u8_pos_rand/8] |= 1<<(u8_pos_rand%8);
   }
}


//! This function resets the random bit table
//!
void  navauto_rand_init( void )
{
   // Init rand table
   memset( g_navauto_rand_tab , 0 , NAVAUTO_MAX_RANGE_RAND );
   g_navauto_u16_rand_pos = 0;
}


//! This function returns a random position not already used
//!
uint16_t  navauto_rand_choose( void )
{
   uint16_t u16_i;
   uint16_t u16_nb_file_rand;
   uint16_t u16_pos_file_rand;

navauto_rand_choose_restart:
   if( g_navauto_u16_nb == g_navauto_u16_rand_pos )
      return 0;   // All files have been selected, then and of random

   // Compute the range used in the bit table
   if( (g_navauto_u16_nb-g_navauto_u16_rand_pos) < (NAVAUTO_MAX_RANGE_RAND*8) )
      u16_nb_file_rand = g_navauto_u16_nb-g_navauto_u16_rand_pos;
   else
      u16_nb_file_rand = NAVAUTO_MAX_RANGE_RAND*8;

   // Table full ?
   u16_i = 0;
   while( 1 )
   {
      if( u16_nb_file_rand == u16_i )
      {
         // Table full then reset table and go to next part of file list
         memset( g_navauto_rand_tab , 0 , NAVAUTO_MAX_RANGE_RAND );
         g_navauto_u16_rand_pos += u16_nb_file_rand;
         goto navauto_rand_choose_restart;
      }
      if( 0 == (g_navauto_rand_tab[u16_i/8] & (1<<(u16_i%8))) )
         break;   // Not used position found
      u16_i++;
   }

   // Get random value
   NAVAUTO_GET_RAND(u16_i);
   if( u16_i == 0 ) u16_i = 1;

   // Compute the current position in rand table
   u16_pos_file_rand = g_navauto_u16_pos-g_navauto_u16_rand_pos-1;

   // Search not used position
   while( 0 != u16_i )
   {
      u16_pos_file_rand++;
      if( u16_pos_file_rand >= u16_nb_file_rand )
         u16_pos_file_rand = 0;
      if( 0 == (g_navauto_rand_tab[u16_pos_file_rand/8] & (1<<(u16_pos_file_rand%8))) )
         u16_i--;
   }
   // Position not used found
   g_navauto_rand_tab[u16_pos_file_rand/8] |= 1<<(u16_pos_file_rand%8); // Update table
   return (g_navauto_u16_rand_pos+u16_pos_file_rand+1);                // Send position
}


//! This function initializes the file extension filter used to create the file list
//!
//! @param sz_filterext    file extension filter
//!
//! @verbatim
//! This parameter is ignored then a play list file is selected
//! @endverbatim
//!
void   navauto_init( const FS_STRING sz_filterext )
{
   g_navauto_filter = sz_filterext;
   g_navauto_u16_dir_level_root = 0;
   g_navauto_u16_nb = 0;
}


//! This function opens a file list at the current position in navigator
//!
//! @param b_playlist   if true then the current selected file is a play list file to open
//!                     else create a file list with files included in a disk part
//! @param pos          If b_playlist true, then position in the play list to start
//!                     else folder level of the current position
//!
//! @return             false, in case of error or file list empty
//!
bool  navauto_open( bool b_playlist , uint16_t pos )
{
   Navauto_mov_state state;

   g_navauto_b_playlist     = b_playlist;
   g_navauto_u16_dir_level_root = 0;
   g_navauto_u16_nb = 0;

   if( b_playlist )
   {
      g_navauto_u16_pos        = pos;
      if( !pl_main_open(false) )
         return false;
      g_navauto_u16_nb = pl_nav_getnbfile();
      if( 0 == g_navauto_u16_nb )
         return false;
      if( NAVAUTO_RAND_OFF == navauto_getrand() )
      {
         if( pl_nav_setpos( g_navauto_u16_pos ))
         {
            if( nav_filelist_validpos() )
               return true;
         }
      }else{
         navauto_rand_init();
      }
      // Error position then restart at the beginning
      g_navauto_u16_pos = 0;
   }
   else
   {
#if( FS_NAV_AUTOMATIC_NBFILE == ENABLE )
      Fs_index       index;
      Fs_index       index2;
      Navauto_rand   rand_mode;
      uint16_t            u16_current_pos=0;

      index.u8_lun = 0xFF;    // Reset index (by default no valid selected file)
      g_navauto_u16_dir_level  = pos;

      rand_mode = navauto_getrand();               // Save random mode
      navauto_setrand( NAVAUTO_RAND_OFF );

      // If no valid file then found the first valid file
      if( !nav_file_checkext( g_navauto_filter ))
      {
         // Go to previous position because "navauto_mov_explorer()" execute a next before search valid file
         if( !nav_filelist_set( 0 , FS_FIND_PREV ) )
           nav_filelist_reset();
         state = navauto_mov_explorer(FS_FIND_NEXT, NAVAUTO_MOV_OPTS_NONE);
         if((NAVAUTO_MOV_OK_LOOP != state.status)
         && (NAVAUTO_MOV_OK      != state.status) )
         {
           navauto_setrand(rand_mode);
           return false;  // List empty
         }
      }
      index = nav_getindex();

      // Compute the size of file list and search the position of selected file
      navauto_mov_explorer_reset();                // Go to beginning of loop
      // Note: the number of file is updated in navauto_mov_explorer() routine when the end of list is detected

      while( navauto_mov_explorer( FS_FIND_NEXT, NAVAUTO_MOV_OPTS_NONE ).status == NAVAUTO_MOV_OK )
      {
         index2 = nav_getindex();
         // Check the current position with the selected file
         if( (index.u8_lun == index2.u8_lun)
         &&  (index.u32_cluster_sel_dir == index2.u32_cluster_sel_dir)
         &&  (index.u16_entry_pos_sel_file == index2.u16_entry_pos_sel_file) )
         {
            u16_current_pos = g_navauto_u16_pos;   // Save the position number found
            g_navauto_u16_dir_level_root = g_navauto_u16_dir_level;
         }
      }

      navauto_setrand(rand_mode);                  // Restore random mode
      if( 0 == g_navauto_u16_nb )
         return false;  // loop empty

      // Go to a file from file list
      if( NAVAUTO_RAND_OFF == navauto_getrand() )
      {
         if( 0xFF != index.u8_lun )
         {
            // Reselect the file selected at startup
            nav_gotoindex( &index );               // Go to this one
            g_navauto_u16_dir_level  = g_navauto_u16_dir_level_root;        // Update position file
            g_navauto_u16_pos = u16_current_pos;   // Update folder level corresponding at file
         }
         // Else, the first file is already selected at the end of "compute file list size" loop
         return true;
      }else{
         navauto_rand_init();
      }
#else
      Fs_index       index;
      Navauto_rand   rand_mode;

      rand_mode = navauto_getrand();
      navauto_setrand(NAVAUTO_RAND_OFF);

      // If no valid file then find the first valid file
      if( !nav_file_checkext( g_navauto_filter ))
      {
         // Go to previous position because "navauto_mov_explorer()" execute a next before search valid file
         if( !nav_filelist_set( 0 , FS_FIND_PREV ) )
           nav_filelist_reset();
         state = navauto_mov_explorer(FS_FIND_NEXT, NAVAUTO_MOV_OPTS_NONE);
         if((NAVAUTO_MOV_OK_LOOP != state.status)
         && (NAVAUTO_MOV_OK      != state.status) )
         {
           navauto_setrand(rand_mode);
           return false;  // List empty
         }
      }
      index = nav_getindex();

      navauto_setrand(rand_mode);

      navauto_mov_explorer_reset();
      // Compute directory level
      g_navauto_u16_dir_level = 0;
      while(nav_dir_gotoparent())
        g_navauto_u16_dir_level++;
      g_navauto_u16_dir_level_root = g_navauto_u16_dir_level;
      //g_navauto_u16_dir_level  = pos;
      // Restore index
      nav_gotoindex(&index);

      g_navauto_u16_nb = 0xFFFF;

      if( NAVAUTO_RAND_OFF == navauto_getrand() )
        navauto_rand_init();

      if( nav_file_checkext( g_navauto_filter ))
      {
        // Valid file then update position with the first position but it is not the first !
        g_navauto_u16_pos = 1;
        return true;
      }
#endif
   }
   // Find first file or use the random feature
   state = navauto_mov(FS_FIND_NEXT, NAVAUTO_MOV_OPTS_NONE);
   if((NAVAUTO_MOV_OK_LOOP != state.status)
   && (NAVAUTO_MOV_OK      != state.status) )
      return false;  // List empty
   return true;
}


//! This function goes to the beginning of file list
//!
//! @verbatim
//! This routine is not authorized then a play list file is opened
//! @endverbatim
//!
void  navauto_mov_explorer_reset( void )
{
   g_navauto_u16_nb = 0;

   switch( g_navauto_exp_mode )
   {
      case NAVAUTO_MODE_DISKS :  // Go to the root of the first disk
      g_navauto_u16_dir_level = 0;
      g_navauto_u16_dir_level_root = 0;
      nav_drive_set(0);
      nav_dir_root();            // Ignore error, because this one is managed by routine next file
      break;

      case NAVAUTO_MODE_DISK :   // Go to the root of the current disk
      g_navauto_u16_dir_level = 0;
      g_navauto_u16_dir_level_root = 0;
      nav_dir_root();            // Ignore error, because this one is managed by routine next file
      break;

      default:
      g_navauto_u16_dir_level = 0;
      g_navauto_u16_dir_level_root = 0;
      nav_dir_root();
  //    nav_filelist_reset();      // Reset directory = no selected file
      break;
   }
   // Here the navigator is at the beginning of file list
   // and no file is selected
   g_navauto_u16_pos = (uint16_t) -1;
}


//! This function closes the file list
//!
//! @return    If a play list file is open then it is the last position in the file list
//!            else it is the directory level corresponding at the last selected file
//!
//! @verbatim
//! At the end of this routine, the selected file of current navigator is :
//! - the play list file (in case of a play list file has been opened)
//! - the last selected file (in other case)
//! @endverbatim
//!
uint16_t  navauto_close( void )
{
   if( g_navauto_b_playlist )
   {
      pl_main_close();
      return g_navauto_u16_pos;
   }
   return g_navauto_u16_dir_level;
}


//! This function returns the file list size
//!
uint16_t   navauto_getnb( void )
{
   return g_navauto_u16_nb;
}

//! This function returns the current position in the file list
//!
uint16_t   navauto_getpos( void )
{
   return g_navauto_u16_pos;
}


//! This function selects a new position in file list
//!
//! @param u16_pos   new position to select
//!
//! @return    false in case of error
//!
bool  navauto_setpos( uint16_t u16_pos )
{
   bool b_direction;
   Navauto_rand rand_mode;
   Navauto_mov_state state;

   rand_mode = navauto_getrand();               // Save random mode
   state.status = NAVAUTO_MOV_OK;

   while( (u16_pos != g_navauto_u16_pos ) )
   {
     b_direction = ( u16_pos > g_navauto_u16_pos )? FS_FIND_NEXT : FS_FIND_PREV;
     state = navauto_mov( b_direction, NAVAUTO_MOV_OPTS_NONE );
   }

   navauto_setrand(rand_mode);                  // Restore random mode

   return (NAVAUTO_MOV_OK == state.status);
}


//! This function jumps to the next or previous file in file list
//!
//! @param b_direction  jump direction (FS_FIND_NEXT or FS_FIND_PREV)
//! @param options  jump direction (FS_FIND_NEXT or FS_FIND_PREV)
//!
//! @return    the status of the action
//!
//! @verbatim
//! When the random is ON, the direction is ignored
//! @endverbatim
//!
Navauto_mov_state   navauto_mov( bool b_direction, navauto_mov_options_t options )
{
   if( g_navauto_b_playlist )
      return navauto_mov_playlist( b_direction );
   else
      return navauto_mov_explorer( b_direction, options );
}




//! This function sets the pointer to a limit.
//!
//! @param b_direction  direction of navigation (FS_FIND_NEXT or FS_FIND_PREV)
//!
//! @return     true if a file is selected
//!             false otherwise
//!
static navauto_mov_explorer_rec_t navauto_mov_explorer_limit(bool b_direction)
{
  // Reset the selection
  nav_filelist_reset();
  // Goes to the root directory
  while(navauto_mov_explorer_updir(b_direction) != NAVAUTO_MOV_EXPLORER_ERROR);
  // Update directory level
  g_navauto_u16_dir_level = g_navauto_u16_dir_level_root;
  // Call back the recursive function
  return NAVAUTO_MOV_EXPLORER_RECURSIVE;
}
//! This function selects the first or the last playable file on the current folder
//!
//! @param b_direction  direction of navigation (FS_FIND_NEXT or FS_FIND_PREV)
//!
//! @return     true if a file is selected
//!             false otherwise
//!
static navauto_mov_explorer_rec_t navauto_mov_explorer_select_limit_file_cur_folder(bool b_direction)
{
  if ((b_direction == FS_FIND_NEXT)?nav_filelist_first(FS_FILE):nav_filelist_last(FS_FILE))
  {
    // Look for the 1st or the last playable file in the current directory
    // Find the 1st or the last valid file
    while(!nav_file_checkext(g_navauto_filter))
    {
      if (!nav_filelist_set(0, b_direction))
        return NAVAUTO_MOV_EXPLORER_ERROR;
    }
    return NAVAUTO_MOV_EXPLORER_OK;
  }
  else
    return NAVAUTO_MOV_EXPLORER_ERROR;
}

//! This function selects the first or the last directory on the current folder
//! and enters in it
//!
//! @param b_direction  direction of navigation (FS_FIND_NEXT or FS_FIND_PREV)
//!
//! @return     The state of the function
//!
static navauto_mov_explorer_rec_t navauto_mov_explorer_select_limit_dir_cur_folder(bool b_direction)
{
  if (!((b_direction == FS_FIND_NEXT)?nav_filelist_first(FS_DIR):nav_filelist_last(FS_DIR)))
    return NAVAUTO_MOV_EXPLORER_ERROR;

  // Update directory level
  g_navauto_u16_dir_level++;
  // Enter in this new directory
  nav_dir_cd();

  // Set folder change bit
  bitfield_status.folder_change = 1;

  return NAVAUTO_MOV_EXPLORER_RECURSIVE;
}

//! This function moves back to the parent directory and selects this directory
//!
//! @param b_direction  direction of navigation (FS_FIND_NEXT or FS_FIND_PREV)
//!
//! @return     The state of the function
//!
static navauto_mov_explorer_rec_t navauto_mov_explorer_updir(bool b_direction)
{
  // To support directory only mode
  if (g_navauto_exp_mode == NAVAUTO_MODE_DIRONLY)
    return NAVAUTO_MOV_EXPLORER_ERROR;

  // To Support subdirectories mode
  if (g_navauto_exp_mode == NAVAUTO_MODE_DIRSUB)
  {
    // If the selection is on the root level
    if (g_navauto_u16_dir_level <= g_navauto_u16_dir_level_root)
      return NAVAUTO_MOV_EXPLORER_ERROR;
  }

  if (nav_dir_gotoparent())
  {
    // Set folder change bit
    bitfield_status.folder_change = 1;

    g_navauto_u16_dir_level--;
    return NAVAUTO_MOV_EXPLORER_RECURSIVE;
  }
  else // If it fails, means the pointer is on the root
    return NAVAUTO_MOV_EXPLORER_ERROR;
}

static __inline__ uint8_t navauto_mov_ok_loop(bool b_direction, navauto_mov_options_t options)
{
  bool b_ok_loop = false;

  // If mode Disks, it should go to the next/previous disk
  if (g_navauto_exp_mode == NAVAUTO_MODE_DISKS)
  {
    // Mount the next/previous disk
    do
    {
      if (b_direction == FS_FIND_NEXT)
      {
        // Go to the next drive
        if (!nav_drive_set(nav_drive_get() + 1))
        {
          nav_drive_set(0);
          b_ok_loop = true;
        }
      }
      else // direction PREVIOUS
      {
        // Go to the previous drive
        if (!nav_drive_set(nav_drive_get() - 1))
        {
          nav_drive_set(nav_drive_nb() - 1);
          b_ok_loop = true;
        }
      }
    }while(!nav_partition_mount());

    // If it reached a limit
    if (b_ok_loop)
    {
      navauto_mov_explorer_rec(b_direction, options);
      return NAVAUTO_MOV_OK_LOOP;
    }

    // Else return the result of the recursive function
    return navauto_mov_explorer_rec(b_direction, options);
  }

  return NAVAUTO_MOV_OK_LOOP;
}

static __inline__ uint8_t navauto_mov_explorer_new_dir(bool b_direction, navauto_mov_options_t options)
{
  int i;
  // Process
  i = (b_direction == FS_FIND_NEXT)?0:(sizeof(new_dir_process_fct)/sizeof(navauto_mov_explorer_process_fct_t)-1);
  while(i >= 0 && i < sizeof(new_dir_process_fct)/sizeof(navauto_mov_explorer_process_fct_t))
  {
    switch(new_dir_process_fct[i](b_direction))
    {
    case NAVAUTO_MOV_EXPLORER_RECURSIVE:
      return navauto_mov_explorer_rec(b_direction, options);
    case NAVAUTO_MOV_EXPLORER_OK:
      return NAVAUTO_MOV_OK;
    default:
      break;
    }
    // Index
    i += (b_direction == FS_FIND_NEXT)?(1):(-1);
  }

  // Else means directory empty!
  // Needs to up dir
  if (navauto_mov_explorer_updir(b_direction) == NAVAUTO_MOV_EXPLORER_RECURSIVE)
    return navauto_mov_explorer_rec(b_direction, options);

  // If it fails, means the pointer is on the root
  return NAVAUTO_MOV_EMPTY;
}

static __inline__ uint8_t navauto_mov_explorer_is_dir(bool b_direction, navauto_mov_options_t options)
{
  bool status;

  // Look for the next/previous directory in the current directory
  if ((status = nav_filelist_set(0, b_direction)))
    status = nav_file_isdir();

  // If status == false, it means it reached a limit
  if (!status)
  {
    switch(is_dir_process_fct[(b_direction == FS_FIND_NEXT)?1:0](b_direction))
    {
    case NAVAUTO_MOV_EXPLORER_RECURSIVE:
      return navauto_mov_explorer_rec(b_direction, options);
    case NAVAUTO_MOV_EXPLORER_OK:
      return NAVAUTO_MOV_OK;
    default:
      break;
    }
  }
  else
  {
    g_navauto_u16_dir_level++;
    // Enter in this new directory
    nav_dir_cd();
    // Set folder change bit
    bitfield_status.folder_change = 1;

    return navauto_mov_explorer_rec(b_direction, options);
  }

  // At this point, there is no directory available and the limit function
  // returns ERROR
  if (navauto_mov_explorer_updir(b_direction) == NAVAUTO_MOV_EXPLORER_RECURSIVE)
    return navauto_mov_explorer_rec(b_direction, options);

  return navauto_mov_ok_loop(b_direction, options);
}

static __inline__ uint8_t navauto_mov_explorer_is_file(bool b_direction, navauto_mov_options_t options)
{
  // Look for the next playable file in the current directory
  while(nav_filelist_set(0, b_direction))
  {
    if (nav_file_isdir())
      break;
    if (!nav_file_checkext(g_navauto_filter))
      continue;
    return NAVAUTO_MOV_OK;
  }

  // In case it needs to stay in the current directory,
  if ( (options & NAVAUTO_MOV_OPTS_STAY_CURRENT_DIR)
    || (g_navauto_exp_mode == NAVAUTO_MODE_DIRONLY))
  {
    // Update g_navauto_u16_pos
    while(nav_filelist_set(0, !b_direction))
    {
      if (nav_file_isdir())
        break;
      if (!nav_file_checkext(g_navauto_filter))
        continue;
      g_navauto_u16_pos += (!b_direction == FS_FIND_NEXT)?(1):(-1);
    }
    // Go to the first/last file
    if (navauto_mov_explorer_select_limit_file_cur_folder(b_direction) == NAVAUTO_MOV_EXPLORER_OK)
    {
      if (options & NAVAUTO_MOV_OPTS_STAY_CURRENT_DIR)
        return NAVAUTO_MOV_OK;
      else
        return NAVAUTO_MOV_OK_LOOP;
    }
    return NAVAUTO_MOV_EMPTY;
  }

  // At this point, it means it reached a limit, there is no more playable files
  switch(is_file_process_fct[(b_direction == FS_FIND_NEXT)?1:0](b_direction))
  {
  case NAVAUTO_MOV_EXPLORER_RECURSIVE:
    return navauto_mov_explorer_rec(b_direction, options);
  case NAVAUTO_MOV_EXPLORER_OK:
    return NAVAUTO_MOV_OK;
  default:
    break;
  }

  // At this point, there is no directory available and the limit function
  // returns ERROR
  if (navauto_mov_explorer_updir(b_direction) == NAVAUTO_MOV_EXPLORER_RECURSIVE)
    return navauto_mov_explorer_rec(b_direction, options);

  return navauto_mov_ok_loop(b_direction, options);
}



//! This function selects a file in EXPLORER mode
//!
//! @param b_direction  direction of navigation (FS_FIND_NEXT or FS_FIND_PREV)
//! @param options  define extra modes for this function
//!
//! @return    the status of the action
//!
static uint8_t navauto_mov_explorer_rec( bool b_direction, navauto_mov_options_t options )
{
  // If no file is selected - new directory
  if (!nav_filelist_validpos())
    return navauto_mov_explorer_new_dir(b_direction, options);

  // If the selection is on a directory
  if (nav_file_isdir())
    return navauto_mov_explorer_is_dir(b_direction, options);
  else // The selection is set on a file
    return navauto_mov_explorer_is_file(b_direction, options);
}

//! This function jumps to the next or previous file in file list included in directories
//!
//! @param b_direction  jump direction (FS_FIND_NEXT or FS_FIND_PREV)
//! @param options  define extra modes for this function
//!
//! @return    the status of the action
//!
//! @verbatim
//! This routine is not authorized then a play list file is opened
//! When the random is ON, the direction is ignored
//! @endverbatim
//!
Navauto_mov_state   navauto_mov_explorer( bool b_direction, navauto_mov_options_t options )
{
  Navauto_mov_state state;
  uint16_t u16_mov_pos = 0;
  size_t nb_loops = 1;

  // Reset the bitfield status
  memset(&bitfield_status, 0, sizeof(bitfield_status));

  // Random implementation
  if (g_navauto_rand == NAVAUTO_RAND_ON)
  {
    u16_mov_pos = navauto_rand_choose();
    if (!u16_mov_pos)
    {
      navauto_rand_init();
      // navauto_mov_explorer_reset();
      u16_mov_pos = navauto_rand_choose();
      if(!u16_mov_pos)
      {
        state.bitfield.all = bitfield_status.all;
        state.status = NAVAUTO_MOV_EMPTY;
        return state;  // Error system
      }
    }
    // To avoid end of loop (NAVAUTO_MOV_OK_LOOP state)
    b_direction = (u16_mov_pos > g_navauto_u16_pos)?FS_FIND_NEXT:FS_FIND_PREV;
    nb_loops = (u16_mov_pos > g_navauto_u16_pos)?(u16_mov_pos-g_navauto_u16_pos):(g_navauto_u16_pos-u16_mov_pos);

    // Approximation used for the "stay in current directory" bit,
    // it reduces the number of loops in large file systems.
    if ( (options & NAVAUTO_MOV_OPTS_STAY_CURRENT_DIR)
      || (g_navauto_exp_mode == NAVAUTO_MODE_DIRONLY))
      nb_loops %= nav_filelist_nb(FS_FILE);

    // Set u16_mov_pos to the current position to compare it with current and to make sure the
    // new file selected is not the same as the last one.
    u16_mov_pos = g_navauto_u16_pos;
  }

  // This loop is done, so that the random method is not a problem once a NAVAUTO_MOV_OPTS_STAY_CURRENT_DIR option is set to 1.
  while(nb_loops--)
  {
    // Compute the recursive function
    state.status = navauto_mov_explorer_rec(b_direction, options);
    // Check its returning state
    switch(state.status)
    {

    // If the function is at the end of the loop
    case NAVAUTO_MOV_OK_LOOP:

      // Update g_navauto_u16_pos counter
      if (b_direction == FS_FIND_NEXT)
      {
        g_navauto_u16_nb = g_navauto_u16_pos + 1;
        g_navauto_u16_pos = 0;
      }
      else
        g_navauto_u16_pos = g_navauto_u16_nb - 1;

      // Means it reached the tail or the beginning of the playlist.
      // Then set the pointer to a limit of the "playlist"
      // should be at the beginning for a NEXT and at end for a PREVIOUS command.
      switch(limit_process_fct[(b_direction == FS_FIND_NEXT)?1:0](b_direction))
      {

      case NAVAUTO_MOV_EXPLORER_RECURSIVE:
        // If it returns NAVAUTO_MOV_OK_LOOP, it means it reached two times a row the end of the playlist,
        // therefore, this is an empty disk!
        if (navauto_mov_explorer_rec(b_direction, options) == NAVAUTO_MOV_OK_LOOP)
        {
          state.bitfield.all = bitfield_status.all;
          state.status = NAVAUTO_MOV_EMPTY;
          return state;
        }
        break;

      default:
        break;
      }

      state.bitfield.all = bitfield_status.all;
      state.status = NAVAUTO_MOV_OK_LOOP;
      return state;

    // If a file is correctly selected
    case NAVAUTO_MOV_OK:
      // Update g_navauto_u16_pos counter
      g_navauto_u16_pos += (b_direction == FS_FIND_NEXT)?(1):(-1);
      // if the selection is the same as last one, then get next file
      if (g_navauto_rand == NAVAUTO_RAND_ON && u16_mov_pos == g_navauto_u16_pos && !nb_loops)
      {
        state.status = navauto_mov_explorer_rec(b_direction, options);
        state.bitfield.all = bitfield_status.all;
        return state;
      }
      break;

    default:
      state.bitfield.all = bitfield_status.all;
      return state;
    }
  }

  state.status = NAVAUTO_MOV_OK;
  state.bitfield.all = bitfield_status.all;
  return state;
}

//! This function jumps to the next or previous file in file list corresponding at a play list file
//!
//! @param b_direction  jump direction (FS_FIND_NEXT or FS_FIND_PREV)
//!
//! @return    The status of the action
//!
//! @verbatim
//! This routine is not authorized then a play list file is not opened
//! @endverbatim
//!
Navauto_mov_state   navauto_mov_playlist( bool b_direction )
{
   bool b_endof_loop;
   b_endof_loop = false;
   Navauto_mov_state state;

   state.bitfield.all = 0;
   while( 1 )
   {
      if( NAVAUTO_RAND_ON == g_navauto_rand )
      {
         g_navauto_u16_pos = navauto_rand_choose();
         if( 0 == g_navauto_u16_pos )
         {
            // Loop finish, then reset random and choose other value
            if( b_endof_loop )
            {
               state.status = NAVAUTO_MOV_DISKERROR;
               return state;
            }
            b_endof_loop = true;
            navauto_rand_init();
            g_navauto_u16_pos = navauto_rand_choose();
         }
         pl_nav_setpos( g_navauto_u16_pos );
      }
      else
      {
         g_navauto_u16_pos += (FS_FIND_NEXT==b_direction)? 1 : -1;
         if( !pl_nav_setpos( g_navauto_u16_pos ) )
         {
            // Loop finish or error
            if( b_endof_loop )
            {
               state.status = NAVAUTO_MOV_DISKERROR;
               return state;
            }
            b_endof_loop = true;
            if( FS_FIND_NEXT == b_direction )
            {
               g_navauto_u16_pos = 1-1;                  // end of list then go to the beginning
            }else{
               g_navauto_u16_pos = pl_nav_getnbfile()+1; // beginning of list then go to the end
            }
         }
      }
      if( nav_filelist_validpos() )
         break;
      // Here it is a DEAD path or to large for system memory
   }
   g_navauto_u16_pos = pl_nav_getpos();
   if( b_endof_loop )
   {
      state.status = NAVAUTO_MOV_OK_LOOP;
      return state;
   }
   state.status = NAVAUTO_MOV_OK;
   return state;
}



