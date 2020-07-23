#define MOSI 23
#define MISO 19
#define SCK 18
#define CS 5

#include <Utils.Sqlite.h>
#include <PubSubClient.h>
#include <wificlientsecure.h>



WiFiClientSecure  espClient;
PubSubClient client(espClient);
String clientId; 


//cuando el sistema no puede establecer conexion con el wifi
void reconnect(String estado){

  while (!client.connected()) {
    Serial.println("Intentando Conexión MQTT");    
       
    String clientId = mqtt_user;
    String inboundstr="devices/"+clientId+"/messages/devicebound/#";
    String outBoundStr ="devices/"+mqtt_user+"/messages/events/Oximetria";
    String  urlUser = mqtt_server+"/"+mqtt_user;


    //mqtt_pass="SharedAccessSignature sr=KieDevices.azure-devices.net%2Fdevices%2FOximetria_1&sig=HpgDbTnC7ys3hwF2XD4BzbbVNexgZK2pcGpQCqgnyA0%3D&se=1627011497";
    //         SharedAccessSignature sr=KieDevices.azure-devices.net%2Fdevices%2FOximetria_1&sig=yUoup9BXDuRGgCx2vC0r0z%2FDXmoy4CUDpjUXyujTDcg%3D&se=1595473928

 Serial.println("prueba no :8"); 

 
 Serial.println("mqtt_server:"+mqtt_server);  
  Serial.println("mqtt_port:"+mqtt_port);  
     Serial.println("clientId:"+clientId);    
    Serial.println("urlUser: "+urlUser);    
    Serial.println("mqtt_pass: "+mqtt_pass);
    Serial.println("inboundstr: "+inboundstr);
    

    if (client.connect(clientId.c_str(),urlUser.c_str(),mqtt_pass.c_str())) {
      Serial.println("Conexión a MQTT exitosa!!!");    
      client.subscribe(outBoundStr.c_str()); 
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
     Serial.print("Reconectando"); 
    reconnect("ReiniciandoConexion");
  }

  client.loop(); 
  //envia los parametros al servidor 
    client.publish(("devices/"+mqtt_user+"/messages/events/Oximetria").c_str(), (bmp+":"+spo).c_str());
}
