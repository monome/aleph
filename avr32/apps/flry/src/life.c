#include "print_funcs.h"

#include "life.h"
#include "monome.h"
#include "ctl.h"

s8 lifenow[128];
s8 lifenext[128];

u8 changed;
u8 step, mass, v, h;

//---------------------------------------
//--- static functions
static u8 neighbors(u8 x, u8 y)
{
  return lifenow[((x + 1) % 8) + ((y)<<4)] + 
    lifenow[(x) + (((y + 1) % 8)<<4)] + 
    lifenow[((x + 8 - 1) % 8) + ((y)<<4)] + 
    lifenow[(x) + (((y + 8 - 1) % 8)<<4)] + 
    lifenow[((x + 1) % 8) + (((y + 1) % 8)<<4)] + 
    lifenow[((x + 8 - 1) % 8) + (((y + 1) % 8)<<4)] + 
    lifenow[((x + 8 - 1) % 8) + (((y + 8 - 1) % 8)<<4)] + 
    lifenow[((x + 1) % 8) + (((y + 8 - 1) % 8)<<4)]; 
}

//---------------------------------
//-- external functions

void life_print(void) {
	u8 x,y;
	print_dbg("\r\n");
	for(y=0;y<8;y++)
		for(x=0;x<8;x++) 
			print_dbg_ulong(lifenext[x+(y<<4)]);
}

void life_init(void) {
	u8 i;
	for(i=0;i<128;i++)
	{
		lifenow[i] = 0;
		lifenext[i] = 0;	
	}
}

void life_change(u8 x,u8 y) {
	lifenext[x+(y<<4)] = 1;
}

void life_update(void) {
	u8 i, x, y, count;
	u8 *p = monomeLedBuffer;

	for(x=0;x<8;x++)
	{
		for(y=0;y<8;y++)
		{
			i = x+(y<<4);
			if(lifenext[i]==1)
			{
				lifenow[i]=1;
				p[i]=1;
			}
			else if(lifenext[i]==-1)
			{
				lifenow[i]=0;
				p[i]=0;
			}
			lifenext[i]=0;
		}
	}

	for (x=0; x < 8; x++)
	{ 
        for (y=0; y < 8; y++) 
        { 
			i = x+(y<<4);
      		count = neighbors(x, y);

      		if (count == 3 && lifenow[i] == 0) 
      			lifenext[i]=1;
      		if ((count < 2 || count > 3) && lifenow[i] == 1) 
     		 	lifenext[i]=-1;  
    	} 
  	} 

	changed = 0;

	monome_set_quadrant_flag(0);

	step ^=0xff;
	v=h=0;
	for(x=0;x<8;x++)
	{
		for(y=0;y<4;y++)
			if(lifenow[x+(y<<4)]) 
				v++;
		for(y=4;y<8;y++)
			if(lifenow[x+(y<<4)]) 
				h++;
	}

	mass = (v+h)==0;
	ctl_set_value(0,step<<12);
	ctl_set_value(1,mass<<15);
	ctl_set_value(2,v<<11);
	ctl_set_value(3,h<<11);


}
