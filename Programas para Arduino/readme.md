##Importante !!
Para el correcto funcionamiento de los programas, es necesario ampliar el Buffer de recepción del puerto Serial a 256 caracteres.

Para ello modificar:
C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\HardwareSerial.h

Los programas ha sido compilados en Arduino 1.0.5 r2

Los tableros de los túneles #1, #2 y #3 incorporan dos Arduinos: un arduino Mega2560 y un Arduino Nano (ver Diagramas de Tableros para mayor referencia).
El Arduino Mega lee el valor de los sensores PT100 y se los envía al Arduino Nano por puerto serial para que este último los muestre en 06 módulos de displays.
