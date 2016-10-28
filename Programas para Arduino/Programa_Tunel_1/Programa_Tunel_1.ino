/****************************************************************************************************************************
* Proyecto : Tablero para monitoreo de Seris Sensores PT100 + Sensor de Huemdad y Temperatura Ambiente 
* Tarjeta  : Arduino Nano
* Módulo   : Bluetooth HC-06, Módulo MAX485
* Fecha    : 29/11/2014
* Autores  : Carlos A. Figueroa Castillo
*            Joe Pisconte Ryes 
* IDE      : 1.0.5-r2
* Librerías: TM1638.h,EEPROM.h,DHT.h
*
* Descripción: Lee seis sensores tipo PT100, luego almacena en variables globales.
*              Calcula la mediana de los 15 últimos valores capturados por los sensores PT100.
*              Muestra en displays valores de sensores PT100 en displays de 7 segmentos.
*              Lee humedad y temperatura ambiente con el sensor DHT22 y almacena valores en variables globales.
*              Esperar a través del puerto Serial la trama de datos del tablero más cercano.
*              Incorporar a la trama recibida los valores de los seis sensores PT100, DHT22 y luego enviar al 
*              siguiente tablero a través del MAX485.
*              Espera a través del puerto serial los valores de offset enviadas por el módulo bluetooth.
*
* Licencia:  BSD license. Todo este texto debe ser incluido en cualquier redistribución.
*            ElectroPro ha invertido tiempo y recursos para hacer que este programa sea open source, 
*            por favor ayúdanos a seguir dando soporte con la compra de nuestros productos!
*****************************************************************************************************************************/

#include <SPI.h>
#include <EEPROM.h>
#include <PlayingWithFusion_MAX31865.h>              // core library
#include <PlayingWithFusion_MAX31865_STRUCT.h>       // struct library
#include "DHT.h"        //Añadimos la libreria con la cual trabaja el sensor DHT22
#define DHTPIN 11        // Indicamos el pin donde conectaremos la patilla data de nuestro sensor
#define DHTTYPE DHT22   // Definimos el sensor DHT22
DHT dht(DHTPIN, DHTTYPE);  //Creamos un objeto dht con los


const int CS0_PIN = 3;
const int CS1_PIN = 4;
const int CS2_PIN = 5;
const int CS3_PIN = 6;
const int CS4_PIN = 7;
const int CS5_PIN = 8;
const int CS_NRF = 9;
const int CS_SD = 10;
const long tiempo1 = 250; // Retardo después de energizar el MAX31865
const long tiempo2 = 70; // Retardo después de configurar el MAX31865
const long tiempo3 = 50; // Retardo después de apagar el MAX31865

//----------------------------------------

long temperaturaAnterior0,temperaturaAnterior1,temperaturaAnterior2,temperaturaAnterior3,temperaturaAnterior4,temperaturaAnterior5;
boolean flat0 = false,flat1 = false,flat2 = false,flat3 = false,flat4 = false,flat5 = false;
float offset0 = 12.00, offset1 = 12.00, offset2 = 12.00, offset3 = 12.00, offset4 = 12.00, offset5 = 12.00 ;

//---------------------------------------

const int arraysize = 15;  // Cantidad de valores que va a tomar para tomar la media.  
float rangevalue0[arraysize],rangevalue1[arraysize],rangevalue2[arraysize],rangevalue3[arraysize],rangevalue4[arraysize],rangevalue5[arraysize];
float temperaturas0[arraysize],temperaturas1[arraysize],temperaturas2[arraysize],temperaturas3[arraysize],temperaturas4[arraysize],temperaturas5[arraysize];
int contador =0;

//----------------- Variables para procesar Trama Bluetooth --------------
const int NUMBER_OF_FIELDS = 2; // numero de campos esperados
int fieldIndex = 0; // define el campo que se está recibiendo
String values[NUMBER_OF_FIELDS]; // array conteniendo los valores de todos los campos

