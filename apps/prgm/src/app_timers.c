//app_timers.c
//aleph-prgm-avr32

/*
this is where callbacks are declared for all application-specific software timers.
 these callbacks are performed from the TC interrupt service routine.
 therefore, they should be kept small.
 
 avr32_lib defines a custom application event type, 
 which should be used when timer-based processing needs to be deferred to the main loop.
*/

#include "app_timers.h"

//static variables
static event_t e;
static u16 adc[4];
static u8 state;
static u16 holdvalue;
static const int kEventAdcId[] =
{
    kEventAdc0,
    kEventAdc1,
};


//TIMERS
//  poll adc
static softTimer_t adcPollTimer = { .next = NULL, .prev = NULL };

//  poll i2c/twi
static softTimer_t i2cPollTimer = { .next = NULL, .prev = NULL };

//  refresh screen
static softTimer_t screenTimer = { .next = NULL, .prev = NULL };

//  poll encoders
static softTimer_t encTimer = { .next = NULL, .prev = NULL };

//  poll custom
static softTimer_t customPollTimer = { .next = NULL, .prev = NULL };

// poll monome device
//static softTimer_t monomePollTimer = { .next = NULL, .prev = NULL };
// refresh monome device
//static softTimer_t monomeRefreshTimer  = { .next = NULL, .prev = NULL };
// poll midi device
//static softTimer_t midiPollTimer = { .next = NULL, .prev = NULL };

//  adc polling callback
static void adc_poll_timer_callback(void *obj) {
    static const int threshold = 50;
    
    adc_convert(&adc);
    u16 i = adc[0];
    s16 diff = i - holdvalue;
    
    if (state == 0)
    {
        //  wait for trig
        if (diff > threshold)
        {
            //  trig found
            e.type = kEventAdc0;
            e.data = 1;
            event_post(&e);
            state = 1;
        }
        
    } else
    {
        //  wait for end of trig
        if (diff < -threshold)
        {
            //  end of trig found
            state = 0;
        }
    }
    holdvalue = i;
}

/*
    static const int threshold = 50;
    
    adc_convert(&adc);
    
    for(int n = 0; n < 1; n++)
    {
        u16 i = adc[n];
        s16 diff = i - holdvalue[n];
        
        if (state == 0)
        {
            //  wait for trig
            if (diff > threshold)
            {
                //  trig found
                e.type = kEventAdcId[n];
                e.data = 1;
                event_post(&e);
                state = 1;
            }
            
        } else
        {
            //  wait for end of trig
            if (diff < -threshold)
            {
                //  end of trig found
                state = 0;
            }
        }
        holdvalue[n] = i;
    }
}
*/

//i2c polling callback
static void i2c_poll_timer_callback(void* obj) {
    //...
}


//  screen refresh callback
static void screen_timer_callback(void* obj) {
    render_update();
}


//  encoder polling callback
static void enc_timer_callback(void* obj) {
    static s16 val, valAbs;
    u8 i;
        
    for(i=0; i<NUM_ENC; i++) {
        val = enc[i].val;
        valAbs = (val & 0x8000 ? (val ^ 0xffff) + 1 : val);
        //  no wrapping on threshold
        if (valAbs > enc[i].thresh) {
            e.type = enc[i].event;
            e.data = val;
            enc[i].val = enc[i].thresh;
            event_post(&e);
        }
    }
}


//  custom callback
static void sample_timer_callback(void *obj) {
    ParamValueSwap s;
    
    while (idx8 < bfinSampleSize)
    {
        ctl_param_change(DUMMY, eParamOffset, sample[smpl]->offset + idx32);
        
        s.asByte[0] = bfinSampleData[idx8];
        s.asByte[1] = bfinSampleData[idx8 + 1];
        s.asByte[2] = bfinSampleData[idx8 + 2];
        s.asByte[3] = bfinSampleData[idx8 + 3];
        
        ctl_param_change(DUMMY, eParamSample, s.asInt);
                
        idx8 += 4;
        idx32++;
    }
}

