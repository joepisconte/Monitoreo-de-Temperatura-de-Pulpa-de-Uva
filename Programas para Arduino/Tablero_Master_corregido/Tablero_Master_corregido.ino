/****************************************************************************************************************************
* Proyecto : Escritura en una memoria SD desde Arduino(MASTER) creando el nombre del archivo con datos de un RTC DS3231SN 
*            y se almacenarán los datos enviados desde 6 Arduino(SLAVE'S) con diversos sensores y que utilizan comunicación
*            serial con MAX485.
* Tarjeta  : Arduino MEGA ADK (El Arduino UNO perdía los datos al agregar la librería SD.h)
* Módulo   : MicroSD Card Adapter de CATALEX v1.0 y módulo de RTC de alta precisión ds3231ns
* Fecha    : 28/11/2014
* Autor    : Carlos A. Figueroa Castillo
* IDE      : 1.0.5-r2
* Librerías: SD.h, Wire.h, RTC.h, SPI.h
* Descripción: En la memoria buscar un archivo y si no existe crear uno con un nombre formado por la fecha, además 
*              agregar la leyenda, mensajes, etc. que forman el reporte. Si el archivo existe sólo agregar los datos.
*              Cada grupo de datos guardados van precedidos de la hora en que se hizo la medición.
*              Los SLAVE'S recogen la data de los sensores y luego se la envían (uno por uno) al MASTER previa petición 
*              de éste. El MASTER es el encargado de guardar la data con el tiempo en que se hicieron las mediciones 
*              (cada minuto) dentro del archivo (uno por día) que se guarda en el memoria SD.
*
* Licencia: Para libre distribución.
*
*      TUNEL#3 --- CAMARA#3 --- CAMARA#2 --- TUNEL#2 --- CAMARA#1 --- TUNEL#1 ------------------------ MASTER
*                                                                                                        ^
*                                                                                        Estamos aquí >> ^
****************************************************************************************************************************/
// Archivos de cabecera:
#include <SD.h>
#include <Wire.h>
#include <RTC.h>
#include <SPI.h>

// Selector de SLAVE para el módulo SD ya que es un SLAVE que se comunica por el bus SPI
#define CS_PIN  4

// Variables utiliazadas en la recepción y almacenamiento de la trama
const int NUMBER_OF_FIELDS_2 = 30;           // Numero de campos esperados
int       fieldIndex_2 = 0;                  // El actual campo siendo recivido
String    values_2[NUMBER_OF_FIELDS_2];      // Array conteniendo los valores de todos los campos
String    save_values_2[NUMBER_OF_FIELDS_2]; // Array para salvar los datos recibidos

// Flag para saber si se detectó memoria
boolean flag_memoria_SD = 0;

// Se crea un objeto del tipo File
          File MyFile;

// Constructor de la clase RTC
          RTC rtc(DST_ON);
String nombre_del_archivo;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////// SETUP //////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
      // Inicializa la comunicación serial
      Serial.begin(115200);  // Usa USART0 para depuración
      Serial1.begin(9600); // Usa USART1 para recibir la trama de datos
      
      //Serial.println("Inciando TABLERO MASTER - ELECTROPRO 2014");
      
      // ADVERTENCIA! Asegúrate de que el RTC está con la hora y fecha correcta.
      // En este código no se configura el RTC.
      
      // Inicializa la memoria SD
      INICIALIZA_MEMORIA();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////// LOOP ///////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
      // Recibe la trama de datos desde TUNEL#1... con todos sus datos y los datos que le pasaron los demás tableros.
      RECIBIR_TRAMA();  
      enviar_archivo_a_PC();      
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------------------------------------------------------------
* Función        : INICIALIZA_MEMORIA
* Parámetros     : ninguno
* Valor devuelto : ninguno
* Descripción    : Verifica que exista comunicación con la memoria SD a través del bus SPI.
----------------------------------------------------------------------------------------------------------------------------*/

