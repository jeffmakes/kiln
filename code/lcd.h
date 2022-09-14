#ifndef __LCD_H
#define __LCD_H
#include "device.h"
#include <stdint.h>

#define RS (1<<0)
#define RW (1<<6)
#define EN (1<<7)

#define rs_lo() ((P3OUT &= ~RS))
#define rs_hi() ((P3OUT |= RS))
#define rw_lo() ((P3OUT &= ~RW))
#define rw_hi() ((P3OUT |= RW))
#define en_lo() ((P3OUT &= ~EN))
#define en_hi() ((P3OUT |= EN))
 
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_DISPLAY_ON 0x0c
#define LCD_DISPLAY_OFF 0x08
//#define LCD_DISPLAY_ON 0x0f
#define LCD_ENTRYMODE_INC 0x06
#define LCD_SHIFT_CURSOR_RIGHT 0x14
#define LCD_SET_DDRAM_ADDR 0x80
#define LCD_FUNCTION_SET 0x28


void delay();
void lcd_strobe();
void lcd_send_cmd(uint8_t cmd);
void lcd_send_char(uint8_t character);
void lcd_init();
void lcd_cursor_to(uint8_t x, uint8_t y);
void lcd_print_string(char *string);
void lcd_print_num(uint16_t num, uint8_t digits);

#endif	/* __LCD_H */
