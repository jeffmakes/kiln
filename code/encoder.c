#include <signal.h>
#include "encoder.h"
#include "lcd.h"
#include "triac.h"
#include "control.h"

uint8_t encoderpos = 0;
uint8_t encoder_button = 0;

void encoder_init()
{
  P2DIR &= ~0x07;
  P2REN |= 0x07;
  P2OUT |= 0x07;
  P2IE |= 0x07;
  P2IES &= ~0x07;
}

void encoder_change(enc_dir_t dir)
{
  if ( dir == CLOCKWISE )
    setpoint += 10;
  else 
    setpoint -= 10;
  
  if (setpoint > MAX_SETPOINT)
    setpoint = MAX_SETPOINT;
  else if (setpoint < MIN_SETPOINT)
    setpoint = MIN_SETPOINT;
}

void encoder_button_down()
{
  encoder_button = 1;
}

void encoder_button_up()
{
  encoder_button = 0;
}
  
interrupt (PORT2_VECTOR) port2_isr(void)
{
  static uint8_t oldstate = 0;
  uint8_t newstate = 0;
  uint8_t bounce0, bounce1, bouncecount=0;
  static uint8_t acw = 0;
  static uint8_t cw = 0;

  if (P2IFG & (1<<5))		/* reset timer A on zero-crossing */
    {
      if (triac_new_triggerphase != triac_triggerphase)
	triac_triggerphase = triac_new_triggerphase;
      TACTL |= TACLR;
      TACCR2 = triac_triggerphase;
      TACCTL2 = OUTMOD_SET | CCIE;
      P2IFG &= ~(1<<5);
    }

  if (P2IFG & 0x03)		/* interrupt from quadrature signals? */
    {
      P2IES ^= (P2IFG & 0x03);	/* toggle the edge select bits to catch the next edge */
      P2IFG = 0;	     	/* messing with P2IES can cause interrupts, so clear flags */

      while (bouncecount < 10)
	{
	  bounce0 = P2IN & 0x03;
	  bounce1 = P2IN & 0x03;
	  if (bounce0 != bounce1)
	    bouncecount = 0;
	  else
	    bouncecount++;
	}
      bouncecount = 0;
     
      newstate = P2IN & 0x03;
      if (newstate != oldstate)
	{
	  if ( (newstate & 0x01) ^ ((oldstate >> 1)&0x01) )
	    {
	      acw++;
	      if (acw > 3)
		{
		  acw = cw = 0;
		  encoder_change(ANTICLOCKWISE);
		}
	    }
	  else
	    {
	      cw++;
	      if (cw > 3)
		{
		  acw = cw = 0;
		  encoder_change(CLOCKWISE);
		}
	    }
	}
      oldstate = newstate;
    }


  if (P2IFG & (1<<2))      /* interrupt from switch */
    {
      while (bouncecount < 5)
	{
	  bounce0 = P2IN & (1<<2);
	  bounce1 = P2IN & (1<<2);
	  if (bounce0 != bounce1)
	    bouncecount = 0;
	  else
	    bouncecount++;
	}
      bouncecount = 0;

      if (P2IN & (1<<2))
	{
	  P2IES |= (1<<2);
	  P2IFG &= ~(1<<2);
	  encoder_button_up();
	}
      else
	{
	  P2IES &= ~(1<<2);
	  P2IFG &= ~(1<<2);
	  encoder_button_down();
	}
    }
}    

    
    

	    
	  
	  
