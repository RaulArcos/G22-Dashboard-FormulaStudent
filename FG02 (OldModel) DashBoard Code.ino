i/*PROGRAMACIÓN SALPICADERO FORMULA GADES 2021 FG02 BY RAUL ARCOS HERRERA.

Con este código se intenta minimizar el mayor problema del arduino, el hecho de que solo tenga 1 hilo, es decir, cada acción ocurre en orden lineal.
Por ejemplo, si para que un led parpadee, tienes que añadir un retraso entre encendido y apagado durante 50 milisegundos, todo el programa debe esperar esos 50 milisegundos, 
tanto la pantalla como los leds como los datos, todo va en orden y es por eso que el codigo intenta ser lo más optimizado posible para evitar el LAG que provocaria que,
una acción afecte a las otras.

Este código también viene explicado para que cualquier miembro del equipo pueda entenderlo asi como modificarlo.
*/

//LIBRERIAS A USAR
#include <Ticker.h>
#include <Wire.h>  // Lo trae Arduino IDE
#include <LiquidCrystal_I2C.h> // Incluimos la libreria del LCD
#include <SPI.h>
#include <MaxMatrix.h> 
#include <stdlib.h>
#include "mcp_can.h"
#include <ArduinoSort.h>

//Declaración de variables globales
size_t m_demo = 0;

//Definimos variables para el uso de componentes.
int data = 8;    // DIN pin del módulo MAX7219
int load = 9;    // CS pin del módulo MAX7219
int clock = 10;  // CLK pin del módulo MAX7219
int maxInUse = 1;    // Cuantos módulos MAX7219 se estan usando
MaxMatrix m(data, load, clock, maxInUse);
byte buffer[10];

//Definimos también las variables que vamos a usar con el CAN BUS para ahorrarnos esas declaraciones continuas
//Dependiendo del dato que queramos recibir usaremos unsigned int (entero sin signo) o float (por ejemplo para numeros con decimales), esto se hace porque unsigned int ocupa poca memoria. +fluidez
unsigned int d1;
unsigned int d2;
unsigned int d3;
unsigned int d4;
unsigned int d5;
unsigned int d6;

//Definición del LOGO G de gades.
byte BUno[8] = {B00000,B00011,B00111,B01111,B01111,B11111,B11110,B11100};
byte BDos[8] = {B01101,B11101, B11011,B11000,B10000,B00000,B00011,B00011};
byte BTres[8] = {B11110,B11110,B11110,B01110,B00000,B00000,B11110,B11110};
byte BCuatro[8] = {B11011,B11011,B10111,B10111,B00111,B00111,B00111,B00000};
byte BCinco[8] = {B00011,B00011,B00001,B10001,B11001,B11101,B11110,B11110};
byte BSeis[8] = {B11110,B11110,B11110,B11100,B11100,B11100,B11000,B00000};

//Definimos los números de la MATRIZ
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

//Definición del PIN CS (53 mega // 9 uno)
const int SPI_CS_PIN = 53;

//Informamos a la librería CAN el PIN CS
MCP_CAN CAN(SPI_CS_PIN);                      
LiquidCrystal_I2C lcd(0x3F, 20, 4);

//DEFINIMOS LA TIRA LED RPM (Donde están conectados)
int led[] = {22,23,24,25,26,27,28,29,30,31,32,33};

//FUNCION DEMO: Pone el salpicadero en modo DEMO (Tira de LEDS y pantalla LCD)
void DemoDash(){

    Logo1();
    digitalWrite(43,HIGH);
    digitalWrite(44,HIGH);
    
    for (int i = 0;i < 12;i++){
    digitalWrite(led[i],HIGH);
    delay(50);
    }

    digitalWrite(42,HIGH);
    if(m_demo < 5)
    {
      grid(m_demo);
      m_demo++;
    }
    else
    {
      grid(m_demo);
      m_demo=0;
    }
  
    lcd.clear();
    lcd.setCursor(6, 1); lcd.print("DEMO"); lcd.setCursor(11, 1); lcd.print("v2");
    lcd.setCursor(1, 2); lcd.print("FORMULA GADES 2021");
    
    digitalWrite(43,LOW);
    digitalWrite(44,LOW);
    for (int i = 0;i < 12;i++){
    digitalWrite(led[i],LOW);
    delay(50); 
  }  
  digitalWrite(42,LOW);
  
}

//FUNCIÓN LOGO1: Muestra el logo( G ).
void Logo1(){
  lcd.clear();
  lcd.createChar(0, BUno); lcd.createChar(1, BDos); lcd.createChar(2, BTres); lcd.createChar(3, BCuatro); lcd.createChar(4, BCinco); lcd.createChar(5, BSeis);
  lcd.home();
  lcd.setCursor(8, 1);
  lcd.write(0); lcd.write(1); lcd.write(2);
  lcd.setCursor(8, 2); lcd.write(3); lcd.write(4); lcd.write(5);
}

//FUNCIÓN LOGO2: Muestra el logo FORMULA GADES
void Logo2(){
  lcd.clear();
  lcd.setCursor(10, 1); lcd.write(0); lcd.write(1); lcd.write(2);
  lcd.setCursor(10, 2); lcd.write(3); lcd.write(4); lcd.write(5);
  lcd.setCursor(2, 2); lcd.print("FORMULA"); lcd.setCursor(13, 2); lcd.print("ADES");
  
}