/*
 u8 clk;
 u8 clk_state;
 u8 tempo;
 
 void clock(void) {
 if(clk_state)
 {
 clk += 1;
 if (clk >= tempo)
 {
 clk = 0;
 send event -> module_set_clock_in();
 }
 }
 }
*/

static void scrub_timer_callback(void *obj) {
/*
    switch (motor) {
            //no movement
        case 0:
            <#statements#>
            break;
            //start fwd /rvs
        case 1:
            
            motor = 7;
            motor = 8;
            <#statements#>
            break;
            //stop
        case 2:
            
            motor = 0;
            <#statements#>
            break;
            //manual forward
        case 3:
            
            motor = 0;
            <#statements#>
            break;
            //manual reverse
        case 4:
            
            motor = 0;
            <#statements#>
            break;
            //manual fwd -> rvs
        case 5:
            
            motor = 4;
            <#statements#>
            break;
            //manual rvs -> fwd
        case 6:
            
            motor = 3;
            <#statements#>
            break;
            //motor fwd
        case 7:
            <#statements#>
            break;
            //motor rvs
        case 8:
            <#statements#>
            break;
            
        default:
            break;
    }
    
    
    
    
    ctl_param_change(DUMMY, eParamSigned0, enc[2].val);
 */
}


/*
 static void op_fade_in_a(op_fade_t* fade, const io_t v) {
 fade->a = v;
 // do the math for crossfade:
 fade->val = (fade->a * (128 - fade->x)) / 128 + (fade->b * fade->x) / 128;
 // activate output
 net_activate(fade->outs[0], fade->val, fade);
 }
 
 static void op_fade_in_b(op_fade_t* fade, const io_t v) {
 fade->b = v;
 fade->val = (fade->a * (128 - fade->x)) / 128 + (fade->b * fade->x) / 128;
 net_activate(fade->outs[0], fade->val, fade);
 }
 
 static void op_fade_in_x(op_fade_t* fade, const io_t v) {
 fade->x = v;
 // limit range of x
 if(fade->x < 0) fade->x = 0;
 else if(fade->x > 128) fade->x = 128;
 
 fade->val = (fade->a * (128 - fade->x)) / 128 + (fade->b * fade->x) / 128;
 net_activate(fade->outs[0], fade->val, fade);
 }
 */



