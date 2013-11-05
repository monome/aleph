#ifndef _ALEPH_CONF_TC_IRQ_H_
#define _ALEPH_CONF_TC_IRQ_H_

//============================================
//====== IRQ priorities
#define SYS_IRQ_PRIORITY       1
#define APP_TC_IRQ_PRIORITY    3
#define UI_IRQ_PRIORITY        2

//==============================================
//==== timers

// application timer
#define APP_TC                 (&AVR32_TC)
#define APP_TC_CHANNEL         0
#define APP_TC_IRQ             AVR32_TC_IRQ0
#define APP_TC_IRQ_GROUP       AVR32_TC_IRQ_GROUP

#endif
