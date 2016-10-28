## Monitoreo de Temperatura de Pulpa de Uva con Sensores PT100, MAX31865 y el Arduino Mega2260

Proyecto que permite monitorear la temperatura de la pulpa de uva de exportación en túneles de enfrimiento con sensor PT100.
Los datos de la temperatura se almacenan en una momoria SD como un Datalogger.

Este proyecto cuenta con tres tipos de tableros (Tablero de Túnel, Tablero de Cámara, Tablero Master):
**Tablero de Túnel**: Tiene seis sensore PT100 tipo lapicero que se insertan en una baya de uvas y capturan la temperatura hasta que llegue a los 0° Celsius, en total hay 03 tableros de este tipo.
**Tablero de Cámara**: Tiene un sensor DHT22, y solo tiene la función de monitorear la temperatura y humedad del ambiente de las cámaras donde se almacena la uva.
**Tablero Master**: Es el tablero que recolecta la data de los tres tableros de túneles y de los tres tablerós de Cámaras. Esta informacion es recolectada a través de una trama serial enviada por RS485 y almacenada en una memoria SD como archivo *.csv.

## Proceso

1. La uva es clasificada por tamaño y empaquetada en cajas de 5kg para posteriormente ser puestos en un palé o tarima.
2. Los palés son introducidos en túneles de enfriamiento para bajar la temperatura de la pulpa de uva hasta 0 grados celsius, este proceso suele tardar 10 horas. Los túneles tienen una capacidad máxima de 4 palés.
3. Cuando el palé de uva se encuentra a 0 grados centígrados es trasladado a almacenes calibrados a temperatura de 0 grados centigrados para mantener la temperatura de la uva mientras se espera el momento de embarque en el contenedor de exportación. A estos almacenes se les denomina Cámaras de enfriamiento.
4. Finalmente lo palés son puestos en contenedores refrigerados y llevados hasta el puerto de Paita para trasladarlos por barco al destino final.

## Diagramas 

En el repositorio encontrará una carpeta con los disgramas de conexión de los tableros de túneles, cámaras y el master.

## Instalación

Este proyecto fue desarrollado en noviembre del 2014 con el compilador Arduino 1.0.5 r2 el cual se encuentra para descargar dentro de este repositorio.

## Fotos

Las fotos mostradas corresponden a la calibración realizar el 21 de Octubre del 2016.

## Programa para PC

Programa compatible con Windows 7 y 8, desarrollado en Visual Basic.
EL programa busca entre todos los puertos seriales disponibles en la PC hasta encontrar el Tablero Master.
Si no encuentra el tablero master, muestra un mensaje de error y se cierra.
El código fuente de este programa se ha perdido y no ha podido ser recuperado.

## Desarrolladores

- Carlos Augusto Figueroa Castillo
- Joe Pisconte Reyes

## License

BSD license.
