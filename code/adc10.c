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

#define ADC_BUFFSZ 32
uint16_t adcbuffer[ADC_BUFFSZ];

void adc10_init( void )
{
  ADC10CTL0 = SREF_0 	/* Use VCC and VSS as the references */
    | ADC10SHT_3 /* 64 x ADC10CLKs
			8 us */
    /* ADC10SR = 0 -- Support 200 ksps sampling (TODO: maybe this can be set) */
    /* REFOUT = 0 -- Reference output off */
    /* REFBURST = 0 -- Reference buffer on continuously (TODO) */
    | MSC		/* Move onto the next conversion after the previous*/
    | REF2_5V
    | REFON         /* Use 2.5V reference */
    | ADC10ON;	/* Peripheral on */
  
  ADC10CTL1 = /* Select the channel later... */
    SHS_0		/* ADC10SC is the sample-and-hold selector */
    /* ADC10DF = 0 -- Straight binary format */
    /* ISSH = 0 -- No inversion on the s&h signal */
    | ADC10DIV_7	/* Divide clock by 8 (1MHz) */
    | ADC10SSEL_2 /* MCLK as source */
    | CONSEQ_0 	/* Single channel, single conversion */
    | INCH_3;			/* Channel 3 */
  
  
  ADC10AE0 |= (1<<3);	/* Enable analog input 3 */
  
  ADC10DTC0 |= ADC10CT; /* DTC Not used. This makes it continuous */

  /* Enable & start the ADC: */
  ADC10CTL0 |= (ENC | ADC10SC); 
}

void adc10_update_average( void )
{
  static uint16_t i = 0;
  uint8_t j = 0;
  uint32_t average = 0;

  adcbuffer[i] = ADC10MEM;	/* store new result in average array */

  average = 0;
  for (j=0;j<ADC_BUFFSZ;j++)	/* compute average */
    average += adcbuffer[j];
  average /= ADC_BUFFSZ;

  thermocouple_temp = tempconvert[average]; /* perform temperature conversion */

  if ((++i) == ADC_BUFFSZ)	/* loop round to beginning of buffer if we're at the end */
    i = 0;
}

void adc10_poll( void )
{
  if (ADC10CTL0 & ADC10IFG)	/* new result ready? */
    {
      ADC10CTL0 &= ~(ADC10IFG);	/* clear flag */
      adc10_update_average();	
      ADC10CTL0 |= ADC10SC;	/* start the next conversion */
    }
}