//<--------------------------------------------------------------------------------------------------------------- Quitsr este comentario al momento de grabar en el ARDUINO--------------
float temperatura = 0;
float humedad = 0;
float temperatura0 = 0; 
float temperatura1 = 0;  
float temperatura2 = 0;  
float temperatura3 = 0; 
float temperatura4 = 0; 
float temperatura5 = 0;  
/* 
//<--------------------------------------------------------------------------------------------------------------- Comentar estos valores al momento de grabar en el ARDUINO--------------
float temperatura = 0.51;
float humedad = 0.52;
float temperatura0 = 0.53;
float temperatura1 = 0.54;
float temperatura2 = 0.55;
float temperatura3 = 0.56;
float temperatura4 = 0.57;
float temperatura5 = 0.58;
//<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
const int NUMBER_OF_FIELDS_2 = 30; // numero de campos esperados
int fieldIndex_2 = 0; // el actual campo siendo recivido
String values_2[NUMBER_OF_FIELDS_2]; // array conteniendo los valores de todos los campos
String save_values_2[NUMBER_OF_FIELDS_2]; // array conteniendo los valores de todos los campos (copia de values_2)

PWFusion_MAX31865_RTD rtd_ch0(CS0_PIN);
PWFusion_MAX31865_RTD rtd_ch1(CS1_PIN);
PWFusion_MAX31865_RTD rtd_ch2(CS2_PIN);
PWFusion_MAX31865_RTD rtd_ch3(CS3_PIN);
PWFusion_MAX31865_RTD rtd_ch4(CS4_PIN);
PWFusion_MAX31865_RTD rtd_ch5(CS5_PIN);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  
  offset0 = ((double)EEPROM.read(0) / 10) - 12;
  offset1 = ((double)EEPROM.read(1) / 10) - 12;
  offset2 = ((double)EEPROM.read(2) / 10) - 12;
  offset3 = ((double)EEPROM.read(3) / 10) - 12;
  offset4 = ((double)EEPROM.read(4) / 10) - 12;
  offset5 = ((double)EEPROM.read(5) / 10) - 12;  
  
  pinMode(22, OUTPUT); //relay adc1 y 2
  pinMode(23, OUTPUT); //relay adc3 y 4
  pinMode(24, OUTPUT); //relay adc5 y 6
  // setup for the the SPI library:
  SPI.begin();                            // begin SPI
  SPI.setClockDivider(SPI_CLOCK_DIV16);   // SPI speed to SPI_CLOCK_DIV16 (1MHz)
  SPI.setDataMode(SPI_MODE3);             // MAX31865 works in MODE1 or MODE3
  
  
  
  // initalize the chip select pin
  pinMode(CS0_PIN, OUTPUT);
  pinMode(CS1_PIN, OUTPUT);  
  pinMode(CS2_PIN, OUTPUT);
  pinMode(CS3_PIN, OUTPUT);
  pinMode(CS4_PIN, OUTPUT);
  pinMode(CS5_PIN, OUTPUT);
  pinMode(CS_NRF, OUTPUT);
  pinMode(CS_SD, OUTPUT);

  digitalWrite(CS0_PIN, HIGH);
  digitalWrite(CS1_PIN, HIGH);
  digitalWrite(CS2_PIN, HIGH);
  digitalWrite(CS3_PIN, HIGH);
  digitalWrite(CS4_PIN, HIGH);
  digitalWrite(CS5_PIN, HIGH);
  digitalWrite(CS_NRF, HIGH);
  digitalWrite(CS_SD, HIGH);
  
  dht.begin();
  
}

void loop() 
{
  float numero2 = dht.readTemperature() ;  //Guarda la lectura de la temperatura en la variable float t
  temperatura = numero2;
  float numero1 = dht.readHumidity();  //Guarda la lectura de la humedad en la variable float h
  humedad = numero1;

  leerPT100();
  calcularMediana();
  mostrarendisplays();
  recibe_carga_envia_trama();
}

/*------------------------------------------------------------------------------
       Función que muestra los valores de la temperatura en los displays:
------------------------------------------------------------------------------*/
void mostrarendisplays(){
  // Comprobamos si lo que devuelve el sensor es valido, si no son temperaturas algo esta fallando
  if (isnan(temperatura0) || isnan(temperatura1) || isnan(temperatura2) || isnan(temperatura3) || isnan(temperatura4) || isnan(temperatura5)) // funcion que comprueba si son temperaturas las variables indicadas 
  {
      Serial.println("Fallo al leer el sensor DHT"); //Mostramos mensaje de fallo si no son temperaturas
  } else {
    
  char buffer1[4];
  char buffer2[4];
  char buffer3[4];
  char buffer4[4];
  char buffer5[4];
  char buffer6[4];
  
  dtostrf(temperatura0, 5, 1, buffer1);
  buffer1[3]=buffer1[4];
  Serial1.print(buffer1);
  Serial1.print(",");
  
  dtostrf(temperatura1, 5, 1, buffer2);
  buffer2[3]=buffer2[4];
  Serial1.print(buffer2);
  Serial1.print(",");
  
  dtostrf(temperatura2, 5, 1, buffer3);
  buffer3[3]=buffer3[4];
  Serial1.print(buffer3);
  Serial1.print(",");
  
  dtostrf(temperatura3, 5, 1, buffer4);
  buffer4[3]=buffer4[4];
  Serial1.print(buffer4);
  Serial1.print(",");
  
  dtostrf(temperatura4, 5, 1, buffer5);
  buffer5[3]=buffer5[4];
  Serial1.print(buffer5);
  Serial1.print(",");
  
  dtostrf(temperatura5, 5, 1, buffer6);
  buffer6[3]=buffer6[4];
  Serial1.print(buffer6);
  Serial1.print("$");
  }
}

