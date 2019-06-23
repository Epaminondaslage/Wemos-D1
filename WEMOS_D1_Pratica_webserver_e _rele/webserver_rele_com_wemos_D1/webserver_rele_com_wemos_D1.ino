/**************************************************************************
                    Placa de desenvolvimento Wemos D1
                 Acionamento de um módulo relé com página web
                   Necessita de shield relé para Wemos d1
                   Não necessita de rede conectada à Internet

 **************************************************************************/

//Incluir as libraries#include <ESP8266WiFi.h>
#include <ESP8266WiFi.h>

// declara os dados da rede wifi
const char* ssid = "SEU SSID";
const char* password = "SUA SENHA";

//Declara os pinos de saída
int ledPin = D4;

// Inicia o Servidor web
WiFiServer server(80);

void setup() {
  // inicializa porta serial com velocidade 115200
  Serial.begin(115200);
  delay(10);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Conecta a sua Rede WIFI e Mostra na Porta Serial
  Serial.println();
  Serial.println();
  Serial.print("Conectando a rede: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");

  // Inicia o Servidor no qual iremos ver no navegador e poder acionar o Relé
  server.begin();
  delay(500);
  Serial.println("Servidor Iniciado");

  // Retorna o Valor do IP que estará nosso servidor na Rede.
  Serial.println("=====================================");
  Serial.print("Usar essa URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.println("=====================================");

}
void loop() {

  // Monitora se existe Cliente  usando o navegador. Pode ser acompanhado pela porta COM.
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Servidor fica em estado de espera para alguém enviar o comando.
  Serial.println("IoT Eletrotécnica");
  while (!client.available()) {
    delay(1);
  }

  // Faz a leitura da chamada HTTP e mostra os valores baseado no programa acima no VOID SETUP.
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Compara o pedido de Chamada HTTP e verifica se bate com o programa.

  int value = LOW;
  if (request.indexOf("/RELE=LIGADO") != -1) {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/RELE=DESLIGADO") != -1) {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
  //  Retorna o Valor Lido e mostra no Navegador.
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<p style='text-align: center;'><span style='color: #0000ff;'><strong>Laborat&oacute;rio Iot Eletrot&eacute;cnica</strong></span></p>");
  client.print("Status do Rel&eacute WEMOS D1 R1: ");

  if (value == HIGH) {
    client.print("<span style='color: #ff0000;'><strong>Ligado</strong></span>");
  } else {
    client.print("<span style='color: #339966;'><strong>Desligado</strong></span>");
  }
  client.println("<br><br>");
  client.println("Click <a href=\"/RELE=LIGADO\">Aqui</a> para ativar Rel&eacute. <br>");
    client.println("<br>");
  client.println("Click <a href=\"/RELE=DESLIGADO\">Aqui</a> para desativar Rel&eacute. <br>");
  client.println("</html>");
  delay(1);
  Serial.println("");

}
