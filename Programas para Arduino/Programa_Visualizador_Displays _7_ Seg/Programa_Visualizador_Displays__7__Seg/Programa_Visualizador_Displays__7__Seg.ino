/* Recepción de varios datos separados por comas y su visialización en displays de 7 segmentos 
 * con el integrado TM1638
 * Se espera un mensaje en el formato: -12.5, 15.6, -0.7, 26.0,  3.6, 27.1$
 * Este programa requiere un $(dolar) para indicar el final de la trama
 * Este programa ha sido desarrollado para grabado en los arduino nano de los tableros de 
 * Túneles de enfrimiento de pulpa de uva #1, #2 y #3 (ver Diagramas de Tableros para mayor referencia).
 */
 
#include <TM1638.h>
// define pines de data, clock, strobe
TM1638 module1(2, 3, 4);
TM1638 module2(2, 3, 5);
TM1638 module3(2, 3, 6);
TM1638 module4(2, 3, 7);
TM1638 module5(2, 3, 8);
TM1638 module6(2, 3, 9);

const int NUMBER_OF_FIELDS = 6; // numero de campos esperados
int fieldIndex = 0; // el actual campo siendo recivido
String values[NUMBER_OF_FIELDS]; // array conteniendo los valores de todos los campos
void setup()
{
  module1.setDisplayToString("--------");
  module2.setDisplayToString("--------");
  module3.setDisplayToString("--------");
  
  module4.setDisplayToString("--------");
  module5.setDisplayToString("--------");
  module6.setDisplayToString("--------");
  
 Serial.begin(9600); // inicializa el puerto serial a 115200 baudios
 //Serial.println("ok");
}
void loop()
{
 if( Serial.available())
 {
 char ch = Serial.read();
 if(ch >= '0' && ch <= '9' || ch == '-' || ch == '.' || ch == ' ') // is this an ascii digit between 0 and 9?
 {
 //si, acumula el valor
 values[fieldIndex] = values[fieldIndex] + ch; 
 } else if (ch == ',') // si detecta coma, mueve al siguiente campo
 {
 if(fieldIndex < NUMBER_OF_FIELDS-1)
   fieldIndex++; // increment field index
 }
 else
 { 
   //cualquier caracter diferente (a un número, punto o coma) termina la adquisición
   //ese caracter sería el "$"
   for(int i=0; i <= fieldIndex; i++)
   { 
     //----- Visualización de Datos en Display de 4 dígitos y 7 segmentos ----
     char buffer[6];
     values[i].toCharArray(buffer, 6);
     buffer[3]=buffer[4];
     switch (i) {
       case 0:
         module2.setDisplayToString(buffer,32);
         break;
       case 1:
         module1.setDisplayToString(buffer,32);
         break;
       case 2:
         module4.setDisplayToString(buffer,32);
         break;
       case 3:
         module3.setDisplayToString(buffer,32);
         break;
       case 4:
         module6.setDisplayToString(buffer,32);
         break;
       case 5:
         module5.setDisplayToString(buffer,32);
         break;
     }
     //-----------------------------------------------------------------------
     values[i] = ""; // reinicia el array para almacenar nuevos valores
   }
     fieldIndex = 0; // listo para empezar de nuevo
   }
 }
}