void leerPT100(){
 
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 
  digitalWrite(24, HIGH);
  delay(tiempo1);
  
  rtd_ch0.MAX31865_config();
  rtd_ch1.MAX31865_config();
  delay(tiempo2);
  
  static struct var_max31865 RTD_CH0;
  static struct var_max31865 RTD_CH1;
   
  RTD_CH0.RTD_type = 1;                         // Tipo 1 = PT100
  RTD_CH1.RTD_type = 1;                         // Tipo 1 = PT100
  
  struct var_max31865 *rtd_ptr;
  rtd_ptr = &RTD_CH0;
  rtd_ch0.MAX31865_full_read(rtd_ptr);          // Update MAX31855 readings 
  
  rtd_ptr = &RTD_CH1;
  rtd_ch1.MAX31865_full_read(rtd_ptr);          // Update MAX31855 readings  
   
  //--------------------------- TEMP 0 ---------------------------------   
  temperatura0 = (double)RTD_CH0.rtd_res_raw;
  temperatura0 = (temperatura0/ 32) - 256.06;
  //--------------------------- TEMP 1 ---------------------------------   
  temperatura1 = (double)RTD_CH1.rtd_res_raw;
  temperatura1 = (temperatura1/ 32) - 256.06;
  //--------------------------------------------------------------------       
  
  digitalWrite(24, LOW); 
  delay(tiempo3);
  
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 
  digitalWrite(22, HIGH);
  delay(tiempo1);
  
  rtd_ch2.MAX31865_config();
  rtd_ch3.MAX31865_config();
  delay(tiempo2);
  
  static struct var_max31865 RTD_CH2;
  static struct var_max31865 RTD_CH3;
  
  RTD_CH2.RTD_type = 1;                         // Tipo 1 = PT100
  RTD_CH3.RTD_type = 1;                         // Tipo 1 = PT100
  
  rtd_ptr = &RTD_CH2;
  rtd_ch2.MAX31865_full_read(rtd_ptr);          // Update MAX31855 readings 
  
  rtd_ptr = &RTD_CH3;
  rtd_ch3.MAX31865_full_read(rtd_ptr);          // Update MAX31855 readings 

  //--------------------------- TEMP 2 ---------------------------------   
  temperatura2 = (double)RTD_CH2.rtd_res_raw;
  temperatura2 = (temperatura2/ 32) - 256;
  //--------------------------------------------------------------------      
  temperatura3 = (double)RTD_CH3.rtd_res_raw;
  temperatura3 = (temperatura3/ 32) - 256.06;
  //--------------------------------------------------------------------  
 
  digitalWrite(22, LOW);
  delay(tiempo3);
  
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  
 
  digitalWrite(23, HIGH);
  delay(tiempo1);
  
  rtd_ch4.MAX31865_config();
  rtd_ch5.MAX31865_config();
  delay(tiempo2);
  
  static struct var_max31865 RTD_CH4;
  static struct var_max31865 RTD_CH5;
  
  RTD_CH4.RTD_type = 1;                         // Tipo 1 = PT100
  RTD_CH5.RTD_type = 1;                         // Tipo 1 = PT100

  rtd_ptr = &RTD_CH4;
  rtd_ch4.MAX31865_full_read(rtd_ptr);          // Update MAX3185 readings 
  
  rtd_ptr = &RTD_CH5;
  rtd_ch5.MAX31865_full_read(rtd_ptr);          // Update MAX3185 readings 
  
  //--------------------------- TEMP 4 ---------------------------------   
  temperatura4 = (double)RTD_CH4.rtd_res_raw;
  temperatura4 = (temperatura4/ 32) - 256;
  //--------------------------- TEMP 5 ---------------------------------   
  temperatura5 = (double)RTD_CH5.rtd_res_raw;
  temperatura5 = (temperatura5/ 32) - 256.06;
  //--------------------------------------------------------------------  
  
  digitalWrite(23, LOW);
  delay(tiempo3);
 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 
}

