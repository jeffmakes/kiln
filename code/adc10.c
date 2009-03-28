#include "adc10.h"
#include "thermocouple.h"
#include <signal.h>
#include <stdint.h>
#include "device.h"

/* Disable the ADC */
#define adc10_dis() do { ADC10CTL0 &= ~ENC; } while (0)

/* Select a channel (0 <= x <= 7) */
#define adc10_set_channel(x) do { ADC10CTL1 &= ~INCH_15; \
		ADC10CTL1 |= x << 12; } while (0)

#define ADC_BUFFSZ 256
uint16_t adcbuffer[ADC_BUFFSZ];

void adc10_init( void )
{
  ADC10CTL0 = SREF_0 	/* Use VCC and VSS as the references */
    | ADC10SHT_DIV64 /* 64 x ADC10CLKs
			8 us */
    /* ADC10SR = 0 -- Support 200 ksps sampling (TODO: maybe this can be set) */
    /* REFOUT = 0 -- Reference output off */
    /* REFBURST = 0 -- Reference buffer on continuously (TODO) */
    | MSC		/* Move onto the next conversion after the previous*/
    | REF2_5V
    | REFON         /* Use 2.5V reference */
    | ADC10ON	/* Peripheral on */
    | ADC10IE;	/* Interrupt enabled */
  
  ADC10CTL1 = /* Select the channel later... */
    SHS_0		/* ADC10SC is the sample-and-hold selector */
    /* ADC10DF = 0 -- Straight binary format */
    /* ISSH = 0 -- No inversion on the s&h signal */
    | ADC10DIV_7	/* Divide clock by 8 (1MHz) */
    | ADC10SSEL_MCLK
    | CONSEQ_2 	/* Repeat single channel */
    | INCH_A3;			/* Channel 3 */
  
  
  ADC10AE |= (1<<3);	/* Enable analog input 3 */
  
  ADC10DTC0 |= ADC10CT; /* DTC Not used. This makes it continuous */

  /* Start the conversion: */
  ADC10CTL0 |= (ENC | ADC10SC);
}

interrupt (ADC10_VECTOR) adc10_isr( void )
{
  static uint16_t i = 0;
  uint32_t average = 0;
  adcbuffer[i] = ADC10MEM;
  i++;
  if (i==ADC_BUFFSZ)			/* every ADC_BUFFSZ results, calculate the average */
    {    
      average = 0;
      for (i=0;i<ADC_BUFFSZ;i++)
	average += adcbuffer[i];
      average /= ADC_BUFFSZ;
      thermocouple_temp = tempconvert[average];
      i = 0;
    }
}
