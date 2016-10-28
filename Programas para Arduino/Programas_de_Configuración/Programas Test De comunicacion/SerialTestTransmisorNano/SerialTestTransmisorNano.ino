#include <TM1638.h>
#include <EEPROM.h>
#include "DHT.h"        //Añadimos la libreria con la cual trabaja el sensor DHT22
#define DHTPIN 8        // Indicamos el pin donde conectaremos la patilla data de nuestro sensor
#define DHTTYPE DHT22   // Definimos el sensor DHT22
DHT dht(DHTPIN, DHTTYPE);  //Creamos un objeto dht con los
#include <SoftwareSerial.h>
SoftwareSerial SerialBT(10, 11); // RX, TX

// define data pin 5, clock pin 4 y strobe pin 3
TM1638 module1(2, 3, 5);
TM1638 module2(2, 3, 4);
float valoractual;
float valoranterior;

//----------------- Variables para procesar Trama Bluetooth ---------------------------
const int NUMBER_OF_FIELDS = 2; // numero de campos esperados
int fieldIndex = 0; // define el campo que se está recibiendo
String values[NUMBER_OF_FIELDS]; // array conteniendo los valores de todos los campos
float offset0, offset1, offset2, offset3, offset4, offset5;

//-------------------------------------------------------------------------------------
//----------------- Variables para procesar Trama Bluetooth ---------------------------
//-------------------------------------------------------------------------------------
const int NUMBER_OF_FIELDS_2 = 30; // numero de campos esperados
int fieldIndex_2 = 0; // el actual campo siendo recivido
String values_2[NUMBER_OF_FIELDS_2]; // array conteniendo los valores de todos los campos
////////// OJO//////////
String save_values_2[NUMBER_OF_FIELDS_2]; // array para salvar los datos recibidos

//<----------------------------------------------------------------------------------------------------------------------------------------- quitar este comentario antes de cargar programa en el Arduino
float TEMPERATURA = 0.00;
float HUMEDAD = 0.00;  
/* 
///////// LAS VARIABLES DE TEMPERATURA Y HUMEDAD //////////////// <--------------------------------------------------------------------------- comentar este antes de cargar programa en el Arduino
float TEMPERATURA = 0.40;
float HUMEDAD = 0.41;                                            
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void setup()
{
    Serial.begin(9600);  // Se usa RX0 y TX0
    module1.setDisplayToString("--------");
    module2.setDisplayToString("--------");
    
    SerialBT.begin(9600);
    dht.begin();
  
    offset0 = ((double)EEPROM.read(0) / 10) - 12;
    offset1 = ((double)EEPROM.read(1) / 10) - 12;
    offset2 = ((double)EEPROM.read(2) / 10) - 12;
    offset3 = ((double)EEPROM.read(3) / 10) - 12;
    offset4 = ((double)EEPROM.read(4) / 10) - 12;
    offset5 = ((double)EEPROM.read(5) / 10) - 12;    
}

void loop()
{

Serial.println("OK");
}

/*----------------------------------------------------------------------------------------------------------------------------
///////////////////////////     Lee la temperatura y humedad del sensor DHT22, luego la    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////   convierte a string y finalemente la muestra en los displays de 7 segmentos   \\\\\\\\\\\\\\\\\\\\\\\\\\\
*---------------------------------------------------------------------------------------------------------------------------*/

void mostrarTemperaturayHumedad(){
  
  float numero2 = dht.readTemperature() - offset0;  //Guarda la lectura de la temperatura en la variable float t
  TEMPERATURA = numero2;
  float numero1 = dht.readHumidity() - offset1;  //Guarda la lectura de la humedad en la variable float h
  HUMEDAD = numero1;
  // Comprobamos si lo que devuelve el sensor es valido, si no son numeros algo esta fallando
  if (isnan(numero1) || isnan(numero2)){ // funcion que comprueba si son numeros las variables indicadas 
  } else {  
  char buffer1[4];
  char buffer2[4];
  dtostrf(numero1, 5, 1, buffer1);
  buffer1[3]=buffer1[4];
  dtostrf(numero2, 5, 1, buffer2);
  buffer2[3]=buffer2[4];  
  //muestra en los display de 7 segmentos  
  module1.setDisplayToString(buffer1,32);
  module2.setDisplayToString(buffer2,32);
  }
}

/*----------------------------------------------------------------------------------------------------------------------------
//////////////////////////////     RECIBE, CARGA Y ENVÍA LA TRAMA AL SIGUIENTE TABLERO      \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*---------------------------------------------------------------------------------------------------------------------------*/

