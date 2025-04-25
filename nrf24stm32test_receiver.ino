//nrf24stm32 test receiver
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  PB4
#define CSN_PIN PB5

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!radio.begin()) {
    Serial.println("❌ nRF24 NOT responding on receiver side!");
    while (1);
  }

  Serial.println("✅ Receiver started, listening...");

  radio.setPALevel(RF24_PA_LOW); // Power level low for testing
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL); // Same pipe as transmitter
  radio.startListening(); // Receiver mode
}

void loop() {
  if (radio.available()) {
    char msg[32] = "";
    radio.read(&msg, sizeof(msg));
    Serial.print("📩 Received: ");
    Serial.println(msg);
  }
}

