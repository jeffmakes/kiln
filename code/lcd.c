#include "lcd.h"

#define set(reg,bit) ((reg) |= 1 << (bit))
#define clear(reg,bit) ((reg) &= ~(1 << (bit)))

void delay()	       
{
  uint32_t i = 0;
  for (i=0; i<50; i++);
} 

void lcd_strobe()
{
  delay();
  en_hi();
  delay();
  en_lo();
  delay();
}

void lcd_send_cmd(uint8_t cmd)
{
  rs_lo();
  delay();
  P1OUT = 0x0f & (cmd >> 4);
  lcd_strobe();
  P1OUT = 0x0f & cmd;
  lcd_strobe();
  rs_hi();
}

void lcd_send_char(uint8_t character)
{
  rs_hi();
  delay();
  P1OUT = 0x0f & (character >> 4);
  lcd_strobe();
  P1OUT = 0x0f & character;
  lcd_strobe();
}

void lcd_cursor_to(uint8_t x, uint8_t y)
{
  /* bounds */
  if (x > 15)
    x = 0;
  if (y > 1)
    y = 0;

  lcd_send_cmd(LCD_SET_DDRAM_ADDR | x | (y<<6) );
}

void lcd_print_num(uint16_t num, uint8_t digits)
{
  static char decbuffer[6];
  uint8_t i = 0;

  if (digits>5)
    return;

  /* null termination */
  decbuffer[digits] = '\0';

  for (i=0; i<digits; i++)
    {
      decbuffer[(digits - 1) - i] = '0' + (num % 10);
      num /= 10;
    }
  /* space padding */
  for (i=0; i<(digits-1); i++)
    {
      if (decbuffer[i] == '0')
	decbuffer[i] = ' ';
      else
	break;
    }

  lcd_print_string(decbuffer);
}

void lcd_print_string(char *string)
{
  uint8_t i = 0;
  while (string[i] != '\0')
    {
      lcd_send_char(string[i]);
      i++;
    }
}

void lcd_init()
{
  uint16_t i = 0;
  /* set control signals to be outputs */
  P3DIR |= RS | RW | EN;
  /* set data lines to be outputs */
  P1DIR |= 0x0f;

  rw_lo();
  rs_lo();
  P1OUT = 0x03;
  for (i=0;i<120;i++) delay();	/* wait >15ms */
  lcd_strobe();
  for (i=0;i<40;i++) delay();	/* wait >15ms */
  lcd_strobe();
  for (i=0;i<2;i++) delay();	/* wait >100us */
  lcd_strobe();

  P1OUT = 0x02; 		/* enter 4-bit transfer mode */
  lcd_strobe();

  lcd_send_cmd(LCD_FUNCTION_SET);
  lcd_send_cmd(LCD_DISPLAY_OFF);
  lcd_send_cmd(LCD_CLEAR);
  lcd_send_cmd(LCD_ENTRYMODE_INC);
  
  lcd_send_cmd(LCD_DISPLAY_ON);
  lcd_send_cmd(LCD_HOME);
}
