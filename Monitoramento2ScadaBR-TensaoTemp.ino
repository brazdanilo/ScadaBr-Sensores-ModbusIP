/*Programa para Ler sensores e enviar dados para Modbus Ip para o ScadaBR
 * Dois Sensores de Tensão ZMPT101B - Onde foi utilizado na entrada e saída de um Nobreak
 * Sensor de temperatura DHT11
 * Utilizado Arduino Uno com Shild Ethernet, Sensor de Tensor ZPMT101B,
 * Adaptado Danilo Braz - danilobrazsilva@gmail.com
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Modbus.h>
#include <ModbusIP.h>
#include "EmonLib.h" //INCLUSÃO DE BIBLIOTECA Sensores de Tensão
#include "dht.h" //INCLUSÃO DE BIBLIOTECA DE TEMPERATURA DHT11

//Pinos Utilizados
const int pintensao0 = 1; //PINO ANALÓGICO UTILIZADO PELO SENSOR
const int pintensao1 = 2; //PINO ANALÓGICO UTILIZADO PELO SENSOR
const int pintemp    = A4; //PINO ANALÓGICO UTILIZADO PELO SENSOR
const int pinaudio   = A5; //PINO ANALÓGICO UTILIZADO PELO SENSOR

#define CALIB_SENSOR 216.2 //VALOR DE CALIBRAÇÃO (DEVE SER AJUSTADO EM PARALELO COM UM MULTÍMETRO)

ModbusIP mb;

//Modbus Registers Offsets
const int SENSOR_TENSAO1 = 100;
const int SENSOR_TENSAO2 = 101;
const int SENSOR_TEMP    = 103;
const int SENSOR_UMID    = 104;
const int SENSOR_AUDIO   = 110;

//ModbusIP object
long ts;

EnergyMonitor emon1; //CRIA UMA INSTÂNCIA
EnergyMonitor emon2; //CRIA UMA INSTÂNCIA
dht DHT; //VARIÁVEL DO TIPO DHT

void setup() {
   //Configuracao da rede
   byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEA };  
   byte ip[] = { 192, 168, 19, 215 }; 
   mb.config(mac,ip);

   Serial.begin(115200); //Serial
   
  //SENSORES DE TENSÃO
  emon1.voltage(pintensao0, CALIB_SENSOR, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
  emon2.voltage(pintensao1, CALIB_SENSOR, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
  
   //Add SENSOR_IREG register - Use addIreg() for analog Inputs
   mb.addIreg(SENSOR_TENSAO1);
   mb.addIreg(SENSOR_TENSAO2);
   mb.addIreg(SENSOR_TEMP);
   mb.addIreg(SENSOR_UMID);
   mb.addIreg(SENSOR_AUDIO);
 
}

void loop() {
   //Call once inside loop() - all magic here
   mb.task();
  
  //sensores de tensão
  emon1.calcVI(17,500); //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)  
  emon2.calcVI(17,500); //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)  
  float t0  = emon1.Vrms; //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO
  float t1  = emon2.Vrms; //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO

  //sensor de temperatura
  DHT.read11(pintemp); //LÊ AS INFORMAÇÕES DO SENSOR
  float  temperatura = DHT.temperature; //VARIÁVEL RECEBE A TEMPERATURA MEDIDA
  float  umidade = DHT.humidity; //VARIÁVEL RECEBE A UMIDADE MEDIDA
  
  //Read each two seconds
   if (millis() > ts + 2000) {
       ts = millis();
  
       //Setting raw value (0-1024)
      mb.Ireg(SENSOR_TENSAO1, t0);
      mb.Ireg(SENSOR_TENSAO2, t1);
      mb.Ireg(SENSOR_TEMP, temperatura);
      mb.Ireg(SENSOR_UMID, umidade);
            
   }
//    IMPRIME O TEXTO NA SERIAL
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
//    Serial.print("Umidade: "); //IMPRIME O TEXTO NA SERIAL
//    Serial.print(umidade, 0); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO
//    Serial.print("%"); //ESCREVE O TEXTO EM SEGUIDA
//    Serial.print(" / Temperatura: "); //IMPRIME O TEXTO NA SERIAL
//    Serial.print(temperatura, 0); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO E REMOVE A PARTE DECIMAL
//    Serial.println("*C"); //IMPRIME O TEXTO NA SERIAL

  delay(100);
   
}
