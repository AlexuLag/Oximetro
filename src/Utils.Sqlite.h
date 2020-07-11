#define MOSI 23
#define MISO 19
#define SCK 18
#define CS 5

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD.h"



//variables que seran asignadas desde la sd
String  wifi_ssid;    //ssid de red wifi
String wifi_pwd;      //pwd de red wifi
String mqtt_server;   //servidor mqtt
String mqtt_port;     //puerto mqtt
String mqtt_user;     //usuario que se conecta al server mqtt
String mqtt_pass;     //contraseña asignada por el server al dispositivo mqtt
int contadorRegistros =0;
char *zErrMsg = 0;
const char* PathBD = "/sd/kie.ParametrosDB.db";                 //path de la base de datos sqlite en el dispositivo arduino
const char* strSQL = "Select Nombre,valor from parametros";     //comando sql que carga todas las variables del sistema




//funcion callback para cada registro de la base de datos
const char* data = "func";
static int callback(void *data, int argc, char **argv, char **azColName){
   int i; 
   for (i = 0; i<argc; i++){
        if(contadorRegistros==0){wifi_ssid=argv[i] ;}           
        if(contadorRegistros==1){wifi_pwd=argv[i];}            
        if(contadorRegistros==2){mqtt_server=argv[i];}            
        if(contadorRegistros==3){ mqtt_port=argv[i];}           
        if(contadorRegistros==4){ mqtt_user=argv[i];}           
        if(contadorRegistros==5){mqtt_pass=argv[i];}

   }
  
   contadorRegistros++;
   return 0;
}

//Abre base de datos  con el path en la sd
int openDb(const char *filename, sqlite3 **db) {
   Serial.println(filename);
   int rc = sqlite3_open(filename, db);
   if (rc) {
       Serial.printf("No se puede abrir Base de datos: %s\n", sqlite3_errmsg(*db));       
       return rc;
   } else {
       Serial.printf("Base de datos abierta \n");
   }
   return rc;
}

//ejecuta comando sql en la base de datos sqlite
int db_exec(sqlite3 *db, const char *sql) { 
       
   int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if (rc != SQLITE_OK) {
       Serial.printf("SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
   } else {      
       Serial.printf("Operacion ejecutada con exito\n");
   }
   return rc;
}


//inicia la lectura de la base de datos 
void InicializaBD() {

   sqlite3 *db1;
   int rc;

   SPI.begin();
   SD.begin();
   sqlite3_initialize();
//abre base de datos 
   if (openDb(PathBD, &db1))
       return;
  
contadorRegistros=0;  
   rc = db_exec(db1, strSQL);
   if (rc != SQLITE_OK) {
       sqlite3_close(db1);      
       return;
   }
   sqlite3_close(db1);
}

