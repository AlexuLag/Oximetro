#include <Arduino.h>
#include <WiFi.h>

#include <Wire.h>
#include <PubSubClient.h>
#include <wificlientsecure.h>

#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "Adafruit_GFX.h"
#include "OakOLED.h"
#include "Oximetro.Config.h"

OakOLED oled;
MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

uint32_t irBuffer[100]; 
uint32_t redBuffer[100];  
int32_t bufferLength; 
int32_t spo2=0; 
int8_t validSPO2; 
int32_t heartRate=0; 
int8_t validHeartRate; 
float BPM=0;
float SPO2=0; 

//imagen en display
const unsigned char bitmap [] PROGMEM =
{
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
  0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
  0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
  0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
  0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
  0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
  0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


  //oled.drawBitmap( 60, 20, bitmap, 28, 28, 1);
  //oled.display();

   

void setup() 
{
  Serial.begin(115200);
  oled.begin();

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(1, 0);
  oled.println("Cargando parametros    de conexion...");
  Serial.println("Cargando parametros de conexion...");
  oled.display();
  InicializaSistema();
  InicializaEnvio();  

//Inicia comunicación I2C del sensor 
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) 
  {
    Serial.println(F("GY-MAX30102 No se encuentra. Verificar alimentación o conexiones."));
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(1);
    oled.setCursor(0, 0);
    oled.println("Fallo inicializacion del sensor comuniquese con soporte");
    oled.display();
    for (;;);
    while (1);
  }
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(1);
    oled.setCursor(0, 0);
    oled.println("Coloque su dedo...");
    oled.display();
//Configuración para el MAX30102
  byte ledBrightness = 0xFF; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 3200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 16384; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
}
    

  
void loop()
{
//Almacena 100 muestras en 4 segundos
 bufferLength = 100; 

  //Lee las primeras 100 muestras y determine el rango de señal
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) 
      particleSensor.check(); //Verifica el sensor para obtener nuevos datos

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); 
  }

  //calcular la frecuencia cardíaca y la SpO2 después de las primeras 100 muestras (primeros 4 segundos de muestras)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  while (1)
  {
    //volcar los primeros 25 conjuntos de muestras en la memoria y desplazar los últimos 75 conjuntos de muestras a la parte superior
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }
    //tome 25 juegos de muestras antes de calcular la frecuencia cardíaca..
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) 
        particleSensor.check(); 

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); 

        if (validHeartRate > 0 && validSPO2 > 0)
        {
             
          oled.clearDisplay();
          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 16);
          oled.println(heartRate);
          
          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 0);
          oled.println("BPM");
          

          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 30);
          oled.println("SpO2");
          

          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 45);
          oled.println(spo2);
          oled.display();

          Serial.print ("BPM: ");
          Serial.println (heartRate);
          Serial.print ("SpO2: ");
          Serial.println (spo2);

                      
        }
        else
        {
          oled.clearDisplay();
          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 16);
          oled.println(0);

          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 0);
          oled.println("BPM");

          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 30);
          oled.println("SpO2");

          oled.setTextSize(1);
          oled.setTextColor(1);
          oled.setCursor(0, 45);
          oled.println(0);
          oled.display();
        }
        
    }
    if (heartRate > 20 && heartRate < 200 )
    {
       BPM = (heartRate);
    }
    else
    {
      BPM = 0;
    }
    
    if (spo2 > 70 && spo2 < 100) 
    {
      SPO2 = (spo2);
    }
    else
    {
      SPO2 = 0;
    }
    
    if (BPM != 0 && SPO2 != 0)
    {
       EnviarLectura( String(BPM,2), String(SPO2));
       Serial.print("Ritmo Cardiaco:");
       Serial.print(BPM);
       Serial.print("bpm / Sat. Oxigeno:");
       Serial.print(SPO2);
       Serial.println("%");
       delay (500);
    }
    //Recalcular valores 
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  
  
  }
}





   
  
 
 


