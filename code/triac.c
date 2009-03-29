#include "triac.h"
#include <signal.h>
#include <stdint.h>
#include "watchdog.h"
#include "device.h"

#define OAADC_1 0x01
#define TRIGGER_PULSELENGTH 200
uint16_t triac_triggerphase = 0;
uint16_t triac_new_triggerphase = 0;

void triac_set_power(uint16_t power)
{
  triac_new_triggerphase = 10000-power;

  if ( triac_new_triggerphase > MAX_TRIGGERPHASE)
    triac_new_triggerphase = MAX_TRIGGERPHASE;
  else if (triac_new_triggerphase < MIN_TRIGGERPHASE)
    triac_new_triggerphase = MIN_TRIGGERPHASE;
}


void triac_init(void)
{
  /* Using opamp 0 module as a comparator to detect zero crossing */
  /* Takes input from transformer on P4.5 (OA0I3) and compares it */
  /* against an internal resistor ladder. */
  /* The output signal is present on P4.3 (OA0O) */
  OA0CTL0 = 
    OAP_3   		/* non-inverting input on OA0I3 (OA0IB) */
    | OAPM_3 			/* fast slew rate */
    | OAADC_1; 			/* output on external pins and ADC A12,13,14 */
  
  OA0CTL1 =  
    OAFBR_2 		/* Tap on feedback ladder */
    | OAFC_3;			/* Comparator configuration */

  ADC10AE1 |= (1<<6) | (1<<4);	/* Enable analogue channels for OA0I3 and OA0O */

  /* Using Timer A to generate the optocoupler LED drive with*/
  /* Optoisolator LED is driven through a FET connected to P2.4 */
  /* Timer A Capture/compare 2 produces output signal TA2 (P2.4) */
  P2SEL |= (1<<4);		/* Enable P2.4 as TA2 output */
  P2DIR |= (1<<4);		/* Make it an output */
  P2IE |= (1<<5);		/* Interrupt on zero-crossing input */
  P2IES &= ~(1<<5);		/* Interrupt on rising edge */

  TACTL =
    TASSEL_SMCLK		/* Clock timer A from SMCLK */
    //    | ID_DIV2 		/* Timer A clock = SMCLK/2 = 500kHz */
    | MC_UPTO_CCR0;			/* Continuous count mode */

  /* CCR2 config */
  TACCTL2 = OUTMOD_SET	/* TA2 set on CCR2, cleared on CCR0 */
    | CCIE;			/* interrupt on CCR2 */

  /* enable CCR0 interrupt */
  TACCTL0 |= CCIE;

  TACCR0 = 10000;		/* = 10ms = half a 50Hz period */
  TACCR2 = triac_triggerphase;
}

interrupt (TIMERA1_VECTOR) ta_isr(void)
{
  if (TAIV & 0x04)		/* when TA hits CCR2, move CCR2 to set the end of the firing pulse */
    {
      if (TACCR2 == triac_triggerphase)
	TACCR2 = triac_triggerphase + TRIGGER_PULSELENGTH;
      
      TACCTL2 = OUTMOD_RESET | CCIE;
      TAIV &= ~0x04;
    }

  clear_wdt();
}

interrupt (TIMERA0_VECTOR) ta_ccr0(void)
{
  /* Timer reset, reconfigure CCR2 */
  TACCR2 = triac_triggerphase;
  TACCTL2 = OUTMOD_SET | CCIE;
}