void calcularMediana(){  
  
  temperaturas0[contador] = temperatura0;
  temperaturas1[contador] = temperatura1;
  temperaturas2[contador] = temperatura2;
  temperaturas3[contador] = temperatura3;
  temperaturas4[contador] = temperatura4;
  temperaturas5[contador] = temperatura5;
  
  //---------------- Mediana TEMP 0------------------
  if (contador == arraysize-1){
  //----------- temperatura 0 ----------------------  
    for (int i =0; i < arraysize;i++){
    rangevalue0[i]=temperaturas0[i];
    rangevalue1[i]=temperaturas1[i];
    rangevalue2[i]=temperaturas2[i];
    rangevalue3[i]=temperaturas3[i];
    rangevalue4[i]=temperaturas4[i];
    rangevalue5[i]=temperaturas5[i];
    }
    for (int i = 1; i < arraysize; ++i)
     { 
       float j = rangevalue0[i];
       int k;
       for (k = i - 1; (k >= 0) && (j < rangevalue0[k]); k--){ 
         rangevalue0[k + 1] = rangevalue0[k];
       }
       rangevalue0[k + 1] = j;
     }
     int midpoint = contador/2;    //El punto medio de la matriz es el valor medio
     temperatura0 = rangevalue0[midpoint]- offset0;
  //----------------- temperatura 1 -----------------   
    for (int i = 1; i < arraysize; ++i)
     { 
       float j = rangevalue1[i];
       int k;
       for (k = i - 1; (k >= 0) && (j < rangevalue1[k]); k--){ 
         rangevalue1[k + 1] = rangevalue1[k];
       }
       rangevalue1[k + 1] = j;
     }
//     midpoint = contador/2;    //El punto medio de la matriz es el valor medio
     temperatura1 = rangevalue1[midpoint]- offset1;
  //----------------- temperatura 2 -----------------   
    for (int i = 1; i < arraysize; ++i)
     { 
       float j = rangevalue2[i];
       int k;
       for (k = i - 1; (k >= 0) && (j < rangevalue2[k]); k--){ 
         rangevalue2[k + 1] = rangevalue2[k];
       }
       rangevalue2[k + 1] = j;
     }
//     midpoint = contador/2;    //El punto medio de la matriz es el valor medio
     temperatura2 = rangevalue2[midpoint]- offset2;
  //----------------- temperatura 3 -----------------   
    for (int i = 1; i < arraysize; ++i)
     { 
       float j = rangevalue3[i];
       int k;
       for (k = i - 1; (k >= 0) && (j < rangevalue3[k]); k--){ 
         rangevalue3[k + 1] = rangevalue3[k];
       }
       rangevalue3[k + 1] = j;
     }
//     midpoint = contador/2;    //El punto medio de la matriz es el valor medio
     temperatura3 = rangevalue3[midpoint]- offset3;
  //----------------- temperatura 4 -----------------   
    for (int i = 1; i < arraysize; ++i)
     { 
       float j = rangevalue4[i];
       int k;
       for (k = i - 1; (k >= 0) && (j < rangevalue4[k]); k--){ 
         rangevalue4[k + 1] = rangevalue4[k];
       }
       rangevalue4[k + 1] = j;
     }
//     midpoint = contador/2;    //El punto medio de la matriz es el valor medio
     temperatura4 = rangevalue4[midpoint]- offset4;
  //----------------- temperatura 5 -----------------   
    for (int i = 1; i < arraysize; ++i)
     { 
       float j = rangevalue5[i];
       int k;
       for (k = i - 1; (k >= 0) && (j < rangevalue5[k]); k--){ 
         rangevalue5[k + 1] = rangevalue5[k];
       }
       rangevalue5[k + 1] = j;
     }
//     midpoint = contador/2;    //El punto medio de la matriz es el valor medio
     temperatura5 = rangevalue5[midpoint]- offset5;
  }
  
  if (contador < arraysize-1){
    contador = contador +1;
    Serial.println(contador);
  }else{
    contador = arraysize-1;
    for (int i = 0; i < arraysize-1; ++i){
     temperaturas0[i]=temperaturas0[i+1];
     temperaturas1[i]=temperaturas1[i+1];
     temperaturas2[i]=temperaturas2[i+1];
     temperaturas3[i]=temperaturas3[i+1];
     temperaturas4[i]=temperaturas4[i+1];
     temperaturas5[i]=temperaturas5[i+1];
    }
    Serial.println(contador);
  }  
}

