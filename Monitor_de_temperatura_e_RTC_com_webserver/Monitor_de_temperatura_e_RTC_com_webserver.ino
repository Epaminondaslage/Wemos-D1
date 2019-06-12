/******************************************************************
        Monitoramento de temperatura com HDC1080 e
       Dados mostrados na porta serial e    servidor web
    Servidor web com IP fixo e cliente de rede wifi existente
    Elaborado por Epaminondas Lage - Junho de 2019
 ******************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include "RTClib.h"

// inicializa RTC
// Usar lib ->https://github.com/adafruit/RTClib
RTC_DS3231 rtc;

//Inicializa Sensor de temperatura e umidade HDC 1080
ClosedCube_HDC1080 hdc1080;

// declara variáveis globais para dias da semana e meses do ano
char daysOfTheWeek[7][14] = {"Domingo", "Segunda-Feira", "Terça-Feira", "Quarta-Feira", "Quinta-Feira", "Sexta-Feira", "Sabado"};
char mesesdoano [12][10] = {"Janeiro", "Fevereiro", "Março", "Abril", "Maio" , "Junho", "Julho" , "Agosto", "Setembro", "Outubro" , "Novembro" , "Dezembro"};


// Configuração para acesso à rede wifi
const char* ssid = "pe de serra extra";
const char* password = "planetfone";

// Declara variáveis de armazenamento minimo e maximo
float tmaxima;
float tminima;
int diamin, mesmin, anomin, horamin, minutomin, segundomax, diamax, mesmax, anomax, horamax, minutomax, segundomin;
String diadasemanamin, diadasemanamax;

int a = 0;


// inicializa servidor web
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

  //inicializa parâmetros do RTC
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

  //Inicializa temperatura tmaxima e tminima
  tminima, tminima = hdc1080.readTemperature();

  //Inicializa porta serial
  Serial.begin(9600);

  //Inicializa o sensor de temperatura HDC1080 no endereço 0x40
  hdc1080.begin(0x40);

  // Inicializa conexão wifi
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 0);
  //Serial.begin(9600);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // configura WEMOS com IP fixo
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(IPAddress(10, 0, 0, 23), IPAddress(10, 0, 0, 1), subnet);
  Serial.println("");

  // Espera por conexão wifi pisca led na placa
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED, 1);
    delay(500);
    Serial.print(".");
    digitalWrite(BUILTIN_LED, 1);
  }

  Serial.println("");
  Serial.println("Sensor HDC1080 e RTC com Wemos D1 R2");
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

  // Sensor de temperatura inicializado no endereço 0x40
  hdc1080.begin(0x40);

}

void loop(void) {

  //Mostra dados do RTC na porta serial
  DateTime now = rtc.now();
  Serial.print("Data :");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(mesesdoano[now.month() - 1]);
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
    DateTime now   = rtc.now();
    diamax         = now.day(), DEC;
    mesmax         = now.month(), DEC;
    anomax         = now.year(), DEC;
    horamax        = now.hour(), DEC;
    minutomax      = now.minute(), DEC;
    segundomax     = now.second(), DEC;
    diadasemanamax = daysOfTheWeek[now.dayOfTheWeek()];
  }
  if (hdc1080.readTemperature() < tminima) {
    tminima        = hdc1080.readTemperature();
    DateTime now   = rtc.now();
    diamin         = now.day(), DEC;
    mesmin         = now.month(), DEC;
    anomin         = now.year(), DEC;
    horamin        = now.hour(), DEC;
    minutomin      = now.minute(), DEC;
    segundomin     = now.second(), DEC;
    diadasemanamin = daysOfTheWeek[now.dayOfTheWeek()];
  }

  // Mostra os valores na página web
  server.handleClient();

}


// Montagem da Página Web
String homePage() {
  String tempmax     = String(tmaxima);
  String tempmin     = String(tminima);
  String temp        = String(hdc1080.readTemperature());
  String umid        = String(hdc1080.readHumidity());
  String dmin        = diamin < 10 ? "0" + String(diamin) : String(diamin);
  String mmin        = mesmin < 10 ? "0" + String(mesmin) : String(mesmin);
  String amin        = String(anomin);
  String hmin        = horamin < 10 ? "0" + String(horamin) : String(horamin);
  String mimin       = minutomin < 10 ? "0" + String(minutomin) : String(minutomin);
  String smin        = segundomin < 10 ? "0" + String(segundomin) : String(segundomin);
  String dmax        = diamax < 10 ? "0" + String(diamax) : String(diamax);
  String mmax        = mesmax < 10 ? "0" + String(mesmax) : String(mesmax);
  String amax        = String(anomax);
  String hmax        = horamax < 10 ? "0" + String(horamax) : String(horamax);
  String mimax       = minutomax < 10 ? "0" + String(minutomax) : String(minutomax);
  String smax        = segundomax < 10 ? "0" + String(segundomax) : String(segundomax);
  DateTime now       = rtc.now();
  String diadasemana = String(daysOfTheWeek[now.dayOfTheWeek()]);
  //String dia         = (dia.toInt() < 10 ? "0" + dia : dia);
  String dia         = String(now.day(), DEC);
  String mes         = String(now.month(), DEC);
  String ano         = String(now.year(), DEC);
  String hora        = String(now.hour(), DEC);
  String minuto      = String(now.minute(), DEC);
  String segundo     = String(now.second(), DEC);
  return (
           "<!DOCTYPE html>"
           "<html>"
           "<meta http-equiv='refresh' content='10'/>"
           "<meta charset=\"utf-8\" />"
           "<title>Sitio Pe de Serra - Temperatura da Granja</title>"
           "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<span style='display: none;'>" + String(0) + "</span>"
           "<div style='text-align: center;'><img src =  'http://caixagg.planetfone.com.br/img/Sitiopedeserra.png' alt='' width='290' height=63' /></div>"
           "<div style='text-align: center;'>" + diadasemana + ", " + dia + " de " + mesesdoano[now.month() - 1] + " de " + ano + "</div>"
           "<h1>Temperatura: " + temp + "<sup>o</sup>C</h1>"
           "<h1>Umidade: " + umid + "%</h1>"
           "<table><tbody><tr><td><img src='http://caixagg.planetfone.com.br/img/Tmin.png' alt=' ' width='20' height='35' /></td><td><h4><span style='color: #87CEFA;'>Temperatura Mínima: " + tempmin + " <sup>o</sup>C</span></h4>" + diadasemanamin + ", " + dmin + "/ " + mmin + "/ " + amin + " [" + hmin + ":" + mimin + ":" + smin +  "] </td></tr></tbody></table>"
           "<table><tbody><tr><td><img src='http://caixagg.planetfone.com.br/img/Tmax.png' alt=' ' width='20' height='35' /></td><td><h4><span style='color: #ff0000;'>Temperatura Máxima: " + tempmax + " <sup>o</sup>C</span></h4>" + diadasemanamax + ", " + dmax + "/ " + mmax + "/ " + amax + " [" + hmax + ":" + mimax + ":" + smax +  "] </td></tr></tbody></table>"
           "<p>&nbsp;</p>"
           "<p>&nbsp;</p>"
           "<div style='text-align: center;'>&copy 2019 - Epaminondas Lage </div>"
         );
}
