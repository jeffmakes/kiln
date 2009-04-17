#ifndef __SCHEDULER_H
#define __SCHEDULER_H

typedef enum {MODE_CHOICE_MANUAL = 0, MODE_CHOICE_PROFILE, MANUAL_MODE, PROFILE_WAIT_START, PROFILE_START_COUNTDOWN_SET, PROFILE_START_COUNTDOWN, HOLD, RAMP_UP, RAMP_DOWN, PROFILE_END} profile_status_t;

typedef struct 
{
  uint16_t end_temp ;		/* Temperature at end of ramp */
  uint16_t duration ;		/* Duration in seconds */
  uint8_t flags;		/* Flags - eg. end of profile */
} ramp_t;

extern uint32_t seconds;
extern uint8_t ramp;
extern profile_status_t status;
extern ramp_t profile[];
extern uint32_t ramp_transition_time;
void scheduler_init(void);

/* Ramp flags */
#define FLAG_PROFILE_START (1<<0)
#define FLAG_PROFILE_END (1<<1)



#endif
