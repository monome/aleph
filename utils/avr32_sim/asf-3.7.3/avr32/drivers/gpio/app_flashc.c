#include "flashc.h"
extern volatile void *app_flashc_memcpy(volatile void *dst, const void *src, size_t nbytes, bool erase) {
    flashc_memcpy(dst, src, nbytes, erase);
}