/*
    else
    env->x += env->decay;
    speed.idx = add_fr1x32(speed[0], mult_fr1x32x32(env->x, sub_fr1x32(speed[255], speed[0])));
    env->x -= env->decay * 0.5;
    
    static inline double dease_func(double y1, char updwn, double ramp, long rfad)
    { return updwn ? y1*(0.5*(1.0-cos((1.0-(((double)rfad)/ramp))*PI))) : y1*(0.5*(1.0-cos((((double)rfad)/ramp)*PI))); }
    
     static inline double snrdease_func(double y1, double fad, long curv)
    {
        switch (curv)
        {
            case 0: y1 = y1*(1.0-fad); break;  //linear
            case 1: y1 = y1*(1.0-(sin((fad-1)*PI/2)+1)); break;  //sine ease in
            case 2: y1 = y1*(1.0-(fad*fad*fad)); break;         //cubic ease in
            case 3: fad = fad-1; y1 = y1*(1.0-(fad*fad*fad+1)); break; //cubic ease out
            case 4: y1 = y1*(1.0-((fad==0.0)?fad:pow(2, 10*(fad-1)))); break; //exponential ease in
            case 5: y1 = y1*(1.0-((fad==1.0)?fad:1-(pow(2, -10*fad)))); break; //exponential ease out
            case 6:
                if ((fad>0)&&(fad<0.5)) y1 = y1*(1.0-(0.5*pow(2, (20*fad)-10)));
                else if ((fad<1)&&(fad>0.5)) y1 = y1*(1.0-(-0.5*pow(2, (-20*fad)+10)+1)); break; //exp easinout
        }
        return y1;
    }
    
    static inline void bufeasoff(long frms, float *b, long nchn, long mrk, char dr, double ramp)
    {
        long i, fadpos;
        for (i=0; i<ramp; i++)
        {
            fadpos = mrk + (dr*i);
            if(!((fadpos<0)||(fadpos>frms)))
            {
                b[fadpos*nchn] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI));
                if (nchn > 1) { b[(fadpos*nchn)+1] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI)); }
            }
        } return;
    }
    
    static inline void bufeas(long frms,float *b,long nchn,long mrk,long mrk2,char dr, double ramp)
    {
        long i, fadpos, fadpos2, fadpos3;
        for (i=0; i<ramp; i++)
        {
            fadpos = (mrk+(-dr)) + (-dr*i); fadpos2 = (mrk2+(-dr)) + (-dr*i); fadpos3 = mrk2 + (dr*i);
            if(!((fadpos<0)||(fadpos>frms)))
            {
                b[fadpos*nchn] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI));
                if (nchn > 1) {b[(fadpos*nchn)+1] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI));}
            }
            if(!((fadpos2<0)||(fadpos2>frms)))
            {
                b[fadpos2*nchn] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI));
                if (nchn > 1) {b[(fadpos2*nchn)+1] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI));}
            }
            if(!((fadpos3<0)||(fadpos3>frms)))
            {
                b[fadpos3*nchn] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI));
                if (nchn > 1) {b[(fadpos3*nchn)+1] *= 0.5 * ( 1.0-cos( (((double)i)/ramp) * PI));}
            }
        } return;
    }
    
    static inline float dease_funcf(float y1, char updwn, float ramp, long rfad)
    { return updwn ? y1*(0.5f*(1.0f-cosf((1.0f-(((float)rfad)/ramp))*PI))) : y1*(0.5f*(1.0f-cosf((((float)rfad)/ramp)*PI))); }
    
    static inline float snrdease_funcf(float y1, float fad, long curv)
    {
        switch (curv)
        {
            case 0: y1 = y1*(1.0f-fad); break;  //linear
            case 1: y1 = y1*(1.0f-(sin((fad-1)*PI/2)+1)); break;  //sine ease in
            case 2: y1 = y1*(1.0f-(fad*fad*fad)); break;         //cubic ease in
            case 3: fad = fad-1; y1 = y1*(1.0f-(fad*fad*fad+1)); break; //cubic ease out
            case 4: y1 = y1*(1.0f-((fad==0.0)?fad:pow(2, 10*(fad-1)))); break; //exponential ease in
            case 5: y1 = y1*(1.0f-((fad==1.0)?fad:1-(pow(2, -10*fad)))); break; //exponential ease out
            case 6:
                if ((fad>0)&&(fad<0.5)) y1 = y1*(1.0-(0.5*pow(2, (20*fad)-10)));
                else if ((fad<1)&&(fad>0.5)) y1 = y1*(1.0-(-0.5*pow(2, (-20*fad)+10)+1)); break; //exp easinout
        }
        return y1;
    }
    
    static inline void bufeasofff(long frms, float *b, long nchn, long mrk, char dr, float ramp)
    {
        long i, fadpos;
        for (i=0; i<ramp; i++)
        {
            fadpos = mrk + (dr*i);
            if(!((fadpos<0)||(fadpos>frms)))
            {
                b[fadpos*nchn] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI));
                if (nchn > 1) { b[(fadpos*nchn)+1] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI)); }
            }
        } return;
    }
    
    static inline void bufeasf(long frms,float *b,long nchn,long mrk,long mrk2,char dr, float ramp)
    {
        long i, fadpos, fadpos2, fadpos3;
        for (i=0; i<ramp; i++)
        {
            fadpos = (mrk+(-dr)) + (-dr*i); fadpos2 = (mrk2+(-dr)) + (-dr*i); fadpos3 = mrk2 + (dr*i);
            
            if(!((fadpos<0)||(fadpos>frms)))
            {
                b[fadpos*nchn] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI));
                if (nchn > 1) {b[(fadpos*nchn)+1] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI));}
            }
            if(!((fadpos2<0)||(fadpos2>frms)))
            {
                b[fadpos2*nchn] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI));
                if (nchn > 1) {b[(fadpos2*nchn)+1] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI));}
            }
            if(!((fadpos3<0)||(fadpos3>frms)))
            {
                b[fadpos3*nchn] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI));
                if (nchn > 1) {b[(fadpos3*nchn)+1] *= 0.5f * ( 1.0f-cosf( (((float)i)/ramp) * PI));}
            }
        } return;
    }
    
    static inline void interp_index(long pos,long *indx0,long *indx1,long *indx2,long *indx3,char dir,char diro,long loop,long frmsm)
    {
        *indx0 = pos - dir;                                                         //calc of indexes 4 interps
        if (diro>=0) { if(*indx0 < 0) { *indx0 = (loop+1) + *indx0; } else if (*indx0 > loop) { *indx0 = *indx0 - (loop+1); } }
        else
        {
            if(*indx0 < (frmsm-loop))
            { *indx0 = frmsm - ((frmsm-loop)-*indx0); } else if (*indx0 > frmsm) { *indx0 = (frmsm-loop)+(*indx0-frmsm); }
        }
        
        *indx1 = pos; *indx2 = pos + dir;
        if (diro>=0) { if (*indx2 < 0) { *indx2 = (loop+1) + *indx2; } else if (*indx2 > loop) { *indx2 = *indx2 - (loop+1); } }
        else
        {
            if (*indx2 < (frmsm-loop)) { *indx2 = frmsm - ((frmsm-loop)-*indx2); }
            else if (*indx2 > frmsm) { *indx2 = (frmsm-loop)+(*indx2-frmsm); }
        }
        
        *indx3 = *indx2 + dir;
        if (diro>=0)
        { if (*indx3 < 0) { *indx3 = (loop+1) + *indx3; } else if (*indx3 > loop) { *indx3 = *indx3 - (loop+1); } }
        else
        {
            if (*indx3 < (frmsm-loop)) { *indx3 = frmsm - ((frmsm-loop)-*indx3); }
            else if (*indx3 > frmsm) { *indx3 = (frmsm-loop)+(*indx3-frmsm); }
        } return;
    }
 */
    
    /*static inline void rec_index(long pos,long *rpos,char dir,char diro,long loop,long frmsm) //<-possible 'to-do': prepare separate rec..
     {                                                                                              //..index for cubic interp at all times.
     *rpos=pos-(2*dir); if(diro>=0){ if(*rpos < 0) { *rpos = (loop+1) + *rpos; } else if (*rpos > loop) { *rpos = *rpos - (loop+1); } }
     else { if(*rpos<(frmsm-loop)){ *rpos=frmsm-((frmsm-loop)-*rpos); } else if(*rpos>frmsm){ *rpos=(frmsm-loop)+(*rpos-frmsm); } }
     }*/
    

