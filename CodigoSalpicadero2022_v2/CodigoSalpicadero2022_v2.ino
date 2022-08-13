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
  int pinRev[3] = {6,5,4};
  int pinRed[2] = {2,3};
  const int SPI_CS_PIN = 7; //PIN CS (53 mega // 9 uno)

//Datos recibidos por el CAN BUS (sólo se necesitará un stream)
  unsigned int rev; // > 255
  int tempAgua; // < 255
  unsigned int marcha; // < 255
  unsigned int tps;
    
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

  PROGMEM unsigned char const TPS[] = {
   8, 8, B11111111, B00000000, B00000000, B00000000, B00000000 ,B00000000, B00000000, B00000000, // 1
   8, 8, B11111111, B11111111, B00000000, B00000000, B00000000 ,B00000000, B00000000, B00000000, // 5
   8, 8, B11111111, B11111111, B11111111, B00000000, B00000000 ,B00000000, B00000000, B00000000, // 10
   8, 8, B11111111, B11111111, B11111111, B11111111, B00000000 ,B00000000, B00000000, B00000000, // 20
   8, 8, B11111111, B11111111, B11111111, B11111111, B11111111 ,B00000000, B00000000, B00000000, // 30
   8, 8, B11111111, B11111111, B11111111, B11111111, B11111111 ,B11111111, B00000000, B00000000, // 50
   8, 8, B11111111, B11111111, B11111111, B11111111, B11111111 ,B11111111, B11111111, B00000000, // 70
   8, 8, B11111111, B11111111, B11111111, B11111111, B11111111 ,B11111111, B11111111, B11111111, // 100
  };
  

//Informamos a la librería CAN el PIN CS
  MCP_CAN CAN(SPI_CS_PIN);   

void tpsshow(unsigned int tps){
     if(tps == 0){
      m.clear();
     } 
     if(tps > 0 && tps < 5){
      m.clear();
      memcpy_P(buffer, TPS , 10);
      m.writeSprite(0, 0, buffer); 
     }
      if(tps > 5 && tps < 10){
      m.clear();
      memcpy_P(buffer, TPS + 10, 10);
      m.writeSprite(0, 0, buffer); 
     }
      if(tps > 10 && tps < 20){
      m.clear();
      memcpy_P(buffer, TPS + 20, 10);
      m.writeSprite(0, 0, buffer); 
     }
      if(tps > 20 && tps < 30){
      m.clear();
      memcpy_P(buffer, TPS + 30, 10);
      m.writeSprite(0, 0, buffer); 
     }
      if(tps > 30 && tps < 50){
      m.clear();
      memcpy_P(buffer, TPS + 40, 10);
      m.writeSprite(0, 0, buffer); 
     }
      if(tps > 50 && tps < 70){
      m.clear();
      memcpy_P(buffer, TPS + 50, 10);
      m.writeSprite(0, 0, buffer); 
     }
      if(tps > 70 && tps < 90){
      m.clear();
      memcpy_P(buffer, TPS + 60, 10);
      m.writeSprite(0, 0, buffer); 
     }
      if(tps > 90){
      m.clear();
      memcpy_P(buffer, TPS + 70, 10);
      m.writeSprite(0, 0, buffer); 
     }
}


//Funciones necesarias
//Función FUN_REVOLUCIONES: Recibe las revoluciones y enciende los leds correspondientes en función al rango de revoluciones que sea
void revs (unsigned int rev){

    if(rev == 0){
        digitalWrite(pinRev[0], HIGH);
        digitalWrite(pinRev[2], LOW);
        delay(100);
        digitalWrite(pinRev[2], HIGH);
        digitalWrite(pinRev[0], LOW);
        delay(100);
    }

    if(rev < 4000 && rev > 0){
        digitalWrite(pinRev[0], LOW);
        digitalWrite(pinRev[1], LOW);
        digitalWrite(pinRev[2], LOW);
    }
    if(rev > 4000 && rev < 6000){
        digitalWrite(pinRev[0], HIGH);
        digitalWrite(pinRev[1], LOW);
        digitalWrite(pinRev[2], LOW);
    }
    if(rev > 6000 && rev < 8000){
        digitalWrite(pinRev[0], HIGH);
        digitalWrite(pinRev[1], HIGH);
        digitalWrite(pinRev[2], LOW);
    }
    if(rev > 8000){
        digitalWrite(pinRev[0], HIGH);
        digitalWrite(pinRev[1], HIGH);
        digitalWrite(pinRev[2], HIGH);
    }
        
    if(rev > 8800){
        for(size_t i = 0; i < 3; i++)
            digitalWrite(pinRev[i], HIGH);
        delay(50);
        for(size_t i = 0; i < 3; i++)
             digitalWrite(pinRev[i], LOW);
        delay(50);
    }   
}

//Función FUN_TEMPAGUA: recibe la temperatura del agua, e ilumina un led de peligro si sobrepasa los 90ºC y lo hace parpadear si supera los 100ºC.
void tempAguaWarning (unsigned int tempAgua){
    if(tempAgua <= 80){
        digitalWrite(pinRed[0], LOW);
        digitalWrite(pinRed[1], LOW);
    }
    if (tempAgua > 80)
        digitalWrite(pinRed[1], HIGH);
    if (tempAgua > 100)
        digitalWrite(pinRed[0], HIGH);
  
}

//Función FUN_MARCHA: recibe la marcha e imprime el número en la matriz.
void showMarcha (unsigned int marcha){
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
void readCanBus(){
  //Limpiamos len y buf.
  unsigned char len = 0;
  unsigned char buf[8];
  unsigned long canId;
  //Leemos CAN LINEA 1 
  do{
  CAN.readMsgBuf(&len, buf);
  }while(buf[0] != 1);
  if(buf[0]== 1){ 
  rev = buf[1] * 256 + buf[2]; //Dato Revoluciones
  tempAgua = buf[3] * 256 + buf[4]; //Dato Temperatura del agua
  marcha = buf[5]; //Dato Marcha actual
  tps = buf[6] * 256 + buf[7];
  
  //Llamada a las respetivas funciones
  revs(rev);
  tempAguaWarning(tempAgua);
  //showMarcha(marcha);
  tpsshow(tps);
  Serial.println("DATOS EN TIEMPO REAL:");

  Serial.print("REVS =");
  Serial.println(rev);
  
  Serial.print("TEMPAGUA = ");
  Serial.println(tempAgua);
  
  Serial.print("MARCHA = ");
  Serial.println(marcha);

  Serial.print("TPS = ");
  Serial.println(tps);
  }
}

//Función START: Inicia el CAN BUS para funcionar.
void start(){  
  
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
  
  for(int i = 0; i < 3; i++)
    pinMode(pinRev[i], OUTPUT);
  for(int i = 0; i < 2; i++)
    pinMode(pinRed[i], OUTPUT);

  //Iniciamos la MATRIZ y ajustamos su brillo.
  for(int i = 0; i < 5; i++){
    m.init(); 
    m.setIntensity(15); // Intensidad luminosa de la matriz
    delay(1000);
  }

  //Llamamos a la función START (Aquí va todo el inicio del programa de CAN y todo)
  start();
}

void loop() {
  //Bucle infinito de la función CanBus(), para actualizar los datos recibidos por la ECU.
  readCanBus();
}
