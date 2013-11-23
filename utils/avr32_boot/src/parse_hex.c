/*
hex file format:

record format:

1. start code, ":"
2. byte count    : 2 hex digits	= no. of bytes in data field
3. address       : 4 digits    ( big endian )
4. record type	 : 2 digits    
5. DATA:  	 : 2*N digits 
6. checksum	 : 2 digits	= 0x100 - ( sum(data) & 0xff )


record type:

00 : data record: 16b address + data
01 : EOF  ( usually: ':00000001FF' )
02 : extended segment address record	: add (data<<4) to subsequent 00 addresses
03 : start segment address record : initial state of CS:IP registers (80x86), addr==0
04 : extended linear address record : upper word of 32-bit addr
05 : start linear address record : data (4 bytes) loaded to EIP register (80386+), addr==0

NOTE:
there are 3 types of Intel HEX based on data size: 8b, 16b, 32b.
data may be byte-swapped in 16b or 32b.

NOTE:
correct checksum means that (sum of all bytes in record) | 0xff == 0
*/


#include <compiler.h>
// #include "print_funcs.h"
#include "parse_hex.h"

/*
 *  This walks over the record and ensures that the checksum is
 *  correct for the record.
 *
 *  returns 0 if checksum validates, anything else on error
 */
static int validate_checksum( hexRecord_t *hexRecord )
{
  int i = 0;
  int checksum = 0;

  checksum = hexRecord->count + hexRecord->type + hexRecord->checksum +
    (hexRecord->address >> 8) + (0xff & hexRecord->address);

  for( i = 0; i < hexRecord->count; i++ )
    {
      checksum += hexRecord->data[i];
    }

  return (0xff & checksum);
}


static int validate_record( hexRecord_t *hexRecord )
{
  /* Validate the checksum */
  if( 0 != validate_checksum( hexRecord ) ) {
    return -1;
  }

  /* Validate the type */
  switch( hexRecord->type ) {
    /* Intel 1 format, for up to 64K length (types 0, 1) */
  case 0:                             /* data record */
    /* Nothing to do. */
    break;

  case 1:                             /* EOF record */
    if( 0 != hexRecord->count )
      return -2;
    break;

    /* Intel 2 format, when 20 bit addresses are needed (types 2, 3, 4) */
  case 2:                             /* extended address record */
    if(    (0 != hexRecord->address)
	   || (2 != hexRecord->count)
	   || (hexRecord->data[1] != (0xf8 & hexRecord->data[1])) )
      {
	return -3;
      }
    break;

  case 3:                             /* start address record */
    /* just ignore these records (could verify addr == 0) */
    return -8;

  case 4:                             /* extended linear address record */
    if( (0 != hexRecord->address) || (2 != hexRecord->count) )
      return -4;
    break;

  case 5:                             /* start linear address record */
    if( (0 != hexRecord->address) || (4 != hexRecord->count) ) {
      return -6;
    }
    break;

  default:
    /* Type 5 and other types are unsupported. */
    return -5;
  }

  return 0;
}


static void process_address( hexRecord_t *hexRecord )
{
  switch( hexRecord->type ) {
  case 2:
    /* 0x1238 -> 0x00012380 */
    hexRecord->address = (hexRecord->data[0] << 8) | hexRecord->data[1];
    hexRecord->address *= 16;
    break;

  case 4:
    /* 0x1234 -> 0x12340000 */
    hexRecord->address = (hexRecord->data[0] << 8) | hexRecord->data[1];
    hexRecord->address <<= 16;
    break;

  case 5:
    /* 0x12345678 -> 0x12345678 */
    hexRecord->address = ((0xff & hexRecord->data[0]) << 24) |
      ((0xff & hexRecord->data[1]) << 16) |
      ((0xff & hexRecord->data[2]) <<  8) |
      (0xff & hexRecord->data[3]);
    break;
  }
}


/***************************************************************************
 * scanf() takes HUGE amounts of memory, so this is a simple function
 * to convert two hex digits to an 8b binary value.
 */