//    if (speed), start slow down
    //    ctl_param_change(DUMMY, eParamOffset, sample[smpl]->offset + idx32);

/*
    s = ....
    e.type = 
    
    while (idx8 < bfinSampleSize)
    {
        ctl_param_change(DUMMY, eParamOffset, sample[smpl]->offset + idx32);
        
        s.asByte[0] = bfinSampleData[idx8];
        s.asByte[1] = bfinSampleData[idx8 + 1];
        s.asByte[2] = bfinSampleData[idx8 + 2];
        s.asByte[3] = bfinSampleData[idx8 + 3];
        
        ctl_param_change(DUMMY, eParamSample, s.asInt);
        
        idx8 += 4;
        idx32++;
    }
}
*/

//external functions
void init_sample_timer(void) {
    timer_add(&customPollTimer, 1, &sample_timer_callback, NULL);
}

void init_scrub_timer(void) {
    timer_add(&customPollTimer, 50, &scrub_timer_callback, NULL);
}

void init_app_timers(void) {
    timer_add(&screenTimer, 50, &screen_timer_callback, NULL);
    timer_add(&encTimer, 50, &enc_timer_callback, NULL);
//    timer_add(&i2cPollTimer, 50, &i2c_poll_timer_callback, NULL);
}

//init adc, set adc timers
void adc_init(void) {
    timers_set_adc(1);
}

//start adc polling
void timers_set_adc(u32 period) {
    timer_add(&adcPollTimer, period, &adc_poll_timer_callback, NULL);
}

//set adc polling period
void timers_set_adc_period(u32 period) {
    adcPollTimer.ticks = period;
}
