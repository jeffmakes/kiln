#include "device.h"
#include "control.h"
#include "scheduler.h"
#include <stdint.h>
#include "signal.h"

uint32_t seconds = 0;
uint8_t ramp = 0;
ramp_t profile[7];
profile_status_t status = PROFILE_START;
uint32_t ramp_transition_time = 0;


void scheduler_init(void)
{
  /* Set up Timer B for timing temperature profiles. It's clocked by ACLK */
  /* which is connected to the LFXT1 32kHz watch crystal oscillator */

  TBCTL = 
    TBSSEL_ACLK 		/* Source Timer B from ACLK */
    | MC_UPTO_CCR0;		/* Count to CCR0 value */

  TBCCTL0 = CCIE;		/* Enable TBCCR0 interrupt */
  TBCCR0 = 32767;		/* Set up CCR0 for 1Hz interrupt */

  profile[0].flags = FLAG_PROFILE_START;
  profile[0].end_temp = 20; 

  profile[1].end_temp = 270;
  profile[1].duration = (0.5 * 3600);
  
  profile[2].end_temp = 270;
  profile[2].duration = (1 * 3600);
  
  profile[3].end_temp = 730;
  profile[3].duration = (1 * 3600);

  profile[4].end_temp = 730;
  profile[4].duration = (2 * 3600);

  profile[5].end_temp = 620;
  profile[5].duration = (0.5 * 3600);

  profile[6].flags = FLAG_PROFILE_END;
  profile[6].end_temp = 620;	/* end_temp of last profile point determines setpoint at end of profile. */
}

interrupt (TIMERB0_VECTOR) tb_isr_1Hz(void)
{
  static float degrees_per_second = 0;
  static float schedule_setpoint = 20;

  schedule_setpoint += degrees_per_second;
  setpoint = (int16_t)schedule_setpoint;

  if (seconds == ramp_transition_time) /* check for end of ramp */
    {				
      setpoint = profile[ramp].end_temp;

      ramp++;			/* yep, move to the next ramp */

      if ( !(profile[ramp].flags & FLAG_PROFILE_END )) /* check for end of profile */	  
	{					       /* not there yet, set up for the ramp */
	  ramp_transition_time = seconds + profile[ramp].duration; /* set the next transition time */
	  schedule_setpoint = setpoint;
	  degrees_per_second = 
	    ( (float)profile[ramp].end_temp /* temperature at end of ramp */
	      - (float)setpoint )  /* minus temperature now */
	    / (float)profile[ramp].duration; /* divided by ramp duration */

	  if ( setpoint < profile[ramp].end_temp)
	    status = RAMP_UP;
	  else if ( setpoint > profile[ramp].end_temp)
	    status = RAMP_DOWN;
	  else
	    status = HOLD;
	}

      else			/* end of profile... */
	{
	  schedule_setpoint = profile[ramp].end_temp;
	  degrees_per_second = 0;
	  ramp_transition_time = 0;
	  status = PROFILE_END;
	}
    }

  
  seconds++;
}  

