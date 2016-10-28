void setup()
{
      Serial.begin(9600);  
}

void loop()
{
     if( Serial.available())
     {
         char ch = Serial.read();
         Serial.print(ch);                
     }
}
