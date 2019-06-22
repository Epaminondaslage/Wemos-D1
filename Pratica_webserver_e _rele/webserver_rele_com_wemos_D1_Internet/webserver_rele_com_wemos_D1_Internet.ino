/**************************************************************************
                    Placa de desenvolvimento Wemos D1 
                 Acionamento de um módulo relé com página web
                   Necessita de shield relé para Wemos d1 
            
 **************************************************************************/
//Incluir as libraries#include <ESP8266WiFi.h>
#include <ESP8266WiFi.h>

// declara os dados da rede wifi
const char* ssid = "SEU SSID";
const char* password = "SUA SENHA";

//Declara os pinos de saída
int ledPin = D2; // GPIO2
int ledPin2 = D3; // GPIO5

// Inicia o Servidor web
WiFiServer server(80);

void setup() {
  
  // inicializa porta serial com velocidade 115200
  Serial.begin(115200);
  delay(10);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, LOW);
 
  // Conecta a redeWIFI
  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");

  // Iniciando o servidor web
  server.begin();
  Serial.println("Webserver inicializado");
 
  // Mostra o IP para acesso na porta serial
  Serial.print("Use está URL para acessar: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  
  // Checa se o cliente está conectado
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Espera o cliente enviar algum comando
  Serial.println("Novo cliente");
  while (!client.available()) {
    delay(1);
  }

  // Le a primeira linha da requisição
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Marca a requicão
  int value = LOW;
  int value2 = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
  if (request.indexOf("/LED2=ON") != -1) {
    digitalWrite(ledPin2, HIGH);
    value2 = HIGH;
  }
  if (request.indexOf("/LED2=OFF") != -1) {
    digitalWrite(ledPin2, LOW);
    value2 = LOW;
  }

  // Ajusta a saida conforme requisições
  //digitalWrite(ledPin, value);
  // Retorna a resposta
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<meta charset ='utf-8'/>");
  client.println("<meta name = 'viewport' content = 'width=device-width, initial-scale=1'>");
  client.println(F("<link   href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.1/css/bootstrap.min.css' rel='stylesheet'></link>"));
  client.println("<div class='jumbotron'>");
  client.println("<h1 class='text-center'>Automação Residencial</h1>");
  client.println("<h2 class='text-center'>Eletrotécnica</h2>");
  client.println("</div>");
  client.println("<div class='col-md-6'>");
  client.println("<div class='alert alert-info text-center' role='alert'>Lampada 1</div>");
  client.println("<a class='btn btn-danger btn-lg center-block' href=\"/LED=OFF\"> Desligar </a>");
  client.println("<br>");
  client.println("<a class='btn btn-success btn-lg center-block' href=\"/LED=ON\"> Ligar </a>");
  client.println("</div>");
  client.println("<div class='col-md-6'>");
  client.println("<div class='alert alert-info text-center' role='alert'>Lampada 2</div>");
  client.println("<a class='btn btn-danger btn-lg center-block' href=\"/LED2=OFF\"> Desligar </a>");
  client.println("<br>");
  client.println("<a class='btn btn-success btn-lg center-block' href=\"/LED2=ON\"> Ligar </a>");
  client.println("</div>");
  client.println("</html>");
  delay(1);
  Serial.println("Cliente desconectado");
  Serial.println("");
}

