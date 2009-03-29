#ifndef __TRIAC_H
#define __TRIAC_H
#include <stdint.h>
#include <iomacros.h>


void triac_init( void );
void triac_set_power(uint16_t power);
void triac_reset_ccr2(void);

extern uint16_t triac_triggerphase;
extern uint16_t triac_new_triggerphase;

/* this defines the earliest trigger point of the AC wave. The minimum phase offset corresponds to the maximum power deivered to the load.  The element could burn out if this is set too low.  */
#define MIN_TRIGGERPHASE 5000	

#define MAX_TRIGGERPHASE 9500

#define triac_on() do { P2OUT |= (1<<4); } while (0)
#define triac_off() do { P2OUT &= ~(1<<4); } while (0)

#endif
