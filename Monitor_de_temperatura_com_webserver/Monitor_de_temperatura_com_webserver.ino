#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include <LiquidCrystal_I2C.h>

ClosedCube_HDC1080 hdc1080;

//Inicializa o display no endereco 0x3B
//Define o numero de colunas e linhas do display
LiquidCrystal_I2C lcd(0x3B, 16, 2);

//Array que desenha o simbolo de grau
byte grau[8] = {B00110, B01001, B01001, B00110,
                B00000, B00000, B00000, B00000,
               };
// Declara tmaxima e tminima
float tmaxima;
float tminima;

// Rede wifi 1
const char* ssid = "Popo";
const char* password = "Planetfone";

// rede wifi 2
//const char* ssid = "123644987";
//const char* password = "planeta514";

ESP8266WebServer server(80);

//const int led = 13;

void handleRoot() {
  digitalWrite(BUILTIN_LED, 1);
  server.send(200, "text/html", homePage());
  digitalWrite(BUILTIN_LED, 0);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  //inicializa tmaxima e tminima
  tminima, tminima = hdc1080.readTemperature();

  //Inicializa porta serial
  Serial.begin(9600);

  //Inicializa o HDC1080 no endereço 0x40
  hdc1080.begin(0x40);

  //Inicializa o display LCD I2C
  lcd.init();
  lcd.backlight();

  //Atribui a "1" o valor do array "grau", que desenha o simbolo de grau
  lcd.createChar(1, grau);

  //Mostra no display as informações iniciais
  lcd.setCursor(0, 0);
  lcd.print("Temp.: XX.XX  C");
  lcd.setCursor(0, 1);
  lcd.print("Umid.: YY.YY %");



  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 0);
  Serial.begin(9600);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Espera por conexão
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED, 1);
    delay(500);
    Serial.print(".");
    digitalWrite(BUILTIN_LED, 1);
  }

  Serial.println("");
  Serial.println("Sensor HDC1080 com Wemos D1 R2");
  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS Inicializado");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP inicializado");
  Serial.println("");
  hdc1080.begin(0x40);

}

void loop(void) {

  digitalWrite(BUILTIN_LED, 1);
  delay(2000);
  //float tmax;
  //float tmin;
  float t = hdc1080.readTemperature();
  float h = hdc1080.readHumidity();
  //Mostra os valores no display
  lcd.setCursor(7, 0);
  lcd.print(t);
  lcd.setCursor(13, 0);
  lcd.write(1);
  lcd.setCursor(7, 1);
  lcd.print(h);
  //Mostra os valores no Serial Monitor
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" C, Umidade: ");
  Serial.print(h);
  Serial.println(" %");
  digitalWrite(BUILTIN_LED, 0);
  delay(2000);
  // Calcula a temp max e min
  if (hdc1080.readTemperature() > tmaxima) {
    tmaxima = hdc1080.readTemperature();
  }
  if (hdc1080.readTemperature() < tminima) {
    tminima = hdc1080.readTemperature();
  }
  // Mostra os valores na página web
  server.handleClient();

}

// Montagem da Página Web
String homePage() {
  String tempmax = String (tmaxima);
  String tempmin = String (tminima);
  String temp = String(hdc1080.readTemperature());
  String umid = String(hdc1080.readHumidity());
  return (
           "<!DOCTYPE html>"
           "<html>"
           "<meta http-equiv='refresh' content='10'/>"
           "<meta charset=\"utf-8\" />"
           "<title>Sitio Pe de Serra - Temperatura da Granja</title>"
           "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<span style='display: none;'>" + String(0) + "</span>"  
           "<style>img{max-width: 100%;}</style><div style='text-align: center;'><img src='http://caixagg.planetfone.com.br/img/sitiopedeserra.png'></div>"
           "<h1>Temperatura: " + temp + "<sup>o</sup>C</h1>"
           "<h1>Umidade: " + umid + "%</h1>"
           "  "
           "  " 
           "<style>img{max-width: 100%;}</style><div style='text-align: center;'><img src='http://caixagg.planetfone.com.br/img/tmax.png'></div>"
           "<h3>Temperatura Mínima: " + tempmin + "<sup>o</sup>C</h3>"
           "<style>img{max-width: 100%;}</style><div style='text-align: center;'><img src='http://caixagg.planetfone.com.br/img/tmin.png'></div>"
           "<h3>Temperatura Máxima: " + tempmax + "<sup>o</sup>C</h3>"
          
           "       "   
         );
}
