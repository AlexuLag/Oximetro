#define MOSI 23
#define MISO 19
#define SCK 18
#define CS 5

#include <Utils.Sqlite.h>
#include <PubSubClient.h>
#include <wificlientsecure.h>



WiFiClientSecure  espClient;
PubSubClient client(espClient);



//cuando el sistema no puede establecer conexion con el wifi
void reconnect(String estado){

  while (!client.connected()) {
    Serial.println("Intentando Conexión MQTT");    
    String clientId = mqtt_user;
    String inboundstr="devices/"+clientId+"/messages/devicebound/#";
    //clientId = clientId + String(random(0xffff), HEX);
    clientId=mqtt_user;
    mqtt_user=mqtt_server+"/"+mqtt_user;
    if (client.connect(clientId.c_str(),mqtt_user.c_str(),mqtt_pass.c_str())) {
      Serial.println("Conexión a MQTT exitosa!!!");     
      client.subscribe(inboundstr.c_str());
    }else{
      Serial.println("Falló la conexión ");
      Serial.println(client.state());
      Serial.print(" Se intentará de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

//funcion que se ejecuta cuando el servidor mqtt envia un mensaje
void callbackInbound(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido bajo el tópico -> ");
  Serial.print(topic);
  Serial.print("\n");

//imprime el mensaje enviado por el servidor
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }


  if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println("\n Led Apagado");     

  } else {
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.println("\n Led Encendido");
  }

  Serial.println();
}


void InicializaEnvio() {
  pinMode(BUILTIN_LED, OUTPUT);
  int puerto  = mqtt_port.toInt();
  client.setServer(mqtt_server.c_str(), puerto);
  client.setCallback(callbackInbound);
  reconnect("IniciaEnvio");
}





bool setup_wifi()
{

if(WiFi.status() != WL_CONNECTED){  
  // Conexión a la red Wifi
  Serial.println();
  Serial.println("Conectando a...");
  Serial.println(wifi_ssid.c_str());

  WiFi.begin(wifi_ssid.c_str(), wifi_pwd.c_str());

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    Serial.print(WiFi.status());
     WiFi.begin(wifi_ssid.c_str(), wifi_pwd.c_str());
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
  return true;
  }else{
      return false;

  }


}


void InicializaSistema()
{
//lee parametros de la base de datos sqlite
  InicializaBD();
//se conecta al wifi configurado
while(!setup_wifi())
{
Serial.print(".");

};
}




//captura lecturas recibidas por el sensor y las envia a azure
void EnviarLectura (String bmp, String spo  )
{
  Serial.print("Verificando Conexion para envio.");
  Serial.print("enviando mensaje desde: ");
  Serial.println(WiFi.localIP());


   if(client.connected()==false){    
    reconnect("ReiniciandoConexion");
  }

  client.loop();

  


  //envia los parametros al servidor 
    client.publish("devices/Oximetria_1/messages/events/Oximetria", (bmp+":"+spo).c_str());

    



}
