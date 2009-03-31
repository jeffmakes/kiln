#include "device.h"
#include "lcd.h"
#include "encoder.h"
#include "adc10.h"
#include "triac.h"
#include "thermocouple.h"
#include "control.h"
#include "scheduler.h"
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
  int16_t kp = 50;
  int16_t ki = 20;
  int32_t outputpower = 500;
  uint16_t itimer = 0;
  uint16_t itimeout = 20;

  init();
  triac_off();
  //  draw_display();  

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
 
      if (perror < (-20))		/* If temperature is way above setpoint, turn off element. */
	outputpower = MIN_OUTPUT_POWER;
     
      if (outputpower > MAX_OUTPUT_POWER)
	outputpower = MAX_OUTPUT_POWER;
      else if (outputpower < MIN_OUTPUT_POWER)
	outputpower = MIN_OUTPUT_POWER;
	
      triac_set_power(outputpower);
    }
}

void init(void)
{
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
  
  BCSCTL3 = LFXT1S0; /*32768Hz watch crystal */
  
  lcd_init();
  encoder_init();
  adc10_init();
  triac_init();
  scheduler_init();
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
  lcd_cursor_to(0,0);		/* Top line... */
  if (status == PROFILE_END)
    lcd_print_string("Profile Complete");
  else
    {
      lcd_print_num(ramp,1);
      lcd_send_char(':');
      switch(status)
	{
	case RAMP_UP:
	  lcd_print_string("Ramp+ ");
	  break;
	case RAMP_DOWN:
	  lcd_print_string("Ramp- ");
	  break;
	default:
	  lcd_print_string("Hold  ");
	  break;
	}
      lcd_print_num(profile[ramp].end_temp, 3);
      lcd_send_char(0xdf);
      lcd_print_num( ((ramp_transition_time - seconds)/60)+1, 3);
      lcd_send_char('m');
    }

  lcd_cursor_to(0,1);		/* Bottom line... */
  lcd_print_string("Set");
  lcd_print_num(setpoint, 4);
  lcd_send_char(0xdf);
  lcd_print_string(" At");
  lcd_print_num(thermocouple_temp, 4);
  lcd_send_char(0xdf);
}
