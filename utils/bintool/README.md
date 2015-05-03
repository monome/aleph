bintool

a very basic utility for including arbitrary binary data as a static array in a C program.

compile it:
gcc bintool.c -o bintool

run it:
./bintool ~/aleph/modules/mix/aleph-mix.ldr

this should result in two files:

~/aleph/modules/mix/aleph-mix.ldr.inc
- a comma-separated list of byte values, suitable for inclusion in a C array.

~/aleph/modules/mix/aleph-mix.ldr_size.inc
- a single value, the number of bytes in the data.


use the results in a C program something like this:

```
static const unsigned long int ldrSize = 
#include "aleph-mix.ldr_size.inc"
;

static const unsigned char ldrData[] = { 
#include "aleph-mix.ldr.inc"
};
```
