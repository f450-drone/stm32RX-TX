void setup() {
  Serial.begin(115200);
  Serial.println("Hello from STM32!");
}

void loop() {
  Serial.println("Looping...");
  delay(1000);
}
