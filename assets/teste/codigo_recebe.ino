#include <IRremote.hpp>  // Biblioteca IRremote moderna (versão 4.x ou 5.x)

#define IR_RECEIVE_PIN 15  // pino do sensor IR (ajuste se usar outro)

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando receptor IR...");
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // inicializa o receptor
  Serial.print("Receptor IR pronto no pino ");
  Serial.println(IR_RECEIVE_PIN);
}

void loop() {
  // Verifica se um sinal IR foi recebido
  if (IrReceiver.decode()) {
    Serial.println("\n=== Sinal IR Detectado ===");

    // Mostra informações básicas
    Serial.print("Endereço: 0x");
    Serial.println(IrReceiver.decodedIRData.address, HEX);
    
    Serial.print("Comando:  0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    
    Serial.print("Protocolo: ");
    Serial.println(IrReceiver.decodedIRData.protocol);

    // Mostra o valor bruto completo
    Serial.print("RawData:  0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    Serial.println("==========================");

    // Pronto para o próximo sinal
    IrReceiver.resume();
  }
}