void recibe_carga_envia_trama()
{
   if( Serial.available())
   {
     char ch = Serial.read();
     if(ch >= '0' && ch <= '9' || ch == '-' || ch == '.' || ch == ' ') // is this an ascii digit between 0 and 9?
     {
           //si, acumula el valor
           values_2[fieldIndex_2] = values_2[fieldIndex_2] + ch; 
     } else if (ch == ',') // si detecta coma, mueve al siguiente campo
     {
           if(fieldIndex_2 < NUMBER_OF_FIELDS_2-1)
           fieldIndex_2++; // increment field index
     } else{                   
           //cualquier caracter diferente (a un número, punto o coma) termina la adquisición
           //ese caracter sería el "$"
           for(int i=0; i <= fieldIndex_2; i++)
           { 
               //----- Visualización de Datos en Display de 4 dígitos y 7 segmentos ----
               //Serial.println(values_2[i]); // OJO: solo para ver, pero debe ser serial1
               //-----------------------------------------------------------------------
               // Limpiar el array donde se almacena los datos
               save_values_2[i] = values_2[i];
               values_2[i] = 0; // setea los valores a 0 para almacenar nuevo dato
           }
           //Serial.println("Trama Recibida");      
           fieldIndex_2 = 0; // listo para empezar de nuevo
         
           /////////////////////////////////////////////////////////////////////////////////////////////////
           // Guardar mis datos en el array
           /////////////////////////////////////////////////////////////////////////////////////////////////
           char buffer1[7];
           dtostrf(TEMPERATURA, 7, 2, buffer1);  // Usar un buffer para convertir a string un float
           save_values_2[8] = buffer1;            
            
           char buffer2[7];
           dtostrf(HUMEDAD, 7, 2, buffer2);
           save_values_2[9] = buffer2;            
              
           /////////////////////////////////////////////////////////////////////////////////////////////////
           // Hay que enviar todos los 30 campos, desde 0 al 29            
           /////////////////////////////////////////////////////////////////////////////////////////////////
           for(int j = 0; j < NUMBER_OF_FIELDS_2; j++)
           {
                  Serial.print(save_values_2[j]);   
                   
                  if(j == 29) { Serial.print('$');}
                  else        { Serial.print(',');}                     
           }                                                
                                                                                                
           //////// Hay que limpiar el array para guardar los nuevos datos, en este caso son 30
           for(int k=0;k<NUMBER_OF_FIELDS_2;k++)
           {
                  save_values_2[k] = 0; 
           }         
     }// Fin de else
  }// Fin de if(Serial.available())  
}// Fin de recibe_carga_envia_trama()

/*----------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////   LEE LOS DATOS DE OFFSET RECIBIDOS DEL MODULO BT     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*---------------------------------------------------------------------------------------------------------------------------*/

void leerBT(){
  while (SerialBT.available()) {
     char ch = SerialBT.read();
     SerialBT.print(ch);
     if(ch >= '0' && ch <= '6'){ //acepta caracteres entre el '0' y el '6'
       //si, acumula el valor
       values[fieldIndex] = values[fieldIndex] + ch; 
     }else if (ch == ','){ // si detecta coma, mueve al siguiente campo
         if(fieldIndex < NUMBER_OF_FIELDS-1) //se esperan 6 campos
         fieldIndex++; // incrementa field index
     }else{ 
       //cualquier caracter diferente a un número o coma termina la adquisición
       if (fieldIndex==NUMBER_OF_FIELDS-1){ //verifica si se han recibido los 6 campos esperados
         int i =0;
          SerialBT.print(values[i]);
               switch (values[i].toInt()){
                 case 0:
                     if (values[i+1].toInt() == 0){
                      EEPROM.write(0, 120);
                      EEPROM.write(1, 120);
                      EEPROM.write(2, 120);
                      EEPROM.write(3, 120);
                      EEPROM.write(4, 120);
                      EEPROM.write(5, 120);
                      offset0 = 0.0;
                      offset1 = 0.0;
                      offset2 = 0.0;
                      offset3 = 0.0;
                      offset4 = 0.0;
                      offset5 = 0.0;
                     }
                 break;
                 case 1:
                     if (values[i+1].toInt() == 0){
                       offset0 += 0.1;
                       int val = (int)(offset0*10+120);
                       EEPROM.write(0, val);
                     }else{
                       offset0 -= 0.1;
                       int val = (int)(offset0*10+120);
                       EEPROM.write(0, val);
                     }
                 break;
                 case 2:
                     if (values[i+1].toInt() == 0){
                       offset1 += 0.1;
                       int val = (int)(offset1*10+120);
                       EEPROM.write(1, val);
                     }else{
                       offset1 -= 0.1;
                       int val = (int)(offset1*10+120);
                       EEPROM.write(1, val);
                     }
                 break;
                 case 3:
                     if (values[i+1].toInt() == 0){
                       offset2 += 0.1;
                       int val = (int)(offset2*10+120);
                       EEPROM.write(2, val);
                     }else{
                       offset2 -= 0.1;
                       int val = (int)(offset2*10+120);
                       EEPROM.write(2, val);
                     }
                 break;
                 case 4:
                     if (values[i+1].toInt() == 0){
                       offset3 += 0.1;
                       int val = (int)(offset3*10+120);
                       EEPROM.write(3, val);
                     }else{
                       offset3 -= 0.1;
                       int val = (int)(offset3*10+120);
                       EEPROM.write(3, val);
                     }
                 break;
                 case 5:
                     if (values[i+1].toInt() == 0){
                       offset4 += 0.1;
                       int val = (int)(offset4*10+120);
                       EEPROM.write(4, val);
                     }else{
                       offset4 -= 0.1;
                       int val = (int)(offset4*10+120);
                       EEPROM.write(4, val);
                     }
                 break;
                 case 6:
                     if (values[i+1].toInt() == 0){
                       offset5 += 0.1;
                       int val = (int)(offset5*10+120);
                       
                       EEPROM.write(5, val);
                     }else{
                       offset5 -= 0.1;
                       int val = (int)(offset5*10+120);
                       EEPROM.write(5, val);
                     }
                 break;
               } 
                   //-----------------------------------------------
           values[i] = 0; // setea los valores a 0 para almacenar nuevo dato
           values[i+1] = 0; // setea los valores a 0 para almacenar nuevo dato
       }
       else{
         for(int i=0; i <= fieldIndex; i++){ //caso contrario borra todos los campos
           values[i] = 0; // setea los valores a 0 para almacenar nuevo dato
         }
       }
       fieldIndex = 0; // listo para empezar de nuevo
    }
  } 
}


