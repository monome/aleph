
// ASF
#include "flashc.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
// aleph-bees
#include "scene.h"
#include "preset.h"
// aleph-avr32
#include "flash.h"
#include "types.h"

/// 64K of blackfin executable storage
#define LDR_FLASH_BYTES 0x10000

// storage layout of default data in nonvolatile memory
typedef const struct {
  u8 ldr[LDR_FLASH_BYTES];
  sceneData_t sceneData;
  sceneDesc_t sceneDesc;
} nvram_data_t;

//! NVRAM data structure located in the flash array.
__attribute__((__section__(".flash_nvram")))
static nvram_data_t flash_nvram_data;

/* void flash_init() { */
/*   //... nothing to do */
/* } */

// read default scene data to pointer
void flash_read_scene(sceneData_t* sceneData, sceneDesc_t* sceneDesc) {
  //  flashc_memcpy( (void*)(&(flash_nvram_data.sceneData)), sceneData, sizeof(sceneData_t), true));
  *sceneData = flash_nvram_data.sceneData;
  *sceneDesc = flash_nvram_data.sceneDesc;
}

// write default scene data from pointer
void flash_write_scene(sceneData_t* sceneData, sceneDesc_t* sceneDesc) {
  flashc_memcpy( (void*)(&(flash_nvram_data.sceneData)), sceneData, sizeof(sceneData_t), true);
  flashc_memcpy( (void*)(&(flash_nvram_data.sceneDesc)), sceneDesc, sizeof(sceneDesc_t), true);
}


void flash_read_ldr_byte(u8* byte, u32 idx) {
  *byte = flash_nvram_data.ldr[idx];
}

void flash_write_ldr_byte(u8* byte, u32 idx) {
  flashc_memset((void*)&(flash_nvram_data.ldr[idx]), *byte, 8, 1, true);
}


// user page
/* __attribute__((__section__(".userpage"))) */
/* static nvram_data_t user_nvram_data; */


//static const uint32_t write = 0x76543210;
/* static const uint8_t writeData[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}; */

/* //=================================== */


/* /\*! \brief Prints the variables stored in NVRAM. */
/*  * */
/*  * \param nvram_data  Pointer to the NVRAM data structure to print. */
/*  *\/ */
/* /\* static void print_nvram_variables(nvram_data_t *nvram_data) *\/ */
/* /\* { *\/ */
/* /\*   print_dbg("var8:\t0x"); *\/ */
/* /\*   print_dbg_char_hex(nvram_data->var8); *\/ */

/* /\*   print_dbg("\r\nvar16:\t0x"); *\/ */
/* /\*   print_dbg_short_hex(nvram_data->var16); *\/ */

/* /\*   print_dbg("\r\nvar8_3:\t0x"); *\/ */
/* /\*   print_dbg_char_hex(nvram_data->var8_3[0]); *\/ */
/* /\*   print_dbg_char_hex(nvram_data->var8_3[1]); *\/ */
/* /\*   print_dbg_char_hex(nvram_data->var8_3[2]); *\/ */

/* /\*   print_dbg("\r\nvar32:\t0x"); *\/ */
/* /\*   print_dbg_hex(nvram_data->var32); *\/ */

/* /\*   print_dbg("\r\n"); *\/ */
/* /\* } *\/ */


/* /\*! \brief This is an example demonstrating flash read / write data accesses */
/*  *         using the FLASHC driver. */
/*  * */
/*  * \param caption     Caption to print before running the example. */
/*  * \param nvram_data  Pointer to the NVRAM data structure to use in the example. */
/*  *\/ */

/* /\* static void flash_rw_example(const char *caption, nvram_data_t *nvram_data) *\/ */
/* /\* { *\/ */
/* /\*   static const uint8_t write_data[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}; *\/ */

/* /\*   print_dbg(caption); *\/ */

/* /\*   print_dbg("Initial values of NVRAM variables:\r\n"); *\/ */
/* /\*   print_nvram_variables(nvram_data); *\/ */

/* /\*   print_dbg("\r\nClearing NVRAM variables..."); *\/ */
/* /\*   flashc_memset((void *)nvram_data, 0x00, 8, sizeof(*nvram_data), true); *\/ */
/* /\*   print_dbg("\r\nNVRAM variables cleared:\r\n"); *\/ */
/* /\*   print_nvram_variables(nvram_data); *\/ */

/* /\*   print_dbg("\r\nWriting new values to NVRAM variables..."); *\/ */
/* /\*   flashc_memcpy((void *)&nvram_data->var8,   &write_data, sizeof(nvram_data->var8),   true); *\/ */
/* /\*   flashc_memcpy((void *)&nvram_data->var16,  &write_data, sizeof(nvram_data->var16),  true); *\/ */
/* /\*   flashc_memcpy((void *)&nvram_data->var8_3, &write_data, sizeof(nvram_data->var8_3), true); *\/ */
/* /\*   flashc_memcpy((void *)&nvram_data->var32,  &write_data, sizeof(nvram_data->var32),  true); *\/ */
/* /\*   print_dbg("\r\nNVRAM variables written:\r\n"); *\/ */
/* /\*   print_nvram_variables(nvram_data); *\/ */
/* /\* } *\/ */

/* //------------------------- */
/* /\* void test_flash(void) { *\/ */
/* /\*   /\\* void* data = &flash_nvram_data; *\\/ *\/ */


/*  /\*  print_dbg("\r\n \r\n ---------------- running flash test; NV data address: "); *\/ */
/*  /\*  print_dbg_hex(data); *\/ */

/*  /\*  flash_rw_example("\r\n ASF TEST :", data); *\/ */



/* } */

