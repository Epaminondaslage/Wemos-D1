#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include "RTClib.h"
//#include <LiquidCrystal_I2C.h>

// inicializa RTC
// Usar lib ->https://github.com/adafruit/RTClib
RTC_DS3231 rtc;

//Inicializa Sensor de temperatura e umidade HDC 1080
ClosedCube_HDC1080 hdc1080;

// declara variáveis globais
char daysOfTheWeek[7][14] = {"Domingo", "Segunda-Feira", "Terça-Feira", "Quarta-Feira", "Quinta-Feira", "Sexta-Feira", "Sabado"};
int a = 0;


//Array que desenha o simbolo de grau no display
byte grau[8] = {B00110, B01001, B01001, B00110,
                B00000, B00000, B00000, B00000,
               };

//Inicializa o display no endereco 0x3B
//Define o numero de colunas e linhas do display
//LiquidCrystal_I2C lcd(0x3B, 16, 2);

// Rede wifi 1
//const char* ssid = "pe de serra extra";
//const char* password = "planetfone";

// rede wifi 2
//const char* ssid = "123644987";
//const char* password = "planeta514";

// rede wifi 3
const char* ssid = "Popo";
const char* password = "Planetfone";

// Declara variáveis de armazenamento minimo e maximo
float tmaxima;
float tminima;
int diamin,mesmin,anomin,horamin,minutomin,segundomin;
String diadasemanamin;



ESP8266WebServer server(80);

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

//Parâmetros de RTC
  Wire.begin(4, 5);
#ifndef ESP8266
  while (!Serial);
#endif

  delay(3000); // Aguarda abertura da console

  if (! rtc.begin()) {
    Serial.println("RTC não encontrado");
    while (1);
  }
  //Rotina para setar as horas do RTC quando perde alimentação
    if (rtc.lostPower()) {
      Serial.println("RTC pedeu alimentação, favor acertar as horas!");
      // Ajusta o RTC para data e hore desta compilação
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // Exemplo para ajustar a hora do RTC Janeiro, 21, 2019 at 3am:
      // rtc.adjust(DateTime(2019, 1, 21, 3, 0, 0));
    }

  //inicializa tmaxima e tminima
  tminima, tminima = hdc1080.readTemperature();

  //Inicializa porta serial
  Serial.begin(9600);

  //Inicializa o sensor de temperatura HDC1080 no endereço 0x40
  hdc1080.begin(0x40);

  /*//Inicializa o display LCD I2C
    lcd.init();
    lcd.backlight();

    //Atribui a "1" o valor do array "grau", que desenha o simbolo de grau
    lcd.createChar(1, grau);

    //Mostra no display as informações iniciais
    lcd.setCursor(0, 0);
    lcd.print("Temp.: XX.XX  C");
    lcd.setCursor(0, 1);
    lcd.print("Umid.: YY.YY %");

  */

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
    server.send(200, "text/plain", "Esta funcionando bem");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP inicializado");
  Serial.println("");
  hdc1080.begin(0x40);

}

void loop(void) {
  //Mostra dados do RTC
  DateTime now = rtc.now();
  Serial.print("Data :");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") Hora: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  digitalWrite(BUILTIN_LED, 1);
  delay(2000);
  float t = hdc1080.readTemperature();
  float h = hdc1080.readHumidity();
  /*Mostra os valores no display
    lcd.setCursor(7, 0);
    lcd.print(t);
    lcd.setCursor(13, 0);
    lcd.write(1);
    lcd.setCursor(7, 1);
    lcd.print(h);*/
  //Mostra os valores no Serial Monitor
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" C, Umidade: ");
  Serial.print(h);
  Serial.print(" %");
  Serial.println();
  Serial.println();
  digitalWrite(BUILTIN_LED, 0);
  delay(2000);
  // Calcula a temp max e min
  if (hdc1080.readTemperature() > tmaxima) {
    tmaxima = hdc1080.readTemperature();
  }
  if (hdc1080.readTemperature() < tminima) {
    DateTime now   = rtc.now();
    tminima        = hdc1080.readTemperature();
    diamin         = (now.day(), DEC);
    mesmin         = (now.month(), DEC);
    anomin         = (now.year(), DEC);
    horamin        = (now.hour(), DEC);
    minutomin      = (now.minute(), DEC);
    segundomin     = (now.second(), DEC);
    diadasemanamin = daysOfTheWeek[now.dayOfTheWeek()];
  }
  // Mostra os valores na página web
  server.handleClient();

}


// Montagem da Página Web
String homePage() {
  DateTime now       = rtc.now();
  String tempmax     = String(tmaxima);
  String tempmin     = String(tminima);
  String temp        = String(hdc1080.readTemperature());
  String umid        = String(hdc1080.readHumidity());
  String dmin        = String(diamin);
  String mmin        = String(mesmin);
  String amin        = String(anomin);
  String hmin        = String(horamin);
  String mimin       = String(minutomin);
  String smin        = String(segundomin);
  String diadasemana = String(daysOfTheWeek[now.dayOfTheWeek()]);
  String dia         = String(now.day(), DEC);
  String mes         = String(now.month(),DEC);
  String ano         = String(now.year(),DEC);
  String hora        = String(now.hour(), DEC);
  String minuto      = String(now.minute(),DEC);
  String segundo     = String(now.second(),DEC);
  return (
           "<!DOCTYPE html>"
           "<html>"
           "<meta http-equiv='refresh' content='10'/>"
           "<meta charset=\"utf-8\" />"
           "<title>Sitio Pe de Serra - Temperatura da Granja</title>"
           "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<span style='display: none;'>" + String(0) + "</span>"
           "<style>img{width: 100%;}</style><div style='text-align: center;'><img src='http://caixagg.planetfone.com.br/img/sitiopedeserra.bmp'></div>"
           "<div style='text-align: center;'>" + diadasemana + ", " + dia + "/" + mes + "/" + ano + "</div>"
           "<h1>Temperatura: " + temp + "<sup>o</sup>C</h1>"
           "<h1>Umidade: " + umid + "%</h1>"
           "<p>&nbsp;</p>"
           "<h3>Temperatura Mínima: " + tempmin + "<sup>o</sup>C</h3>"
           "<div style='text-align: left;'>" + diadasemanamin + ", " + dmin + "/" + mmin + "/" + amin + "</div>"
           "<p>&nbsp;</p>"
           "<h3>Temperatura Máxima: " + tempmax + "<sup>o</sup>C</h3>"
           "<div style='text-align: left;'>" + diadasemana + ", " + dia + "/" + mes + "/" + ano + "</div>"
           "<p>&nbsp;</p>"
         );
  //  "<style>img{with:10%;}</style><div style='text-align: left;'><img src='http://caixagg.planetfone.com.br/img/Tmin.png'></div><h3>Temperatura Mínima: " + tempmin + "<sup>o</sup>C</h3>"
  //  "<style>img{width:10%;}</style><div style='text-align: left;'><img src='http://caixagg.planetfone.com.br/img/Tmax.png'></div><h3>Temperatura Máxima: " + tempmax + "<sup>o</sup>C</h3>"

/*DateTime now = rtc.now();
  Serial.print("Data :");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") Hora: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
*/
}
