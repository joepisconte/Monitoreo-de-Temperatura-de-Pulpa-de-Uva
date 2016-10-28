void setup()
{
      Serial.begin(115200);  // Usa USART0 para depuración
      Serial2.begin(9600); // Usa USART1 para recibir la trama de datos
}

void loop()
{
      // Recibe la trama de datos desde TUNEL#1... con todos sus datos y los datos que le pasaron los demás tableros.
      RECIBIR_TRAMA();  
}

void RECIBIR_TRAMA()
{
     if( Serial2.available())
     {
         char ch = Serial2.read();
         Serial.print(ch);                
     }
}


