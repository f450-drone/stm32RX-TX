//stm32 nrf24 transmeter test

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
    Serial.println("❌ nRF24 NOT responding! Check wiring & power.");
    while (1); // Stop execution
  }

  Serial.println("✅ nRF24 initialized successfully!");

  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.stopListening(); // TX mode
}

void loop() {
  const char msg[] = "Test";
  bool result = radio.write(&msg, sizeof(msg));

  if (result) {
    Serial.println("✔️ Message sent (but no receiver to ACK it)");
  } else {
    Serial.println("❌ Sending failed (no receiver)");
  }

  delay(1000);
}

