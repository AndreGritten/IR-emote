#include <IRremote.hpp>

#define IR_RECEIVE_PIN 15  // pino do receptor IR
#define LED_PIN 4          // pino do LED visível

void setup() {
  Serial.begin(115200);
  Serial.println("Esperando sinal IR...");
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println("Sinal IR detectado!");
    
    // Mostra os códigos no serial
    Serial.print("Endereço: 0x");
    Serial.println(IrReceiver.decodedIRData.address, HEX);
    Serial.print("Comando:  0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    
    // Simula envio: acende LED por 1 segundo
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);

    // Pronto para próximo sinal
    IrReceiver.resume();
  }
}
