/**********************************************************************
 *
 * 	Utility code that parses a raw hex record into binary format.
 *
 *  Basically, pass in one raw record at a time, check the type.
 *  If it's data, write data to flash at hexRecord.address + addressOffset
 *  If it's EndOfFile, stop.
 *  Ignore everything else.
 *
 *****************************************************************************/
#ifndef __PARSE_HEX_H__
#define __PARSE_HEX_H__

// Record Types
#define HEX_DATA				0
#define HEX_EOF					1
#define HEX_EXT_SEG_ADDRESS			2
#define HEX_START_SEG_ADDRESS			3
#define HEX_EXT_LINEAR_ADDRESS			4
#define HEX_START_LINEAR_ADDRESS		5

// Structure to hold parsed data from one record of the hex file.
typedef struct {
  U8	count;
  U8	type;
  U8	checksum;
  U32	address;
  U8	data[256];
} hexRecord_t;

int parse_raw_hex_record( U8 *rawData, hexRecord_t *hexRecord );

#endif //__PARSE_HEX_H__
