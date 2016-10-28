// CAMBIAR LA FECHA Y HORA AL RTC
// GRABAR EL PROGRAMA UNA VEZ, INMEDIATAMENTE DESPUÉS GRABAR OTRO QUE NO ALTERE AL RELOJ.

#include <Wire.h>
#include <RTC.h>

RTC rtc(DST_ON);

void setup()
{
    //Serial.println("Estableciendo fecha y hora en el RTC...");
    rtc.setDateTime( 2016, 10, 20, 10, 38, 00 ); // Año, mes, día del mes, horas, minutos y segundos 
}

void loop()
{
  
}
