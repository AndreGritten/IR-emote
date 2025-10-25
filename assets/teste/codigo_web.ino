#include <WiFi.h>
#include <WebServer.h>
#include <IRremote.hpp>
#include <SPIFFS.h>
#include <ArduinoJson.h>

const char* ssid = "ESP32_IR_Remote";
const char* password = "12345678";

WebServer server(80);

#define IR_RECEIVE_PIN 15
#define IR_SEND_PIN 4

struct IRSignal {
  String name;
  uint16_t address;
  uint8_t command;
};

std::vector<IRSignal> irSignals;

unsigned long lastSendTime = 0;  // tempo do último envio IR

bool captureMode = false;  // controla se estamos aguardando um sinal real
String pendingName = "";   // guarda o nome que o usuário digitou

void setup() {
  
  irSignals.clear(); // limpa lista ao iniciar

  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  loadIRSignals();
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN);
  
  server.on("/", handleRoot);
  server.on("/capture", handleCaptureIR);
  server.on("/delete", handleDeleteIR);
  server.on("/send", handleSendIR);
  server.on("/style.css", handleStyleCSS);
  
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (IrReceiver.decode()) {
    // só processa se estivermos capturando e o código for válido
    if (captureMode && IrReceiver.decodedIRData.address != 0 && IrReceiver.decodedIRData.command != 0) {
      IRSignal signal = {
        pendingName,
        IrReceiver.decodedIRData.address,
        IrReceiver.decodedIRData.command
      };

      irSignals.push_back(signal);
      saveIRSignals();

      Serial.println("✅ Sinal IR capturado com sucesso!");
      Serial.print("Nome: "); Serial.println(signal.name);
      Serial.print("Endereço: 0x"); Serial.print(signal.address, HEX);
      Serial.print("  Comando: 0x"); Serial.println(signal.command, HEX);

      captureMode = false; // desativa captura até o próximo clique
      pendingName = "";
    }

    IrReceiver.resume();
  }
}


void handleRoot() {
  String page = R"====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Web-Based IR Remote Control Interface</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <div class="container">
    <header>
      <h1>IR-emote - realize operações via infravermelho</h1>
    </header>
    <main>
      <section class="capture-form">
        <h2>Capturar Novo Sinal</h2>
        <form onsubmit="startCapture(event)">
          <input type="text" id="signalName" placeholder="Informe o nome do sinal" required>
          <button type="submit" class="btn btn-primary">Capturar</button>
        </form>
      </section>
      <section class="signal-list">
        <h2>Sinais IR Capturados</h2>
        <ul>
)====";

  for (size_t i = 0; i < irSignals.size(); i++) {
    page += "<li class='signal-item'>";
    page += "<div class='signal-info'>";
    page += "<h3>" + irSignals[i].name + "</h3>";
    page += "<span>Endereço: 0x" + String(irSignals[i].address, HEX) + "</span>";
    page += "<span>Comando: 0x" + String(irSignals[i].command, HEX) + "</span>";
    page += "</div>";
    page += "<div class='signal-actions'>";
    page += "<button class='btn btn-send' onclick='sendSignal(" + String(i) + ")'>Enviar Sinal</button>";
    page += "<a href='/delete?id=" + String(i) + "' class='btn btn-delete'>Excluir</a>";
    page += "</div></li>";
  }

  page += R"====(
        </ul>
      </section>
    </main>
    <footer>
      <p>&copy; 2025 Interface ESP32: Capture, Armazene e Envie Sinais IR. Por André Gritten, Djames Renunza e Mateus Furtado</p>
    </footer>
  </div>

  <script>
    async function startCapture(event) {
      event.preventDefault();
      const name = document.getElementById("signalName").value;
      if (!name) return alert("Informe um nome para o sinal!");
      try {
        const response = await fetch(`/capture?name=${encodeURIComponent(name)}`, { method: "GET" });
        const text = await response.text();
        alert(text);
      } catch (error) {
        alert("Erro ao iniciar captura.");
        console.error(error);
      }
    }

    async function sendSignal(id) {
      try {
        const response = await fetch(`/send?id=${id}`);
        if (response.ok) {
          console.log("Sinal enviado com sucesso:", id);
        } else {
          console.error("Erro ao enviar sinal:", id);
        }
      } catch (error) {
        console.error("Falha na conexão:", error);
      }
    }
  </script>
</body>
</html>
)====";

server.send(200, "text/html", page);
}

