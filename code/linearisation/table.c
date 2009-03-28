#include <stdio.h>
#include <math.h>
#include <stdint.h>

/* Nadc = 1023 * (Vin / 3.3) */
/* AD595 gain = 66.486 */

const float G = 66.486;

const float Dlo[10] = 
  {
    0.000000E+00, 
    2.508355E+01, 
    7.860106E-02, 
    -2.503131E-01, 
    8.315270E-02,
    -1.228034E-02,
    9.804036E-04,
    -4.413030E-05,
    1.057734E-06,
    -1.052755E-08
  };

const float Dhi[10] = 
  {
    -1.318058E+02,
    4.830222E+01,
    -1.646031E+00,
    5.464731E-02,
    -9.650715E-04,
    8.802193E-06,
    -3.110810E-08,
    0.000000E+00,
    0.000000E+00,
    0.000000E+00,
  };

/* convert voltage (mV) to temperature using the thermocouple polynomial */
float temperaturepoly(float voltage)
{
  float temp = 0;
  char i;
  if (voltage < 20.644)
    for (i=0;i<10;i++)
      {
	temp += Dlo[i] * pow(voltage,i);
      }
  else
    for (i=0;i<10;i++)
      {
	temp += Dhi[i] * pow(voltage,i);
      }
  return temp;
}


int main(void)
{
  uint32_t adcval = 0;
  float Vadc = 0;		/* ADC input voltage, mV */
  float Vtherm = 0;		/* Thermocouple voltage, mV */
  float temp = 0;		/* Temperature */

  printf("tempconvert[1024] = { \n");

  for (adcval = 0; adcval<1024; adcval++)
    {
      Vadc = (float)adcval / 1023 * 3.3 * 1000;
      Vtherm = Vadc / G;
      temp = temperaturepoly(Vtherm);

      printf("%4.0f,\n", rint(temp));
      //printf("%i %f %f %f %f \n", adcval, Vadc, Vtherm, temp, rint(temp) );
    }
	 
  printf("}\n");

  return 0;

}
