#include <WiFi.h>
#include <WebServer.h>

// === CONFIGURAÇÃO DO WI-FI ===
const char* ssid     = "andaraki calcados";       // coloque seu SSID
const char* password = "mateuscorno";      // coloque sua senha

// === PINO DO LED ===
const int ledPin = 2;  // GPIO2, D2 na sua placa
bool ledState = false;  // Estado inicial do LED (desligado)

// === SERVIDOR HTTP ===
WebServer server(80);

// === HTML da página (minha adaptação do seu código) ===
String getPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>IR-emote Home</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.8/dist/css/bootstrap.min.css" rel="stylesheet">
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">
<style>
.btn-round { 
  width: 70px; 
  height: 70px; 
  border-radius: 50%; 
  display:flex; 
  align-items:center; 
  justify-content:center; 
  font-size:2rem; 
}
.centered-form {
  display: flex;
  justify-content: center;
  align-items: center;
  flex-direction: column;
  margin-top: 50px;
}
</style>
</head>
<body>
<div class="container">
  <div class="centered-form">
    <h1>Controle LED ESP32</h1>
    <form action="/toggle" method="get">
      <button type="submit" class="btn btn-outline-danger btn-round mt-3">
        <i class="bi bi-power"></i>
      </button>
    </form>
    <p class="mt-3">LED Estado: )rawliteral";
  html += (ledState ? "Ligado" : "Desligado");
  html += R"rawliteral(</p>
  </div>
</div>
</body>
</html>
)rawliteral";
  return html;
}


// === ROTAS DO SERVIDOR ===
void handleRoot() {
  server.send(200, "text/html", getPage());
}

void handleToggle() {
  ledState = !ledState;               // Alterna o estado do LED
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  server.send(200, "text/html", getPage());  // Atualiza a página
}

// === SETUP ===
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Conectar Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // Configurar rotas HTTP
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

// === LOOP ===
void loop() {
  server.handleClient();
}