void handleCaptureIR() {
  if (!server.hasArg("name")) {
    server.send(400, "text/plain", "Erro: nome do sinal ausente.");
    return;
  }

  String name = server.arg("name");
  server.send(200, "text/plain", "Aponte o controle para o sensor e pressione o botão desejado...");

  unsigned long startTime = millis();
  bool received = false;

  while (millis() - startTime < 5000) {  // aguarda até 5 segundos
    if (IrReceiver.decode()) {
      uint16_t address = IrReceiver.decodedIRData.address;
      uint16_t command = IrReceiver.decodedIRData.command;

      // Só grava se for um sinal válido
      if (command != 0 || address != 0) {
        irSignals.push_back({name, address, command});
        Serial.printf("Sinal capturado: %s | Addr: 0x%X | Cmd: 0x%X\n", name.c_str(), address, command);
        received = true;
      }

      IrReceiver.resume();
      break;
    }
    delay(100);
  }

  if (!received) {
    server.send(200, "text/plain", "Nenhum sinal IR detectado. Tente novamente.");
  } else {
    server.send(200, "text/plain", "Sinal capturado com sucesso!");
  }
}



void handleDeleteIR() {
  size_t id = server.arg("id").toInt();
  
  if (id < irSignals.size()) {
    irSignals.erase(irSignals.begin() + id);
    saveIRSignals();
  }
  
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSendIR() {
  size_t id = server.arg("id").toInt();
  
  if (id < irSignals.size()) {
    // Desativa temporariamente o receptor para não captar o próprio sinal
    IrReceiver.stop();
    delay(50);

    // Envia o sinal IR armazenado
    IrSender.sendNEC(irSignals[id].address, irSignals[id].command, 2);

    delay(200);  // tempo pra terminar o envio

    // Religa o receptor IR
    IrReceiver.start();

    // Guarda o momento em que o sinal foi enviado
    lastSendTime = millis();
  }

  server.send(200, "text/plain", "OK");
}


void handleStyleCSS() {
  String css = R"(
:root {
  --primary-color: #007bff;
  --secondary-color: #6c757d;
  --success-color: #28a745;
  --danger-color: #dc3545;
  --light-color: #f8f9fa;
  --dark-color: #343a40;
}

* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

body {
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  line-height: 1.6;
  color: var(--dark-color);
  background-color: var(--light-color);
}

.container {
  max-width: 800px;
  margin: 0 auto;
  padding: 20px;
}

header {
  text-align: center;
  margin-bottom: 2rem;
}

h1 {
  color: var(--primary-color);
}

h2 {
  color: var(--secondary-color);
  margin-bottom: 1rem;
}

.instructions {
  background-color: #fff;
  border-radius: 8px;
  padding: 20px;
  margin-bottom: 2rem;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.instructions ol {
  padding-left: 20px;
}

.capture-form {
  background-color: #fff;
  border-radius: 8px;
  padding: 20px;
  margin-bottom: 2rem;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

form {
  display: flex;
  gap: 10px;
}

input[type="text"] {
  flex-grow: 1;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

.btn {
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.3s ease;
}

.btn-primary {
  background-color: var(--primary-color);
  color: white;
}

.btn-primary:hover {
  background-color: #0056b3;
}

.signal-list {
  background-color: #fff;
  border-radius: 8px;
  padding: 20px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.signal-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 15px;
  border-bottom: 1px solid #eee;
}

.signal-item:last-child {
  border-bottom: none;
}

.signal-info h3 {
  margin-bottom: 5px;
}

.signal-info span {
  display: block;
  font-size: 0.9em;
  color: var(--secondary-color);
}

.signal-actions {
  display: flex;
  gap: 10px;
}

.btn-send {
  background-color: var(--success-color);
  color: white;
}

.btn-send:hover {
  background-color: #218838;
}

.btn-delete {
  background-color: var(--danger-color);
  color: white;
}

.btn-delete:hover {
  background-color: #c82333;
}

footer {
  text-align: center;
  margin-top: 2rem;
  color: var(--secondary-color);
}

@media (max-width: 600px) {
  .container {
    padding: 10px;
  }
  
  form {
    flex-direction: column;
  }
  
  .signal-item {
    flex-direction: column;
    align-items: flex-start;
  }
  
  .signal-actions {
    margin-top: 10px;
  }
}
  )";
  
  server.send(200, "text/css", css);
}

void loadIRSignals() {
  File file = SPIFFS.open("/irsignals.json", "r");
  
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  
  DynamicJsonDocument doc(1024);
  
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print(F("Failed to read file, using default configuration"));
    return;
  }
  
  for (JsonVariant v : doc["signals"].as<JsonArray>()) {
    IRSignal signal = {
      v["name"].as<String>(),
      v["address"].as<uint16_t>(),
      v["command"].as<uint8_t>()
    };
    
    irSignals.push_back(signal);
  }
  
  file.close();
}

void saveIRSignals() {
  DynamicJsonDocument doc(1024);
  
  JsonArray array = doc.createNestedArray("signals");
  for (IRSignal &signal : irSignals) {
    JsonObject obj = array.createNestedObject();
    obj["name"] = signal.name;
    obj["address"] = signal.address;
    obj["command"] = signal.command;
  }
  
  File file = SPIFFS.open("/irsignals.json", "w");
  
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  
  file.close();
}