void INICIALIZA_MEMORIA()
{
  //Serial.println("Inicializando memoria SD...");  
        pinMode(SS,OUTPUT);  // Configurar el pin SS como salida incluso si no esta siendo usado debido a SD.h      
        // Verificar que está presente la memoria y que puede ser inicializada
        if (!SD.begin(CS_PIN))
        {      //Serial.println("ADVERTENCIA: memoria SD no encontrada"); 
               //Serial.println("... los datos recibidos no se guardan.");
               flag_memoria_SD = 0;     
        } 
        else 
        {      //Serial.println("Memoria SD inicializada correctamente."); 
               flag_memoria_SD = 1;
        }  
}

/*---------------------------------------------------------------------------------------------------------------------------
* Función        : RECIBIR_TRAMA
* Parámetros     : ninguno
* Valor devuelto : ninguno
* Descripción    : Alamacena todos los valores recibidos de la trama de datos y los guarda temporalmente en save_value_2[].
---------------------------------------------------------------------------------------------------------------------------*/
void RECIBIR_TRAMA()
{
     while( Serial1.available())
     {
         char ch = Serial1.read();
         //Serial.print(ch);
         if(ch >= '0' && ch <= '9' || ch == '-' || ch == '.' || ch == ' '|| ch == 'N' || ch == 'A') // is this an ascii digit between 0 and 9?
         {
             //Sí... entonces acumula el valor.
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
                           // Limpiar el array donde se almacena los datos
                           save_values_2[i] = values_2[i];
                           values_2[i] = 0; // Setea los valores a 0 para almacenar nuevo dato
                     }
                     
                      fieldIndex_2 = 0; // Listo para empezar de nuevo                   
               /*
                                       /////////////////////////////////////////////////////////////////////////
                                       // DEPURACIÓN - esto se puede comentar o eliminar
                                       /////////////////////////////////////////////////////////////////////////
                                       // Hay que enviar todos los 30 campos, desde 0 al 29            
                                       /////////////////////////////////////////////////////////////////////////
                                       for(int j = 0; j < NUMBER_OF_FIELDS_2; j++)
                                       {
                                              Serial.print(save_values_2[j]);   
                                           
                                              if(j == 29) { Serial.print('$'); Serial.println("");}
                                              else        { Serial.print(',');}                     
                                       } 
               */
                                       
                      // Antes de guardar los datos en la memoria, verificar que todos los campos de la variable save_values_2
                      // contienen datos. Cada dato es de 7 caracteres.
                      boolean flag_datos_correctos = true;
                      int z = 0;
                      
                      // Mientras halla datos correctos y la cuenta sea menor a 30
                      while( (flag_datos_correctos == true) && (z < NUMBER_OF_FIELDS_2) )
                      {                          
                          if( save_values_2[z].length() != 7 )  // ¿La longitud del campo 0,1,2...29 es distinto de 7?
                          {  // Sí!! ... entonces avisa que hay al menos un campo con datos mal
                                flag_datos_correctos = false;
                          }
                          
                          // Aumenta el contador
                          z++;
                      }
                      
                      if( flag_datos_correctos == true)  //¿Todos los datos son correctos?
                      {    // Sí!!! ... entonces...
                            GUARDAR_DATOS_EN_MEMORIA();                                               
                      }
                      
         }// Fin de else(... cualquier caracter diferente... termina la adquisición)
     }// Fin de if(Serial1.available())  
}// Fin de RECIBIR_TRAMA()

/*---------------------------------------------------------------------------------------------------------------------------
* Función        : GUARDAR_DATOS_EN_MEMORIA
* Parámetros     : ninguno
* Valor devuelto : ninguno
* Descripción    : Toma los datos ya almacenados en save_value_2[] y los guarda en la memoria SD. También crea el reporte 
*                  si todavía no existe un reporte (archivo.csv) con la fecha actual.
----------------------------------------------------------------------------------------------------------------------------*/

