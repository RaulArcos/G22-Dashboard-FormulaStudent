//Librerías
  #include <Ticker.h>
  #include <Wire.h>  // Lo trae Arduino IDE
  #include <SPI.h>
  #include <MaxMatrix.h> 
  #include <stdlib.h>
  #include "mcp_can.h"
  #include <ArduinoSort.h>

//Inicialización de pines
  int data = 8;    // DIN pin del módulo MAX7219
  int load = 10;    // CS pin del módulo MAX7219
  int clock = 9;  // CLK pin del módulo MAX7219
  int maxInUse = 1;    // Cuantos módulos MAX7219 se estan usando
  MaxMatrix m(data, load, clock, maxInUse);
  byte buffer[10];
  int pin_leds_revoluciones[3] = {4,5,6};
  int pin_led_rojo = 6;
  const int SPI_CS_PIN = 7; //PIN CS (53 mega // 9 uno)

//Datos recibidos por el CAN BUS (sólo se necesitará un stream)
  unsigned int revoluciones; // > 255
  unsigned int temp_agua; // < 255
  unsigned int marcha; // < 255
    
//Números de la matriz
  PROGMEM unsigned char const GRID[] = {
   
  8, 8, B00000000, B01111110, B00000100, B00001000, B00010000 ,B00100000, B01111110, B00000000, // 0
  8, 8, B00000000, B00000000, B10000010, B11111111, B10000000, B00000000, B00000000, B00000000, //1
  8, 8, B00000000, B10000010, B11000001, B10100001, B10010001, B10001110, B00000000, B00000000, // 2
  8, 8, B00000000, B01000010, B10000001, B10001001, B10001001, B01110110, B00000000, B00000000, // 3
  8, 8, B00000000, B00110000, B00101000, B00100100, B00100010, B11111111, B00000000, B00000000, // 4
  8, 8, B00000000, B01001111, B10001001, B10001001, B10001001, B01110001, B00000000, B00000000, // 5
  8, 8, B00000000, B01111110, B10001001, B10001001, B10001001, B01110010, B00000000, B00000000, // 6
  8, 8, B01111110, B00001010, B00001010, B00000010, B00111100, B01000010, B01010010, B00110010, //FG
  };

//Informamos a la librería CAN el PIN CS
  MCP_CAN CAN(SPI_CS_PIN);   

//Funciones necesarias
//Función FUN_REVOLUCIONES: Recibe las revoluciones y enciende los leds correspondientes en función al rango de revoluciones que sea
void fun_revoluciones (unsigned int revoluciones){
  if (revoluciones <= 4000){
    for (int i = 0; i < 2; i++)
      digitalWrite(pin_leds_revoluciones[i], HIGH);
  } else if (revoluciones > 4000 && revoluciones < 6000) {
    for (int i = 0; i < 4; i++)
      digitalWrite(pin_leds_revoluciones[i], HIGH);
  } else {
    for (int i = 0; i < 6; i++)
      digitalWrite(pin_leds_revoluciones[i], HIGH);
  }
}

//Función FUN_TEMPAGUA: recibe la temperatura del agua, e ilumina un led de peligro si sobrepasa los 90ºC y lo hace parpadear si supera los 100ºC.
void fun_tempAgua (unsigned int temp_agua){
    if (temp_agua >= 100){
      digitalWrite(pin_led_rojo, LOW);
      delay (500);
      digitalWrite(pin_led_rojo, HIGH);
    } else if (temp_agua > 90){
        digitalWrite(pin_led_rojo, HIGH);
    }
}

//Función FUN_MARCHA: recibe la marcha e imprime el número en la matriz.
void fun_marcha (unsigned int marcha){
  switch (marcha){
    case 0: 
     m.clear();
     memcpy_P(buffer, GRID, 10);
     m.writeSprite(0, 0, buffer);    
    case 1:
     m.clear();
     memcpy_P(buffer, GRID + 10, 10);
     m.writeSprite(0, 0, buffer);    
    case 2:
     m.clear();
     memcpy_P(buffer, GRID + 20, 10);
     m.writeSprite(0, 0, buffer);    
    case 3:
     m.clear();
     memcpy_P(buffer, GRID + 30, 10);
     m.writeSprite(0, 0, buffer);    
    case 4:
     m.clear();
     memcpy_P(buffer, GRID + 40, 10);
     m.writeSprite(0, 0, buffer);    
    case 5:
     m.clear();
     memcpy_P(buffer, GRID + 50, 10);
     m.writeSprite(0, 0, buffer);    
    case 6:
     m.clear();
     memcpy_P(buffer, GRID + 60, 10);
    m.writeSprite(0, 0, buffer);    
  }
}
    
//Función CANBUS: Lee los datos del CANBUS y realiza las distintas funciones propuestas.
void CanBus(){
  //Limpiamos len y buf.
  unsigned char len = 0;
  unsigned char buf[8];

  //Leemos CAN LINEA 1 
  do{
    CAN.readMsgBuf(&len, buf); 
  }while(buf[0]==1);
  revoluciones = buf[1] * 256 + buf[2]; //Dato Revoluciones
  temp_agua = buf[3] * 256 + buf[4]; //Dato Temperatura del agua
  marcha = buf[5] * 256 + buf[6]; //Dato Marcha actual
  //Llamada a las respetivas funciones
  fun_revoluciones (revoluciones);
  fun_tempAgua (temp_agua);
  fun_marcha (marcha);
}

//Función START: Inicia el CAN BUS para funcionar.
void Start(){  
  //INICIA EL CANBUS
      for (int i = 0; i < 3; i++)
      digitalWrite(pin_leds_revoluciones[i], HIGH);
  
  while (CAN_OK != CAN.begin(CAN_250KBPS)) {           //PONER ECU A 125 SI NO, NO VA.
    Serial.println("CAN BUS Shield Fallo Inicio");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
}

//Funciones básicas de arduino (main)
void setup() {
  //Definimos el Serial en el que hablará el arduino.
  Serial.begin(115200);

  //Definimos LEDS que vamos a usar, como salida.
  pinMode(pin_led_rojo, OUTPUT); //LED PELIGRO (Rojo)
  for(int i = 0; i < 6; i++)
    pinMode(pin_leds_revoluciones[i], OUTPUT);

  //Iniciamos la MATRIZ y ajustamos su brillo.
  m.init(); 
  m.setIntensity(15); // Intensidad luminosa de la matriz

  //Llamamos a la función START (Aquí va todo el inicio del programa de CAN y todo)
}

void loop() {
  //Bucle infinito de la función CanBus(), para actualizar los datos recibidos por la ECU.
  CanBus();
}
