#define MOSI 23
#define MISO 19
#define SCK 18
#define CS 5
#include <Utils.Sqlite.h>
//#include <mySD.h>



void setup_wifi()
{
  // Conexión a la red Wifi
  Serial.println();
  Serial.println("Conectando a...");
  Serial.println(wifi_ssid.c_str());

  WiFi.begin(wifi_ssid.c_str(), wifi_pwd.c_str());

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(2000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}


void InicializaSistema()
{
//lee parametros de la base de datos sqlite
  InicializaBD();
//se conecta al wifi configurado
  setup_wifi();
}