void GUARDAR_DATOS_EN_MEMORIA()
{// Variables locales
   int i;
   
   if(flag_memoria_SD == 1) // Si se ha inicializado exitosamente la memoria, entonces continúa...
   {
            // Se crea un objeto del tipo Data
            Data d = rtc.getData();  // La función getData() toma los datos del RTC 
            
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Cadena donde se almacena el nombre del archivo según la fecha del RTC
            /*String temp_filename = String(d.day);
                   temp_filename.concat("-");
                   temp_filename.concat(d.month);
                   temp_filename.concat("-");  
                   temp_filename.concat(d.year - 2000);  // Para obtener sólo los dos últimos dígitos
                   temp_filename.concat(".csv");
            */
             String temp_filename = "";
            
                   if(d.day < 10) {temp_filename.concat("0");}
                   temp_filename.concat(d.day);                   
                   temp_filename.concat("-");
                   
                   if(d.month < 10) {temp_filename.concat("0");}
                   temp_filename.concat(d.month);                   
                   temp_filename.concat("-");
                   
                   temp_filename.concat(d.year - 2000);  // Para obtener sólo los dos últimos dígitos
                   temp_filename.concat(".csv");
                   
            
            // Cadena donde se almacena la hora
            String cadenahora = "";
            
                     if(d.hour24h < 10) { cadenahora.concat("0");}  
                   cadenahora.concat(d.hour24h);       
                   cadenahora.concat(":");
                   
                     if(d.minutes < 10) { cadenahora.concat("0");} 
                   cadenahora.concat(d.minutes);
                   cadenahora.concat(":");
                   
                     if(d.seconds < 10)  {cadenahora.concat("0");}
                   cadenahora.concat(d.seconds);
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Volver a asignar el nombre a una variable que acepte la librería
              // Crear una cadena, la longitud sale de los 12 elementos que acepta el nombre del archivo + 1 del caracter nulo
                char filename[13];
            
              // Pasar los elementos de temp_filename a filename
                temp_filename.toCharArray(filename, sizeof(filename));
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
            // Preguntar si no existe el archivo de la fecha actual
            if(!SD.exists(filename)) // Si no existe el archivo...
            {   // Crear y abrir el archivo  
                MyFile = SD.open(filename,FILE_WRITE);   
              
                    if(MyFile)  // Si está abierto el archivo...
                    {        // Armar la cadena donde se almacena el título del archivo según la fecha del RTC
                             String TITULO = String(d.day);  // día
                                    TITULO.concat("-");
                                    TITULO.concat(d.month);  // mes
                                    TITULO.concat("-");  
                                    TITULO.concat(d.year);   // año con 4 dígitos
                      
                             // ... escribir los mensajes, leyendas y datos iniciales
                             MyFile.print(",,,,,,");
                             MyFile.print(TITULO);
                             MyFile.println("");
                             
                             MyFile.println("LEYENDA:,PT100T1[X],ES EL SENSOR DE TEMPERATURA PT100 EN EL TUNEL #1 Y 'X' ES EL NUMERO DEL SENSOR 1 AL 6");
                             MyFile.println(",PT100T2[X],ES EL SENSOR DE TEMPERATURA PT100 EN EL TUNEL #2 Y 'X' ES EL NUMERO DEL SENSOR 1 AL 6");
                             MyFile.println(",PT100T3[X],ES EL SENSOR DE TEMPERATURA PT100 EN EL TUNEL #3 Y 'X' ES EL NUMERO DEL SENSOR 1 AL 6");
                             
                             /*
                             MyFile.println(",PT100C#1,ES EL SENSOR DE TEMPERATURA PT100 EN LA CAMARA #1");
                             MyFile.println(",HUMEDC#1,ES EL SENSOR DE HUMEDAD EN LA CAMARA #1");
                             MyFile.println(",PT100C#2,ES EL SENSOR DE TEMPERATURA PT100 EN LA CAMARA #2");
                             MyFile.println(",HUMEDC#2,ES EL SENSOR DE HUMEDAD EN LA CAMARA #2");
                             MyFile.println(",PT100C#3,ES EL SENSOR DE TEMPERATURA PT100 EN LA CAMARA #3");
                             MyFile.println(",HUMEDC#3,ES EL SENSOR DE HUMEDAD EN LA CAMARA #3");
                             */
                             
                             MyFile.println(",TEMPERATURA, ES LA TEMPERATURA AMBIENTE");
                             MyFile.println(",HUMEDAD, ES LA HUMEDAD AMBIENTE");
                             MyFile.println("");
                             MyFile.println(",,,,,TUNEL,#1,,,,,,,,TUNEL,#2,,,,,,,,TUNEL,#3,,,,,CAMARA,#1,,CAMARA,#2,,CAMARA,#3");
                             MyFile.println("HORA,,TEMPERATURA,HUMEDAD,PT100T1[1],PT100T1[2],PT100T1[3],PT100T1[4],PT100T1[5],PT100T1[6],,TEMPERATURA,HUMEDAD,PT100T2[1],PT100T2[2],PT100T2[3],PT100T2[4],PT100T2[5],PT100T2[6],,TEMPERATURA,HUMEDAD,PT100T3[1],PT100T3[2],TP100T3[3],PT100T3[4],PT100T3[5],PT100T3[6],,TEMPERATURA,HUMEDAD,,TEMPERATURA,HUMEDAD,,TEMPERATURA,HUMEDAD");
                             // OJO: sólo son rótulos, el array empieza de 0, pero se entiende mejor de esta forma
                            // ... luego guardar los datos de los sensores precedidos del tiempo
                            //////// Hora ///////////////////////////////////////////////////////////////////////////////////
                            MyFile.print(cadenahora);
                            MyFile.print(",,"); 
                            
                            //////// Sensores del TUNEL#1 ///////////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[0]... [7]
                            for(i=0;i<=7;i++) 
                            {   MyFile.print(save_values_2[i]);
                                MyFile.print(","); 
                            }
                            MyFile.print(","); // Se agrega una sola coma porque ya sobraba una del for
                            
                            //////// Sensores del TUNEL#2 ///////////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[10]... [17]               
                            for(i=10;i<=17;i++) 
                            {   MyFile.print(save_values_2[i]); 
                                MyFile.print(",");
                            }
                            MyFile.print(","); 
                            
                            //////// Sensores del TUNEL#3 ///////////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[22]... [29]
                            for(i=22;i<=29;i++) 
                            {   MyFile.print(save_values_2[i]);
                                MyFile.print(","); 
                            }
                            MyFile.print(",");
                            
                            //////// Sensores de la  CAMARA#1 ///////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[8] y [9]                                 
                            MyFile.print(save_values_2[8]);
                            MyFile.print(",");
                            MyFile.print(save_values_2[9]);
                            MyFile.print(",,");
                            
                            //////// Sensores de la  CAMARA#2 ///////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[18] y [19]                                 
                            MyFile.print(save_values_2[18]);
                            MyFile.print(",");
                            MyFile.print(save_values_2[19]);
                            MyFile.print(",,");
                            
                            //////// Sensores de la  CAMARA#3 ///////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[20] y [21]                                 
                            MyFile.print(save_values_2[20]);
                            MyFile.print(",");
                            MyFile.print(save_values_2[21]);
                            //-------------------------------
                            MyFile.println("");  // Genera un salto de línea
                            
                            // Cierra el archivo
                            MyFile.close();
                            
                            // Para verificar
                            //Serial.print("Se ha creado el archivo: ");
                            //Serial.print(filename); 
                            //Serial.println("");
                            //Serial.println("Y los datos se han guardado satisfactoriamente.");
                    }
                    else // Si no se puede abrir el archivo
                    {
                        //Serial.println("Error al intentar abrir el archivo");
                    } 
            }
            else // En caso de que ya exista un archivo con el nombre de la fecha actual, solamente agrega los datos de los sensores precedidos del tiempo
            {
                  MyFile = SD.open(filename,FILE_WRITE);  // Abrir el archivo
                  
                  //////// Hora ///////////////////////////////////////////////////////////////////////////////////
                            MyFile.print(cadenahora);
                            MyFile.print(",,"); 
                            
                            //////// Sensores del TUNEL#1 ///////////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[0]... [7]
                            for(i=0;i<=7;i++) 
                            {   MyFile.print(save_values_2[i]);
                                MyFile.print(","); 
                            }
                            MyFile.print(","); // Se agrega una sola coma porque ya sobraba una del for
                            
                            //////// Sensores del TUNEL#2 ///////////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[10]... [17]               
                            for(i=10;i<=17;i++) 
                            {   MyFile.print(save_values_2[i]); 
                                MyFile.print(",");
                            }
                            MyFile.print(","); 
                            
                            //////// Sensores del TUNEL#3 ///////////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[22]... [29]
                            for(i=22;i<=29;i++) 
                            {   MyFile.print(save_values_2[i]);
                                MyFile.print(","); 
                            }
                            MyFile.print(",");
                            
                            //////// Sensores de la  CAMARA#1 ///////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[8] y [9]                                 
                            MyFile.print(save_values_2[8]);
                            MyFile.print(",");
                            MyFile.print(save_values_2[9]);
                            MyFile.print(",,");
                            
                            //////// Sensores de la  CAMARA#2 ///////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[18] y [19]                                 
                            MyFile.print(save_values_2[18]);
                            MyFile.print(",");
                            MyFile.print(save_values_2[19]);
                            MyFile.print(",,");
                            
                            //////// Sensores de la  CAMARA#3 ///////////////////////////////////////////////////////////////
                            /////////////////////////////// Están en save_values_2[20] y [21]                                 
                            MyFile.print(save_values_2[20]);
                            MyFile.print(",");
                            MyFile.print(save_values_2[21]);
                            //-------------------------------
                            MyFile.println("");  // Genera un salto de línea
                            
                            // Cierra el archivo
                            MyFile.close();
                            
                            // Para verificar
                            //Serial.println("Los datos se han guardado satisfactoriamente.");
            }// Fin de else (... si ya existe un archivo con la fecha actual) 
         
            //////// Hay que limpiar el array para guardar los nuevos datos, en este caso son 30
            for(i=0;i<NUMBER_OF_FIELDS_2;i++)
            {
                   save_values_2[i] = 0; 
            }
            
   }// Fin de if(flag_memoria_SD == 1)       
}// Fin de GUARDAR_DATOS_EN_MEMORIA().

