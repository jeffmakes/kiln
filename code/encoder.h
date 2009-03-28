#ifndef __ENCODER_H
#define __ENCODER_H
#include <signal.h>
#include "device.h"

typedef enum{CLOCKWISE, ANTICLOCKWISE} enc_dir_t;

extern uint8_t encoderpos;
extern uint8_t encoder_button;

void encoder_change(enc_dir_t dir);
void encoder_init();
interrupt (PORT2_VECTOR) encoder_isr(void);




#endif	/* __ENCODER_H */
