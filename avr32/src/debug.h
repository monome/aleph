#if RELEASEBUILD==1

#define  print_dbg(blahblah) ;;
#define  print_dbg_char(blahblah) ;;
#define  print_dbg_ulong(blahblah) ;;
#define  print_dbg_char_hex(blahblah) ;;
#define  print_dbg_short_hex(blahblah) ;;
#define  print_dbg_hex(blahblah) ;;

#else

#include "print_funcs.h"

#endif