/*----------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////        RECIBE, CARGA Y ENVÍA LA TRAMA      \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*---------------------------------------------------------------------------------------------------------------------------*/
void recibe_carga_envia_trama()
{
   while( Serial2.available())
   {
     char ch = Serial2.read();
     if(ch >= '0' && ch <= '9' || ch == '-' || ch == '.' || ch == ' ') // is this an ascii digit between 0 and 9?
     {
           //si, acumula el valor
           values_2[fieldIndex_2] = values_2[fieldIndex_2] + ch; 
     } else if (ch == ',') // si detecta coma, mueve al siguiente campo
     {
           if(fieldIndex_2 < NUMBER_OF_FIELDS_2 - 1)
           fieldIndex_2++; // increment field index
     } else{                   
           //cualquier caracter diferente (a un número, punto o coma) termina la adquisición
           //ese caracter sería el "$"
           for(int i=0; i <= fieldIndex_2; i++)
           { 
               save_values_2[i] = values_2[i];
               values_2[i] = 0; // setea los valores a 0 para almacenar nuevo dato
           }    
           fieldIndex_2 = 0; // listo para empezar de nuevo
         
           /////////////////////////////////////////////////////////////////////////////////////////////////
           // Guardar mis datos en el array
           /////////////////////////////////////////////////////////////////////////////////////////////////
           char buffer1[7];
           dtostrf(temperatura, 7, 2, buffer1);  // Usar un buffer para convertir de float a string 
           save_values_2[0] = buffer1;
            
           char buffer2[7];
           dtostrf(humedad, 7, 2, buffer2);  
           save_values_2[1] = buffer2;
            
           char buffer3[7];
           dtostrf(temperatura0, 7, 2, buffer3);  
           save_values_2[2] = buffer3;
            
           char buffer4[7];
           dtostrf(temperatura1, 7, 2, buffer4);  
           save_values_2[3] = buffer4;
            
           char buffer5[7];
           dtostrf(temperatura2, 7, 2, buffer5);  
           save_values_2[4] = buffer5;
            
           char buffer6[7];
           dtostrf(temperatura3, 7, 2, buffer6);  
           save_values_2[5] = buffer6;
            
           char buffer7[7];
           dtostrf(temperatura4, 7, 2, buffer7);  
           save_values_2[6] = buffer7;
            
           char buffer8[7];
           dtostrf(temperatura5, 7, 2, buffer8);  
           save_values_2[7] = buffer8;
    
          /////////////////////////////////////////////////////////////////////////////////////////////////
          // Hay que enviar todos los 30 campos, desde 0 al 29            
          /////////////////////////////////////////////////////////////////////////////////////////////////
          for(int j = 0; j < NUMBER_OF_FIELDS_2; j++)
          {
                 Serial2.print(save_values_2[j]);   
                 
                 if(j == 29) { Serial2.print('$');}
                 else        { Serial2.print(',');}                     
          }
                                                                                                        
          //////// Hay que limpiar el array para guardar los nuevos datos, en este caso son 30
          for(int k=0;k<NUMBER_OF_FIELDS_2;k++)
          {
                 save_values_2[k] = 0; 
          }   
     }// Fin del if-else if - else         
   }// Fin de if(Serial2.availale())
}// Fin de recibe_carga_envia_trama()

void serialEvent1(){
  while (Serial1.available()) {
     char ch = Serial1.read();
     if(ch >= '0' && ch <= '6'){ //acepta caracteres entre el '0' y el '2'
       //si, acumula el valor
       values[fieldIndex] = values[fieldIndex] + ch; 
     }else if (ch == ','){ // si detecta coma, mueve al siguiente campo
         if(fieldIndex < NUMBER_OF_FIELDS-1) //se esperan 6 campos
         fieldIndex++; // incrementa field index
     }else{ 
       //cualquier caracter diferente a un número o coma termina la adquisición
       if (fieldIndex==NUMBER_OF_FIELDS-1){ //verifica si se han recibido los 6 campos esperados
         int i =0;
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
