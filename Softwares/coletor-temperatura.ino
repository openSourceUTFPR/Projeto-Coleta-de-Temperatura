/*
      DataLogger para monitorar temperatura 

      Salva dados no cartão SD

 */
#include <OneWire.h>                        // Biblioteca de aceso e comunicação com os sensorees de temperatura.
#include <SD.h>                             // Biblioteca cartão SD.
#include <SPI.h>                            // Biblioteca de interface de tranferência de dados e monitoramento utilizando a mesma linha de transmissão.
#include <DallasTemperature.h>              // Biblioteca para usar e coletar temperatura dos sensores.
#include <Wire.h>                           // Biblioteca para comunicar e integrar diferentes módulos entre si.
#include "RTClib.h"                         // Biblioteca para o módulo RTC(Real Time Clock).

RTC_Millis rtc;                             // Classe para usar o tempo em milisegundos.
RTC_DS1307 RTC;                             // Classe para identificar o módulo RTC.

// --- MAPEAMNETO PINOS DO HARDWARE ---
const int ONE_WIRE_BUS = 10;                // Sensores de temperatura conectado no pino 10
const int chipSelect = 53;  // Configura o Slave Select pin como o pino 53 para comunicação da placa com o modulo SD

// --- CONFIGURA BIBLIOTECAS DE TEMPERATURA ---
OneWire ourWire(ONE_WIRE_BUS);              // cria uma intância para comunicar com o sensor
DallasTemperature sensors(&ourWire);        // Passa a referência OneWire para a biblioteca DallasTEmperature.

// --- DECLARAR VARIÁVEIS ---
// Endereços e quantidade de sensores.
DeviceAddress T1 = {0x28, 0xFF, 0xDC, 0x91, 0x90, 0x16, 0x04, 0x56}; // variável do sensor 1
/* --- EXEMPLOS ---
DeviceAddress T2 = {0x28, 0xFF, 0x28, 0xBB, 0x90, 0x16, 0x04, 0x84};
DeviceAddress T3 = {0x28, 0xFF, 0x46, 0x9C, 0x87, 0x16, 0x03, 0x72};
*/

float temp;                 // Variável de temperatura.
float tp[50];               // Vetor de temperaturas.
float tempC;                // variável auxiliar de coleta de temperatura

// --- MONITOR SERIAL ---

#define WAIT_TO_START 0 // constante teste para pausa e espera de entrada de dado no serial


void setup() {
  Serial.begin(9600);   // inicializa o monitor serial para analisar os dados
  Serial.println();     // quebra de linha

 // teste de compilação para começar programa
  #if WAIT_TO_START
    Serial.println("Aperte qualquer botão para começar.");
    while (!Serial.available());
  #endif

// escreve no Monitor Serial
  Serial.print("Inicializando cartão SD...");    
  pinMode(53, OUTPUT);                        // Configura pino 53 como saída de dados

// Verifica se o cartão está presente e pode ser inicializado.
  if (!SD.begin(chipSelect)) {
    Serial.println("Cartão falhou ou não presente");
    // Não faz mais nada.
    return;
  }
  Serial.println("Cartão incializado.");

  // Cria um novo arquivo
  File logfile; // cria variável logfile do tipo File
  char filename[] = "DADOS00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
 
    // Apenas abre um novo arquivo se não exite outro.
    if (! SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE);
      break;  // Sai do loop.
    }
}

  Serial.begin(9600); // inicializa serial
  Serial.println(); // quebra de linha
  
  #if WAIT_TO_START
  Serial.println("Aperte qualquer tecla para começar");
  while (!Serial.available()); // executa ação enquanto existir dado disponível 
  #endif //WAIT_TO_START

  // Se o relógio não está rodando, verifica o tempo.
  if (! RTC.isrunning()) {
  Serial.println("RTC is running!");
  // following line sets the RTC to the date & time this sketch was compiled
  // RTC.adjust(DateTime(__DATE__, __TIME__));
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  }

  Wire.begin(); // Inicializa biblioteca wire, e a comunicação entre sesnores e modulo.
  RTC.begin();  // Inicializa o RTC.
  sensors.begin(); // Inicia sensor
  
  sensors.setResolution(T1, 9); // Configura a resolução da leitura em 9bit.
  /*
  sensors.setResolution(T2, 9);
  sensors.setResolution(T3, 9);
  */

}

// Imprime as temperaturas na tela.
void printTemperature(DeviceAddress deviceAddress)
{
  tempC = sensors.getTempC(deviceAddress);
  delay (100); // mudado de 10 para 100
  // Se não conseguir ler temperatura do sensor.
  if (tempC == -127.00) {
    Serial.print("Erro ao ler temperatura !");
  } else {
 
  }
}

// --- LOOP PRINCIPAL ---
void loop() {
  DateTime now = rtc.now(); // variável que recebe a o tempo
  sensors.requestTemperatures(); // Solicita que a função informe a temperatura do sensor
  temp = sensors.getTempCByIndex(0); // temp recebe o valor da temperaura  medido
  sensors.requestTemperatures(); // Solicita que a função informe a temperatura do sensor
  
  // imprime temperaturas no monitor serial
  Serial.print("\n\r");
  Serial.print("  ");
  Serial.print("Temperatura Sensor 1: ");
  printTemperature(T1);
  tp[1]=tempC;
  Serial.print(tp[1]);
  Serial.print("\n\r");
  Serial.print("  ");
  /*
  Serial.print("Temperatura Sensor 2: ");
   printTemperature(T2);
  tp[2]=tempC;
  Serial.print(tp[2]);
  Serial.print("\n\r");
  Serial.print("  ");
  Serial.print("Temperatura Sensor 3: ");
  printTemperature(T3);
  tp[3]=tempC;
  Serial.print(tp[3]);
  Serial.print("\n\r");
  Serial.print("  ");
  */

  // imprime data e hora no monitor serial
  Serial.print("\n\r\n\r");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // cria arquivo e nomeia datafile
  File dataFile = SD.open("DADOS00.txt", FILE_WRITE); // abre arquivo e escreve nele do incio ao fim
  // escreve no arquivo criado data, hora e temperatura
   if (dataFile) 
  dataFile.print("\n\r");
  dataFile.print(' ');
  dataFile.print("\n\r\n\r");
  dataFile.print (now.year(), DEC);
  dataFile.print('/');
  dataFile.print(now.month(), DEC);
  dataFile.print('/');
  dataFile.print(now.day(), DEC);
  dataFile.print(' ');
  dataFile.print(now.hour(), DEC);
  dataFile.print(':');
  dataFile.print(now.minute(), DEC);
  dataFile.print(':');
  dataFile.print(now.second(), DEC);
   
  dataFile.print(" ;");
  dataFile.print(tp[1]); // temperatura
 
  /*
  dataFile.print(" ;");
  dataFile.print(tp[2]);
 
  dataFile.print(" ;");
  dataFile.print(tp[3]);

  */

  dataFile.print(" ");
  dataFile.close();
  delay (600000); // MUDAR O INTERVALO DE TEMPO DE COLETA DE DADOS DE TEMPERATURA EM MILISEGUNDOS.
  
}