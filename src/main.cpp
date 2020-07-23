#include <Arduino.h>
#include <WiFi.h>

#include <Wire.h>
#include <PubSubClient.h>
#include <wificlientsecure.h>
#include "MAX30100_PulseOximeter.h"

#include "Adafruit_GFX.h"
#include "OakOLED.h"
#include "Oximetro.Config.h"






#define REPORTING_PERIOD_MS     1000
uint32_t tsLastReport = 0;
OakOLED oled;
PulseOximeter pox;

 float heartRate;
 uint8_t spO2;

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


//cuando detecta un objeto o beat en el sensor
void onBeatDetected()
{  
  oled.drawBitmap( 60, 20, bitmap, 28, 28, 1);
  oled.display();

 
   
}

void setup() {
  Serial.begin(115200);
  oled.begin();
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(0, 0);
  oled.println("Cargando parametros de conexion...");
  Serial.println("Cargando parametros de conexion...");
  oled.display();
  InicializaSistema();
  InicializaEnvio();  
    


  
  if (!pox.begin())
  {
    Serial.println("Fallo inicializacion del sensor comuniquese con soporte");
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(1);
    oled.setCursor(0, 0);
    oled.println("Fallo inicializacion del sensor comuniquese con soporte");
    oled.display();
    for (;;);
  }
  else
  {
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(2);
    oled.setCursor(0, 0);
    oled.display(); 

     Serial.println("Max iniciado"); 
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA); 
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
  
  }



void loop()
{
  

  pox.update();
 
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
     heartRate=0;
     spO2=0; 


   heartRate= pox.getHeartRate();
   spO2= pox.getSpO2();

    Serial.print("Ritmo Cardiaco:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / Sat. Oxigeno:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.setCursor(0, 16);
    oled.println(heartRate);

    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.setCursor(0, 0);
    oled.println("PPM");

    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.setCursor(0, 30);
    oled.println("SpO2");

    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.setCursor(0, 45);
    oled.println(spO2);
    oled.display();
    tsLastReport = millis();

     if(heartRate!=0 && spO2!=0)
      EnviarLectura( String(heartRate,2), String(spO2));
  heartRate=0;
  spO2=0; 



   
  }
 
 

}
