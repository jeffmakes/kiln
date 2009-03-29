#include "device.h"
#include "lcd.h"
#include "encoder.h"
#include "adc10.h"
#include "triac.h"
#include "thermocouple.h"
#include "control.h"
#include <signal.h>

#define MAX_OUTPUT_POWER 7500
#define MIN_OUTPUT_POWER 500

int16_t setpoint = MIN_SETPOINT;

/* Initialises everything. */
void init(void);
void draw_display();
void update_display();
int i = 0;

int main( void )
{
  uint32_t time;
  int16_t perror = 0;
  int16_t ierror = 0;
  int8_t kp = 60;
  int8_t ki = 5;
  int32_t outputpower = 500;
  uint16_t itimer = 0;
  uint16_t itimeout = 30;

  init();
  triac_off();
  draw_display();  

  while(1)
    {
      for (time=0; time<1000; time++);
      adc10_poll();
      update_display();

      /* proportional error */
      perror = setpoint - (int16_t)thermocouple_temp;

      /* integral error (only compute when output is not saturated, and only infrequently) */
      if ( (itimer++) == itimeout)
	{
	  itimer = 0;
	   if ( (outputpower != MAX_OUTPUT_POWER) && (outputpower != MIN_OUTPUT_POWER) )
	    ierror += perror;
	}
      
      outputpower = (kp * (int32_t)perror) + (ki * (int32_t)ierror);
      if (outputpower > MAX_OUTPUT_POWER)
	outputpower = MAX_OUTPUT_POWER;
      else if (outputpower < MIN_OUTPUT_POWER)
	outputpower = MIN_OUTPUT_POWER;
	
      triac_set_power(outputpower);
    }
}

void init(void)
{
	/* Disable the watchdog timer */
  WDTCTL = WDTHOLD | WDTPW;

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


void draw_display()
{
  lcd_cursor_to(0,0);
  lcd_print_string("Setpoint:     C");
  lcd_cursor_to(0,1);
  lcd_print_string("Temp:         C");
  lcd_cursor_to(14,0);
  lcd_send_char(0xdf);
  lcd_cursor_to(14,1);
  lcd_send_char(0xdf);
}

void update_display()
{
  /* Display format: (16x2 characters) */
  /* Setpoint:_XXXXoC */
  /* Temp:_____YYYYoC */
  

  lcd_cursor_to(10,0);
  lcd_print_num(setpoint, 4);
  lcd_cursor_to(10,1);
  lcd_print_num(thermocouple_temp, 4);
}
