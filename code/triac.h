#ifndef __TRIAC_H
#define __TRIAC_H
#include <stdint.h>
#include <iomacros.h>


void triac_init( void );
void triac_set_phase(uint16_t phase);

extern uint16_t triac_triggerphase;
extern uint16_t triac_new_triggerphase;

#define MAX_TRIGGERPHASE 9500
#define MIN_TRIGGERPHASE 500

sfrb(ADC10AE1, 0x004b);

#define triac_on() do { P2OUT |= (1<<4); } while (0)
#define triac_off() do { P2OUT &= ~(1<<4); } while (0)

#endif