static U8 hex_digit_to_int( char digit )
{
  U8 retVal = 0xFF;

  // smaller size but same speed:
  retVal = digit - 48;
  if(retVal > 9) retVal = retVal - 7;
  if(retVal > 15) retVal = retVal - 32;

  // switch (digit)
  //   {
  //   case '0':
  //     retVal = 0;
  //     break;
  //   case '1':
  //     retVal = 1;
  //     break;
  //   case '2':
  //     retVal = 2;
  //     break;
  //   case '3':
  //     retVal = 3;
  //     break;
  //   case '4':
  //     retVal = 4;
  //     break;
  //   case '5':
  //     retVal = 5;
  //     break;
  //   case '6':
  //     retVal = 6;
  //     break;
  //   case '7':
  //     retVal = 7;
  //     break;
  //   case '8':
  //     retVal = 8;
  //     break;
  //   case '9':
  //     retVal = 9;
  //     break;
  //   case 'a':
  //   case 'A':
  //     retVal = 10;
  //     break;
  //   case 'b':
  //   case 'B':
  //     retVal = 11;
  //     break;
  //   case 'c':
  //   case 'C':
  //     retVal = 12;
  //     break;
  //   case 'd':
  //   case 'D':
  //     retVal = 13;
  //     break;
  //   case 'e':
  //   case 'E':
  //     retVal = 14;
  //     break;
  //   case 'f':
  //   case 'F':
  //     retVal = 15;
  //     break;
  //   }

  return retVal;
}

static U8 hex_digits_to_int( char digitH, char digitL );
static U8 hex_digits_to_int( char digitH, char digitL ) {
  U8 retVal;

  retVal = hex_digit_to_int( digitH );
  retVal <<= 4;

  retVal += hex_digit_to_int( digitL );

  return retVal;
}

/***************************************************************************
 * Takes a pointer to raw hex record, parses it into binary data and returns.
 */
static void parse_record( U8 *rawHexData, hexRecord_t *hexRecord )
{
  U8	i;
  U8	buffIndex = 0;
  U8	addr_upper = 0;
  U8	addr_lower = 0;

  // parse the start of the record ':bbaaaarr'
  //   bb - byte count
  // aaaa - the address in memory
  //   rr - record type

  // skip the ':'
  buffIndex++;

  hexRecord->count = hex_digits_to_int( rawHexData[buffIndex], rawHexData[buffIndex+1] );
  buffIndex += 2;

  addr_upper = hex_digits_to_int( rawHexData[buffIndex], rawHexData[buffIndex+1] );
  buffIndex += 2;

  addr_lower = hex_digits_to_int( rawHexData[buffIndex], rawHexData[buffIndex+1] );
  buffIndex += 2;

  hexRecord->type = hex_digits_to_int( rawHexData[buffIndex], rawHexData[buffIndex+1] );
  buffIndex += 2;

  hexRecord->address = addr_upper << 8 | addr_lower;

  /* Read the data */
  for( i = 0; i < hexRecord->count; i++ )
    {
      hexRecord->data[i] = hex_digits_to_int( rawHexData[buffIndex], rawHexData[buffIndex+1] );
      buffIndex += 2;
    }

  /* Read the checksum */
  hexRecord->checksum = hex_digits_to_int( rawHexData[buffIndex], rawHexData[buffIndex+1] );
  buffIndex += 2;
}


/***************************************************************************
 * Takes a pointer to raw hex record, parses it into binary data and returns.
 */
int parse_raw_hex_record( U8 *rawHexData, hexRecord_t *hexRecord )
{
  int retVal = 0;

  // Scan through the raw hex record and convert it into binary format.
  parse_record( rawHexData, hexRecord );

  // Check the binary record and make sure it looks sane.
  retVal = validate_record( hexRecord );
  switch( retVal )
    {
    case 0:     /* data, extended address, etc */
      process_address( hexRecord );
      break;

    case -8:    /* start address (ignore) */
      break;

    default:
      goto error;
    }
  
  return 0;

 error:
  return -1;
}
