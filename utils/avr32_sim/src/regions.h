/* regions.h 
   
   draw and otherwise manipulate  arbitrary grids of pixels.

 */

// data type for screen regions
typedef struct _region { 
  // width
  u8 w;
  // height
  u8 h;
  // size (store for speed)
  u32 len;
  // x offset
  u8 x;
  // y offset
  u8 y;  
  // dirty flag
  u8 dirty;
  // data
  u8 * data;
} region;


// allocate and initialize region 
extern void region_init(region* reg);
