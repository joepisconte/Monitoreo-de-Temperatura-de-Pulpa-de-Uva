/****************************************************************************************************************************
* Proyecto :   Programa para Leer memoria EEPROM y que por puerto serial envío valores de offset de los sensores.
* Tarjeta  :   Arduino Nano
* Fecha    :   29/11/2014
* Autores  :   Joe Pisconte Ryes 
* IDE      :   1.0.5-r2
* Librerías:   EEPROM.h
*
* Descripción: Este programa permite leer los seis primeros bytes de la memoria EEPROM y luego envía los valores de offset a 
*              través del puerto Serial
*
* Licencia:    BSD license. Todo este texto debe ser incluido en cualquier redistribución.
*              ElectroPro ha invertido tiempo y recursos para hacer que este programa sea open source, 
*              por favor ayúdanos a seguir dando soporte con la compra de nuestros productos!
*****************************************************************************************************************************/

#include <EEPROM.h>

float offset0,offset1,offset2,offset3,offset4,offset5;

void setup()
{
   Serial.begin(9600);  // Se usa RX0 y TX0  
}

void loop()
{
  Leer_EEPROM_Enviar_Serial();
}


void Leer_EEPROM_Enviar_Serial(){
  
    offset0 = ((double)EEPROM.read(0) / 10) - 12;
    offset1 = ((double)EEPROM.read(1) / 10) - 12;
    offset2 = ((double)EEPROM.read(2) / 10) - 12;
    offset3 = ((double)EEPROM.read(3) / 10) - 12;
    offset4 = ((double)EEPROM.read(4) / 10) - 12;
    offset5 = ((double)EEPROM.read(5) / 10) - 12;  
    
    Serial.println("------------ Valores de offset almacenados en memoria EEPROM ------------");
    Serial.print("offset0 = ");
    Serial.println(offset0);
    Serial.print("offset1 = ");
    Serial.println(offset1);
    Serial.print("offset2 = ");
    Serial.println(offset2);
    Serial.print("offset3 = ");
    Serial.println(offset3);
    Serial.print("offset4 = ");
    Serial.println(offset4);
    Serial.print("offset5 = ");
    Serial.println(offset5);
    Serial.println("");
    
}

