/*Programa para Ler sensores e enviar dados para Modbus Ip para o ScadaBR
 * Dois Sensores de Tensão ZMPT101B - Onde foi utilizado na entrada e saída de um Nobreak
 * Sensor de temperatura DHT11
 * Dois sensosres de audio para monitorar o áudio de uma rádio 
 * Utilizado Arduino Uno com Shild Ethernet, Sensor de Tensor ZPMT101B,
 * sensor de temperatura DHT11, Sensor de som LM393 e outro KY-037 onde foi retirado o microfone e adaptado um p2 fêmea
 * Adaptado Danilo Braz - danilobrazsilva@gmail.com
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Modbus.h>
#include <ModbusIP.h>
#include "EmonLib.h" //INCLUSÃO DE BIBLIOTECA Sensores de Tensão
#include "dht.h" //INCLUSÃO DE BIBLIOTECA DE TEMPERATURA DHT11

//Pinos Utilizados
const int pintensao0 = 0; //PINO ANALÓGICO UTILIZADO PELO SENSOR
const int pintensao1 = 2; //PINO ANALÓGICO UTILIZADO PELO SENSOR
const int pintemp    = A4; //PINO ANALÓGICO UTILIZADO PELO SENSOR
const int pinaudio1  = A5; //PINO ANALÓGICO UTILIZADO PELO SENSOR
const int pinaudio2  = A3; //PINO ANALÓGICO UTILIZADO PELO SENSOR

#define CALIB_SENSOR 215.3 //VALOR DE CALIBRAÇÃO (DEVE SER AJUSTADO EM PARALELO COM UM MULTÍMETRO)

ModbusIP mb;

//Modbus Registers Offsets
const int SENSOR_TENSAO1 = 100;
const int SENSOR_TENSAO2 = 101;
const int SENSOR_TEMP    = 103;
const int SENSOR_UMID    = 104;
const int SENSOR_AUDIO1  = 110;
const int SENSOR_AUDIO2  = 111;

//ModbusIP object
long ts;

//configuraçãos da biblioteca do sensor de tensão
EnergyMonitor emon1; //CRIA UMA INSTÂNCIA
EnergyMonitor emon2; //CRIA UMA INSTÂNCIA

dht DHT; //VARIÁVEL DO TIPO DHT

void setup() {
   //Define as configurações de rede
   byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEC };  
   byte ip[] = { 192, 168, 19, 216 }; 
   mb.config(mac,ip);
  
   //Pino definido como entrada
   pinMode(pintensao0, INPUT);  
   pinMode(pintensao1, INPUT);
   pinMode(pintemp, INPUT); 
   pinMode(pinaudio1, INPUT);
   pinMode(pinaudio2, INPUT);
   
   Serial.begin(9600);
   
  //SENSORES DE TENSÃO
  emon1.voltage(pintensao0, CALIB_SENSOR, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
  emon2.voltage(pintensao1, CALIB_SENSOR, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
 
   //Add SENSOR_IREG register
   
   mb.addIreg(SENSOR_TENSAO1);
   mb.addIreg(SENSOR_TENSAO2);
   mb.addIreg(SENSOR_TEMP);
   mb.addIreg(SENSOR_UMID);
   mb.addIreg(SENSOR_AUDIO1);
   mb.addIreg(SENSOR_AUDIO2);
 
}

void loop() {
   //Call once inside loop() - all magic here
   mb.task();
  
  //sensores de tensão
  emon1.calcVI(17,500); //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)  
  emon2.calcVI(17,500); //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)  
  float t0  = emon1.Vrms; //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO
  float t1  = emon2.Vrms; //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO
  
  DHT.read11(pintemp); //LÊ AS INFORMAÇÕES DO SENSOR
  float  temperatura = DHT.temperature; //VARIÁVEL RECEBE A TEMPERATURA MEDIDA
  float  umidade = DHT.humidity; //VARIÁVEL RECEBE A UMIDADE MEDIDA
  
  //sensores de audio
  int audio1 = analogRead(pinaudio1);
  int audio2 = analogRead(pinaudio2);
  //Leitura a cada 2 segundos
   if (millis() > ts + 2000) {
       ts = millis();


      //envio para registrador modbusip
       //Setting raw value (0-1024)
      mb.Ireg(SENSOR_TENSAO1, t0);
      mb.Ireg(SENSOR_TENSAO2, t1);
      mb.Ireg(SENSOR_TEMP, temperatura);
      mb.Ireg(SENSOR_UMID, umidade);
      mb.Ireg(SENSOR_AUDIO1, audio1);
      mb.Ireg(SENSOR_AUDIO2, audio2);
       
   }

//IMPRIME O TEXTO NA SERIAL
//    Serial.println("Tensão do sensor1 : ");
//    Serial.println(t0, 0);
//    Serial.println("");
//    Serial.println("Tensão do sensor2 : ");
//    Serial.println(t1, 0);
//    Serial.println("");  
//    Serial.println("Tensão de Temperatura : ");
//    Serial.println(temperatura);
//    Serial.println("");
//    Serial.println("--------------------");
//    Serial.print("Umidade: "); 
//    Serial.print(umidade, 0); 
//    Serial.print("%"); /
//    Serial.print(" / Temperatura: "); /
//    Serial.print(temperatura, 0); 
//    Serial.println("*C");
//    Serial.print("Audio: ");
//    Serial.print(audio1);
//    Serial.println(",");
//    Serial.print("Audio2: ");
//    Serial.println(audio2);
//    Serial.println("");
  delay(50);
   
}
