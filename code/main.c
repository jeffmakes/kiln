#include "device.h"
#include "lcd.h"
#include "encoder.h"
#include "adc10.h"
#include "triac.h"
#include "thermocouple.h"
#include <signal.h>

/* Initialises everything. */
void init(void);

int i = 0;

int main( void )
{
  uint8_t oldpos = 0xff;
  uint8_t oldbutt = 0xff;
  uint16_t oldtemp = 0xff;
  uint32_t time;

  init();
  //	lcd_send_char(0xdf);
  
  lcd_print_string("Cats:");
  lcd_cursor_to(9,1);
  lcd_send_char(0xdf);
  lcd_send_char('C');  

  while(1)
    { 
      for (time=0; time<1000; time++);

      if (encoderpos != oldpos)
	{
	  lcd_cursor_to(5,0);
	  lcd_print_num(triac_triggerphase, 5);

	  triac_set_phase(encoderpos * 40);
	}
      oldpos = encoderpos;

      if (encoder_button != oldbutt)
	{
	  if (encoder_button)
	    {
	      lcd_cursor_to(0,1);
	      lcd_print_string("Bees!");
	    }
	  else
	    {
	      lcd_cursor_to(0,1);
	      lcd_print_string("Temp:");
	    }
	  oldbutt = encoder_button;
	}
      
      if (thermocouple_temp != oldtemp)
	{
	  lcd_cursor_to(5,1);
	  lcd_print_num(thermocouple_temp, 4);
	}
      oldtemp = thermocouple_temp;
    }
}

void init(void)
{
	/* Disable the watchdog timer */
  //	WDTCTL = WDTHOLD | WDTPW;

	/* GPIO: All inputs */
	P1DIR = P2DIR = P3DIR = P4DIR = 0;

	/* Use a 8 MHz clock (DCO) */
	DCOCTL = CALDCO_8MHZ;
	BCSCTL1 &= ~0x0f;
	BCSCTL1 |= 
		/*XT2O=0: XT2 is on*/
		/*XTS=0: LFXT1 mode select. 0 -Low frequency mode*/
		DIVA_0 /* ACLK Divider 0: /1 */
		|CALBC1_8MHZ; /* BCSCTL1 Calibration Data for 16MHz */

	BCSCTL2 = SELM_DCOCLK	/* MCLK from DCO */
		/* DIVMx = 0 : No MCLK divider */
		/* SELS = 0: SMCLK from DCO */
	  | DIVS_DIV8 /* : Divide SMCLK by 8 */
		/* DCOR = 0 : DCO internal resistor */;

	BCSCTL3 = LFXT1S1; /*VLOCLK */
	
	lcd_init();
	encoder_init();
	adc10_init();
	triac_init();
	eint();
}