/*---------------------------------------------------------------------------------------------------------------------------
* Función        : enviar_archivo_a_PC()
* Parámetros     : ninguno
* Valor devuelto : ninguno
* Descripción    : Espera la fecha en formato dd-mm-aa seguido del caracter "#", luego busca dentro de la memoria SD, un archivo
*                  con ese mismo nombre y devuelve su contenido.
----------------------------------------------------------------------------------------------------------------------------*/

void enviar_archivo_a_PC(){
  while (Serial.available()) {
         char ch = Serial.read();
         if (ch=='@'){
             Serial.print("@");
         }else{    
             if(ch >= '0' && ch <= '9' || ch == '-'){ // verifica si es un caracter permitido
                 //Sí... entonces acumula el valor.
                         nombre_del_archivo = nombre_del_archivo + ch; 
             }else{                   
                         //cualquier caracter, diferente a un número o a un guion, termina la adquisición
                         //ese caracter sería el "#"
                         if (nombre_del_archivo.length()==8){ //verifico si el nombre tiene el tamaño correcto

                              nombre_del_archivo = nombre_del_archivo + ".csv";
                              
                              char filename[13];
                              // Pasar los elementos de temp_filename a filename
                              nombre_del_archivo.toCharArray(filename, sizeof(filename));
                              
                              File registro_solicitado = SD.open(filename);
                            
                              // si el archivo esta disponible, entonces lo lee:
                              if (registro_solicitado) {
                                while (registro_solicitado.available()) {
                                  Serial.write(registro_solicitado.read());
                                }
                                registro_solicitado.close();
                                Serial.write("|");
                              }else { // caso contrario muestra mensaje
                               //envía el caracter especial "&" para indicar que no se ha podido abrir el archivo
                                Serial.println("&");
                              } 
                           
                         }
                        nombre_del_archivo=""; 
             }// Fin de else(... cualquier caracter diferente... termina la adquisición)   
         }
  }
}