//FUNCIÓN START: Muestra la animación de inicio e inicia el CAN BUS para funcionar.
void Start(){

  //SECUENCIA DE INICIO
  Logo1();
  m.clear();
  memcpy_P(buffer, GRID + 70, 10);
  m.writeSprite(0, 0, buffer);   
  delay(2000);
  for (size_t i = 0;i < 12;i++){
    digitalWrite(led[i],HIGH);
    delay(50);
  }
  for(size_t i = 0; i<7; i++)
  {
    grid(i);
    delay(500);
  }
  digitalWrite(42,HIGH);
  digitalWrite(43,HIGH);
  digitalWrite(44,HIGH);
  Logo2();
  delay(2000);
  digitalWrite(42,LOW);
  digitalWrite(43,LOW);
  digitalWrite(44,LOW);
  for (size_t i = 0;i < 12;i++){
    digitalWrite(led[i],LOW);
    delay(50);
  }
  m.clear();
  //INICIA EL CANBUS
  while (CAN_OK != CAN.begin(CAN_250KBPS)) {           //PONER ECU A 125 SI NO, NO VA.
    Serial.println("CAN BUS Shield Fallo Inicio");
    lcd.setCursor(3, 0);
    lcd.print("CAN BUS FAIL");
    Serial.println(" Init CAN BUS Shield again");
    lcd.setCursor(2, 1);
    lcd.print("BIEN CONECTADO??");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("CAN BUS Iniciado");
  lcd.setCursor(3, 1);
  lcd.print("Correctamente");
  lcd.setCursor(1, 3);
  lcd.print("SalpicaderoFG Test");
  delay(2000);
  lcd.clear();
}

//FUNCION GRID: Declara la posición de los números en la MATRIZ.
void grid(int gear){

    if(gear == 0){
      m.clear();
      memcpy_P(buffer, GRID, 10);
      m.writeSprite(0, 0, buffer);    
    }
     if(gear == 1){
     m.clear();
     memcpy_P(buffer, GRID + 10, 10);
     m.writeSprite(0, 0, buffer);    
    }
    if(gear == 2){
      m.clear();
      memcpy_P(buffer, GRID + 20, 10);
      m.writeSprite(0, 0, buffer);    
    }
     if(gear == 3){
      m.clear();
        memcpy_P(buffer, GRID + 30, 10);
        m.writeSprite(0, 0, buffer);    
    }
    if(gear == 4){
      m.clear();
        memcpy_P(buffer, GRID + 40, 10);
        m.writeSprite(0, 0, buffer);    
    }
     if(gear == 5){
      m.clear();
        memcpy_P(buffer, GRID + 50, 10);
        m.writeSprite(0, 0, buffer);    
    }
      if(gear == 6){
        m.clear();
        memcpy_P(buffer, GRID + 60, 10);
        m.writeSprite(0, 0, buffer);    
    }
}

//FUNCIÓN CANBUS: Lee los datos del CANBUS y los imprime por pantalla.
//Con el CANBUS podemos llegar a leer 6 lineas de forma fluida o incluso más pero con algo de lag.
void CanBus(){
  
  //Limpiamos len y buf.
  unsigned char len = 0; -
  unsigned char buf[8];

  //Leemos CAN LINEA 1 
  do{
    CAN.readMsgBuf(&len, buf); 
  }while(buf[0]==1);
      d1 = buf[1] * 256 + buf[2]; //DatoRPM
      d2 = buf[3] * 256 + buf[4]; //DatoTemp
      d3 = buf[5] * 256 + buf[6]; //DatoRandom
  //Leemos CAN LINEA 2
  do{
    CAN.readMsgBuf(&len, buf); 
  }while(buf[0]==2);
      d4 = buf[1] * 256 + buf[2]; //DatoRPM
      d5 = buf[3] * 256 + buf[4]; //DatoTemp
      d6 = buf[5] * 256 + buf[6]; //DatoRandom
  
  //Ahora, tenemos todos los datos que la ECU tiene que aportar en las variables, ya sea verlas en pantalla o ver si hay peligro con ellas.

}

void setup() {
  //Definimos el Serial en el que hablará el arduino.
  Serial.begin(115200);

  //Definimos los BOTONES y LEDS que vamos a usar.
  pinMode(2,INPUT);//OK BUTTON
  pinMode(3,INPUT);//PRINCIPAL BUTTON pinMode(42,OUTPUT);//LED AVISO
  pinMode(42,OUTPUT);//LED AVISO (naranja)
  pinMode(43,OUTPUT);//LED PELIGRO (Rojo)
  pinMode(44,OUTPUT);//LED ARRANQUE (Rojo)

  //Iniciamos la MATRIZ y ajustamos su brillo.
  m.init(); 
  m.setIntensity(15); // Intensidad luminosa de la matriz

  //Declaramos los LEDS como OUTPUTS
  for(int i= 0;i<12;i++){
    pinMode(led[i],OUTPUT);
  }
  
  //Comenzamos el LCD 
  lcd.begin(); 

  //Llamamos a la función START (Aquí va todo el inicio del programa de CAN y todo).
  Start();
  
}

void loop() {
  //Llamamos a la función DEMO para que se repita constantemente.
  //DemoDash();
  CanBus();
}
